#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SparkFun_Bio_Sensor_Hub_Library.h>
//display
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <time.h>
topic
//wifi details
const char *ssid = "aritejh";
const char *PWD = "fvvs4503";

//patient data
char sensorID[] = "123";
int spo2lowerlimit = 95;
int BPMlowerlimit = 80;
int BPMupperlimit = 135;

//time stuff
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

//change limit
//void change_limit(byte* payload) {

//}
//change alert mode
//void change_alert_mode() {
  
//}

//MQTT callback (used below)
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  char limit_topic[22];
  //if (String(topic) == "wearatals/123/limit") {
    //change_limit(payload);
  //}
  //if (String(topic) == "wearatals/123/alert") {
    //change_alert_mode();
  //}
}

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
char *mqttServer = "broker.mqttdashboard.com";
int mqttPort = 1883;
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

//Display setup
Adafruit_SH1107 display = Adafruit_SH1107(64,128, &Wire);
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14


uint32_t last_time = 0;

// Reset pin, MFIO pin
int resPin = 12;
int mfioPin = 13;

SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 
bioData body;  

void setup() {
  
  Serial.begin(9600);
  Wire.begin();

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

  //Display setup

  Serial.println("OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.begin(0x3C, true); // Address 0x3C for 128x64

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
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.print("Connecting to SSID\n'adafruit':");
  display.println("IP: 10.0.1.23");
  display.println("Sending val #0");
  display.setCursor(0,0);
  display.display(); // actually display all of the above
  display.clearDisplay(); 
  display.setCursor(10,0);
  display.print(" HR ");
  display.setCursor(10,40);
  display.print(" O2 ");
  display.setCursor(10,80);
  display.print("Stat");
  display.display();
  
  setupMQTT();
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  

}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "clientId-";
      clientId += String(random(0xffff), HEX);
      Serial.println(clientId);
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        
// subscribe to topic
        char subtopic[25];
        sprintf(subtopic, "wearatals/%s/#", sensorID);
        mqttClient.subscribe(subtopic);
      }
      
  }
}

void displayData(int BPM, int SpO2, int Status)
{
  display.setTextColor(SH110X_WHITE, SH110X_BLACK);
  display.setCursor(10,20);
  display.print(BPM);
  display.setCursor(10,60);
  display.print(SpO2);
  display.setCursor(10,100);
  display.print(Status);
  display.display();
}

char BPMtopic[35];
char SpO2topic[35];
char mqttBPM[300];
char mqttSpO2[300];

void sendmqtt() {
  if (!mqttClient.connected())
  {
    reconnect();
    }
  sprintf(BPMtopic, "wearatals/data/%s/Heartrate", sensorID);
  sprintf(SpO2topic, "wearatals/data/%s/SpO2", sensorID);
  mqttClient.loop();
  
  //send data
  mqttClient.publish(BPMtopic, mqttBPM);
  mqttClient.publish(SpO2topic, mqttSpO2); 
  Serial.println("reading published");
    
  //clear arrays
  memset(mqttBPM, 0, sizeof(mqttBPM));
  memset(mqttSpO2, 0, sizeof(mqttSpO2));
}  


//send alert function, input params topic as str and reading as str
void sendalert(char *topic, char *reading) {
  if (!mqttClient.connected())
  {
    reconnect();
  }
  char alert_topic[35];
  sprintf(alert_topic, "wearatals/alert/%s/%s", sensorID, topic);

// insert display print

  mqttClient.loop();
  mqttClient.publish(alert_topic, reading);
}

//void loop
int last_reading = 0;
int last_sendmqtt = 0;
int reading_time = 1000;
int sendmqtt_time = 15000;
bool alert_mode = false;

void loop() {
  int now = millis();
//listen for change in alert levels
  if (now - last_reading >= reading_time) {
    body= bioHub.readBpm();
    int BPM = body.heartRate;
    int SpO2= body.oxygen;
    int Confidence = body.confidence;
    int Status = body.status; 
    last_reading = now;

    displayData(BPM, SpO2, Status);
    
    //filter values
    if (Confidence >= 90 && BPM != 0 && SpO2 != 0 && Status == 3){
      
      //get timestamp
      struct tm timeinfo;
      if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
      }
      char timestamp[14];
      //returns str of "day_of_yearTime
      strftime( timestamp, sizeof( timestamp ), "%j%T", &timeinfo );
      
      //append readings to array to send via mqtt 
      char BPMstr[18];
      sprintf(BPMstr, "%s,%i,", timestamp, BPM);
      strcat(mqttBPM, BPMstr);
      char SpO2str[18];
      sprintf(SpO2str, "%s,%i,", timestamp, SpO2);
      strcat(mqttSpO2, SpO2str);
      
      
      //BPM and SpO2 alert
      if (BPMlowerlimit>=BPM || BPM>=BPMupperlimit) {
        sendalert("/Heartrate",BPMstr);
      }

      if (SpO2 <= spo2lowerlimit) {
          sendalert("/SpO2", SpO2str);
      }
      
      //serial print
      Serial.print("Heartrate: ");
      Serial.println(BPM); 
      Serial.print("Oxygen: ");
      Serial.println(SpO2);
      Serial.print("Confidence: ");
      Serial.println(Confidence); 
      Serial.print("Status: ");
      Serial.println(Status);
    
    }
  //send to mqtt every 15s
  if (now - last_sendmqtt >= 15000 && strcmp(mqttBPM, "") != 0 && strcmp(mqttSpO2, "") != 0 ) {
    sendmqtt();
    last_sendmqtt = now;
  }
  Serial.println("reached end of void loop");
  }
}
    
   
