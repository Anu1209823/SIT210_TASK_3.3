#include <ArduinoMqttClient.h>

// Check the type of Arduino board and include the appropriate WiFi library
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

char ssid[] = "IBN-B";    // Your WiFi network SSID (name)
char pass[] = "CUPunjab";    // Your WiFi network password (WPA or WEP key)

int light = 2;  // Pin for an LED that indicates message reception

// Create a WiFiClient instance and an MqttClient instance using WiFiClient
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "mqtt-dashboard.com";  // MQTT broker address
int port = 1883;  // Port number for MQTT communication
const char topic[] = "Task3.3D_Task210";  // MQTT topic to subscribe to

void setup() {
  Serial.begin(9600);  // Initialize serial communication

  // Set the LED pin as an output
  pinMode(light, OUTPUT);

  // Attempt to connect to the WiFi network
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");  // Display dots while attempting to connect
    delay(5000);  // Wait for 5 seconds before retrying
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // Attempt to connect to the MQTT broker
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);  // Infinite loop if MQTT connection fails
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // Subscribe to the specified MQTT topic
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  mqttClient.subscribe(topic);  // Subscribe to the topic
}

void loop() {
  int messageSize = mqttClient.parseMessage();  // Check for incoming MQTT messages

  if (messageSize) {
    // Print information about received MQTT messages
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    // Print the contents of the received message
    while (mqttClient.available()) {
      Serial.print((char)mqttClient.read());
    }
    
    Serial.println();
    
    // Toggle the LED on and off to indicate message reception
    digitalWrite(light, HIGH);
    delay(200);
    digitalWrite(light, LOW);
    delay(200);
    digitalWrite(light, HIGH);
    delay(200);
    digitalWrite(light, LOW);
    delay(200);
    digitalWrite(light, HIGH);
    delay(200);
    digitalWrite(light, LOW);

    Serial.println();
  }
}
