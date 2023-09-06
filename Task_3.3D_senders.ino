/*This line includes the "ArduinoMqttClient" library, which is essential for MQTT communication in Arduino projects. 
It provides functions and tools for connecting to MQTT brokers and managing MQTT messages.*/

#include <ArduinoMqttClient.h>
/*This section checks which type of Arduino board is being used. Depending on the board, it includes the appropriate
 WiFi library. Arduino boards have different WiFi modules, and each requires a specific library for WiFi connectivity. 
 In this case, it includes the "WiFiNINA.h" library if the board is one of the mentioned types, like MKR WiFi 1010,
  Nano 33 IoT, or UNO WiFi Rev2.*/

#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>

/*Similarly, if the Arduino board is a SAMD MKR1000, it includes the "WiFi101.h" library. 
Different WiFi modules require different libraries.*/

#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>

/*If the board is based on the ESP8266 architecture (like the NodeMCU or ESP-01), it includes the "ESP8266WiFi.h" library, 
which is specific to ESP8266-based boards.*/

#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>

/*Finally, if the board is an ESP32-based board, it includes the "WiFi.h" library, designed for ESP32 devices.*/
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif


char ssid[] = "IBN-B"; // your network SSID (name)
char pass[] = "CUPunjab";    // your network password (use for WPA, or use as key for WEP)
const int trigPin = 2;
const int echoPin = 3;

float duration, distance;


//This line creates an instance of the WiFiClient class named wifiClient. The WiFiClient class is part of the Arduino core 
//library for ESP8266 and ESP32 boards and is used for handling Wi-Fi connections. It allows your Arduino device to 
//connect to a Wi-Fi network.
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


/*This line defines a character array named broker and assigns it the value "mqtt-dashboard.com". 
This is the hostname or IP address of the MQTT broker that you want to connect to. In MQTT communication, 
the broker is the central server that facilitates the exchange of messages between clients.*/
const char broker[] = "mqtt-dashboard.com";
/*The port variable represents the port number on which the MQTT broker is listening. */
int        port     = 1883;

/*The topic variable represents the MQTT topic to which our Arduino will publish messages or subscribe for 
incoming messages. In our code, it's set to "wave," so our Arduino will interact with the MQTT broker's "wave" topic.*/

const char topic[]  = "wave";

//This tells the Arduino to wait for 1000 milliseconds (1 second) before doing something. It's like setting a timer.
const long interval = 1000;
//This is like starting a stopwatch at zero. We'll use it to measure time.
unsigned long previousMillis = 0;
//Think of this as a counter that keeps track of how many times something has happened, like counting how many laps you've run.
int count = 0;

void setup() {
  
  /*This line sets up communication with your computer. It's like making a phone call. You're saying, 
  "I want to talk to my computer, and I want to use a certain speed to talk." In this case, the speed is 9600 bits per second, 
  which is a common speed for Arduino to talk to your computer.*/

  Serial.begin(9600);
  /*This part is like waiting for your computer to pick up the phone call. When you first connect your Arduino to your computer, 
  it takes a moment to set up the connection. The Arduino is patiently waiting until your computer is ready to talk.*/
  while (!Serial) {
    ; 
  }

  
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)//Think of this as a loop that repeats something as long as a condition is true.
  {
    
    Serial.print(".");// It's a way for the Arduino to tell you that it's still trying. Each dot represents an attempt.
    delay(5000);//After trying, it waits for 5 seconds before trying again. It's like waiting a moment before trying to log in again if you fail.
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  /*The Arduino is printing the value of the broker variable, which contains the address (or name) of the MQTT broker. 
  So, it's showing you the specific broker it's trying to connect to.*/
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    /*This is an infinite loop. If the MQTT connection fails, the Arduino will get stuck in this loop, 
    and the program will not proceed further. It's like saying, "Stop here and don't do anything else until 
    the issue is resolved."*/
    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {

  //Here, the Arduino is calling a function called mqttClient.poll(). This function helps maintain the MQTT 
  //connection by sending "keep-alive" messages to the MQTT broker. It's like saying, "Hey, I'm still here!" 
  //to the broker so it doesn't disconnect.
  mqttClient.poll();

  //The Arduino is using a strategy to avoid long delays in the loop(). It's similar to the "BlinkWithoutDelay" example provided
  //with Arduino.millis() is a function that gives the current time in milliseconds, and the Arduino stores it in the 
  //currentMillis variable.
  unsigned long currentMillis = millis();

  //The code inside the block you see in the if statement is doing the following:
  //Using an ultrasonic sensor to measure distance.
  //Checking if the measured distance is less than 12 units.
  //If it's less than 12, it sends an MQTT message with information about the distance.

  if (currentMillis - previousMillis >= interval)
  {
    //This line is saving the current time, as measured by millis(), into the previousMillis variable. 
    //It's like taking a snapshot of the time right now and storing it for reference.
    previousMillis = currentMillis;

    //These lines are controlling an ultrasonic sensor. The sensor has two parts: a transmitter (trigger) and a receiver (echo).
    //digitalWrite(trigPin, LOW); sets the trigger pin to a low voltage, preparing it to send a signal.
    digitalWrite(trigPin, LOW);
    //waits for a very short time (2 microseconds) before the next step.
    delayMicroseconds(2);
    //sets the trigger pin to a high voltage, which sends out an ultrasonic pulse.
    digitalWrite(trigPin, HIGH);
    //waits for a slightly longer time (10 microseconds) for the pulse to bounce off an object and return.
    delayMicroseconds(10);
    //This sequence of commands essen
    digitalWrite(trigPin, LOW);

    //This line is measuring how long it takes for the ultrasonic pulse to bounce back and be detected by the sensor.
    duration = pulseIn(echoPin, HIGH);
    //After measuring the duration, this part calculates the distance to the object based on the time it took for the sound wave to bounce back.
    distance = (duration * .0343) / 2;
    Serial.print("Distance: ");
    Serial.println(distance);
    //This line checks whether the distance variable, which was calculated earlier using the ultrasonic sensor, 
    //is less than 12 units. It's like asking, "Is the object closer than 12 units?"
    //this part of the code checks if an object is closer than 12 units. If it is, it creates an MQTT message that says, 
    //"Anushika chauhan: Wave is detected," followed by the measured distance. This message is then sent to the MQTT broker's 
    //"wave" topic, with a 1-second delay before potentially sending another message. It's like letting someone know that 
    //you've detected something and providing details about it.
    if (distance < 12)
    {
      
      mqttClient.beginMessage(topic);
      mqttClient.print("Anushika chauhan: Wave is detected, ");
      mqttClient.print("Distance: ");
      mqttClient.print(distance);
      mqttClient.endMessage();
      delay(1000);
    }

    Serial.println();

    count++;
  }
}