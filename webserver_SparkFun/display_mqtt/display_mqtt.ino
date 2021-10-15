#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SparkFun_Bio_Sensor_Hub_Library.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


const char *ssid = "aritejh";
const char *PWD = "fvvs4503";

char data[100];

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

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

//Display setup
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
#define BUTTON_A 1
#define BUTTON_B 21
#define BUTTON_C 14


uint32_t last_time = 0;

// Reset pin, MFIO pin
int resPin = 32;
int mfioPin = 33;

SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body;  
void setup() {
  
  Serial.begin(9600);
  Wire.begin();
  
  //Sensor setup
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

  //Wifi setup
  Serial.println("Connecting to ");
  Serial.println(ssid);
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

  //Display setup

  Serial.println("OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  //Serial.println("IO test");

  //pinMode(BUTTON_A, INPUT_PULLUP);
  //pinMode(BUTTON_B, INPUT_PULLUP);
  //pinMode(BUTTON_C, INPUT_PULLUP);

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Connecting to SSID\n'adafruit':");
  display.println("IP: 10.0.1.23");
  display.println("Sending val #0");
  display.setCursor(0,0);
  display.display(); // actually display all of the above
  display.clearDisplay(); 
  display.setCursor(0,0);
  display.print(" HR ");
  display.setCursor(30,0);
  display.print("Conf");
  display.setCursor(60,0);
  display.print(" O2 ");
  display.setCursor(90,0);
  display.print("Stat");

}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "clientId-t1L0h0i8gz";
      clientId += String(random(0xffff), HEX);
      Serial.println(clientId);
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("/swa/commands");
      }
      
  }
}




void displayData()
{
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0,16);
  display.print(" ");
  display.print(body.heartRate);
  display.print(" ");
  display.setCursor(30,16);
  display.print(" ");
  display.print(body.confidence);
  display.print(" ");
  display.setCursor(60,16);
  display.print(" ");
  display.print(body.oxygen);
  display.print(" ");
  display.setCursor(90,16);
  display.print(" ");
  display.print(body.status);
  display.display();
  Serial.print("Heartrate: ");
  Serial.println(body.heartRate); 
  Serial.print("Confidence: ");
  Serial.println(body.confidence); 
  Serial.print("Oxygen: ");
  Serial.println(body.oxygen); 
  Serial.print("Status: ");
  Serial.println(body.status);
}


void loop() {
  if (!mqttClient.connected())
    reconnect();
  mqttClient.loop();
  long now = millis();
  if (now - last_time > 2000) {
    // Send data
    body= bioHub.readBpm();
    displayData();
    
    float BPM = body.heartRate;
    float SpO2= body.oxygen;
    float Confidence = body.confidence;
    float Status = body.status; 
    
    // Publishing data throgh MQTT
    sprintf(data, "%f", BPM);
    Serial.print("Heartrate: ");
    Serial.println(BPM);
    mqttClient.publish("/swa/Heartrate", data);
    sprintf(data, "%f", SpO2);
    Serial.print("Oxygen: ");
    Serial.println(SpO2);
    mqttClient.publish("/swa/SpO2", data);
    sprintf(data, "%f", Confidence);
    Serial.print("Confidence: ");
    Serial.println(body.confidence);
    mqttClient.publish("/swa/confidence", data);
    sprintf(data, "%f", Status);
    Serial.print("Status: ");
    Serial.println(body.status);
    mqttClient.publish("/swa/status", data);
    last_time = now;
    delay(250);
  }
}
