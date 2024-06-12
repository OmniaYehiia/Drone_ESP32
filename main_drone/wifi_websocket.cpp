#include <WiFi.h> //use for wifi setup
#include <WiFiClient.h> //use for wifi setup
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <PubSubClient.h> //use for mqtt setup
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
/*********************WIFI (user_name & password)******************************/
#define WIFI_SSID "user_name" 
#define WIFI_PASSWORD "password"
/******************************MQTT********************************************/
const char* mqttServer = "mqtt-dashboard.com";
const char* clientID = "nnUwnjr8HP"; // Client ID username+0001
const char* topic = "Smart_Flight_Drone_2000"; // Publish topic
// Setting up WiFi and MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

bool wsConnected;
int controlReference[5]; // throttle, yaw, pitch, roll , active
/***********************Connected To WI-FI**************************************/
void connectWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}
/***********************Set Callback Function**********************************/
void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived in topic: ");
  //Serial.println(topic);
  //Serial.print("Message: ");
  String data = "";
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    data += (char)payload[i];
  }
  //Serial.println();
  //Serial.print("Message size: ");
  //Serial.println(length);
  //Serial.println();
  //Serial.println("-----------------------");
  //Serial.println(data);

  DynamicJsonDocument doc(150);
  
  // msg "{\"throttle\":150,\"yaw\":20,\"pitch\":25,\"roll\":25}"
  //const char* msg = data;
                    //{"throttle":116,"yaw":0,"pitch":0,"roll":88}
  deserializeJson(doc,data);
  
  controlReference[0] = doc["throttle"];
  controlReference[1] = doc["yaw"];
  controlReference[2] = doc["pitch"];
  controlReference[3] = doc["roll"];
  controlReference[4] = doc["active"];
  
}
/***********************Connect To HiveMq Function**************************/
void connecthivemq(){
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}
/***********************Subscribe function**************************/
void connectWebsocket() {
  while (!client.connected()) {
    if (client.connect(clientID)) {
      Serial.println("MQTT connected");
      client.subscribe(topic);
      Serial.println("Topic Subscribed");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}

void client_loop(){
  client.loop();
}
/**********************If client connected***************************/
bool serverconnect(){
  if (client.connected()) { // If client connected
    return true; 
  }
  else{
    return false;
  }
}
/***********************Get Control Reference**************************/
int* getControlReference(){
  return controlReference;
}
