#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SparkFun_Bio_Sensor_Hub_Library.h>

const char *ssid = "RiceCooker(2.4G)";
const char *PWD = "HotSteamingRice30";

char data[100];



// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}




uint32_t last_time = 0;

// Reset pin, MFIO pin
int resPin = 32;
int mfioPin = 33;

SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body;  
void setup() {
  Serial.begin(9600);
  Wire.begin();
  int result = bioHub.begin();
  if (result == 0) //Zero errors!
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!!!");
 
  Serial.println("Configuring Sensor...."); 
  int error = bioHub.configBpm(MODE_ONE); // Configuring just the BPM settings. 
  if(error == 0){ // Zero errors
    Serial.println("Sensor configured.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }
  
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, PWD);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  setupMQTT();
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("/swa/commands");
      }
      
  }
}


void loop() {
  if (!mqttClient.connected())
    reconnect();
  mqttClient.loop();
  long now = millis();
  if (now - last_time > 2000) {
    // Send data
    body= bioHub.readBpm();
    
    float BPM = body.heartRate;
    float SpO2= body.oxygen; 
    // Publishing data throgh MQTT
    sprintf(data, "%f", BPM);
    Serial.print("Heartrate: ");
    Serial.println(BPM);
    mqttClient.publish("/swa/Heartrate", data);
    sprintf(data, "%f", SpO2);
    Serial.print("Oxygen: ");
    Serial.println(SpO2);
    mqttClient.publish("/swa/SpO2", data);
    Serial.print("Confidence: ");
    Serial.println(body.confidence); 
    Serial.print("Status: ");
    Serial.println(body.status);
    last_time = now;
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}
