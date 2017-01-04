#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
//#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

#include <dht.h>
#define DHTPIN 5
dht DHT;

#define SSID "Liang's SE"                                   
#define PASS ""  

// Choreo execution interval in milliseconds
unsigned long Shadow = 5000; 
// Store the time of the last Choreo execution
unsigned long LastSun = 0; 

unsigned long Current = 0;
unsigned long Lastwater = 0;
int flag = 0;
unsigned long LightCounter = 0;

ESP8266WebServer server(80);

WiFiClient client;

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 10;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);
  
  WiFi.begin(SSID, PASS); 
  
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  server.begin();
  Serial.println(WiFi.localIP());  
  
    

  Serial.println("Setup complete.\n");
}

void NeedWater() {
  if (numRuns <= maxRuns) {
    Serial.println("Running Post - Run #" + String(numRuns++));

    TembooChoreo PostChoreo(client);

    // Invoke the Temboo client
    PostChoreo.begin();

    // Set Temboo account credentials
    PostChoreo.setAccountName(TEMBOO_ACCOUNT);
    PostChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    PostChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String ProfileIDValue = "me";
    PostChoreo.addInput("ProfileID", ProfileIDValue);
    String MessageValue = "你的植物需要澆水，他在你背後，看起來非常火。";
    PostChoreo.addInput("Message", MessageValue);
    String AccessTokenValue = "";
    PostChoreo.addInput("AccessToken", AccessTokenValue);

    // Identify the Choreo to run
    PostChoreo.setChoreo("/Library/Facebook/Publishing/Post");

    // Run the Choreo; when results are available, print them to serial
    PostChoreo.run();
    PostChoreo.close();
  }

}

void NeedSun() {
  if (numRuns <= maxRuns) {
    Serial.println("Running Post - Run #" + String(numRuns++));

    TembooChoreo PostChoreo(client);

    // Invoke the Temboo client
    PostChoreo.begin();

    // Set Temboo account credentials
    PostChoreo.setAccountName(TEMBOO_ACCOUNT);
    PostChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    PostChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String ProfileIDValue = "me";
    PostChoreo.addInput("ProfileID", ProfileIDValue);
    String MessageValue = "你的植物孤單寂寞覺得暗，他需要太陽的溫暖。";
    PostChoreo.addInput("Message", MessageValue);
    String AccessTokenValue = "";
    PostChoreo.addInput("AccessToken", AccessTokenValue);

    // Identify the Choreo to run
    PostChoreo.setChoreo("/Library/Facebook/Publishing/Post");

    // Run the Choreo; when results are available, print them to serial
    PostChoreo.run();
    PostChoreo.close();
  }

}

void loop(){
  int light = analogRead(A0);
  Current = millis();
  DHT.read11(DHTPIN);
  float h = DHT.humidity;
  if(Current - LightCounter >= 3000){
    Serial.print("Light:");
    Serial.println(light);
    LightCounter = Current;
  }
  if(Current - Lastwater >= 7000){
  Serial.print("Humidity: ");
  Serial.println(h);

  //為Demo方便 以秒數觸發替代
/*  if(millis() - lastChoreoRunTime >= choreoInterval) {
    if(h <= 25) NeedWater(); 
  } 
*/
  NeedWater();
  Serial.println("\nFacebook post succed!\n");
  Serial.println("\nWaiting for water...\n");
  Lastwater = millis();
  }

  if(light < 500 && !flag){
    LastSun = millis();
    flag = 1;
  }
  if(light >=500){
    LastSun = Current;
    flag = 0;
  }
  
  if(Current - LastSun >= Shadow){
  Serial.println("Need Sunlight!");
  NeedSun();
  Serial.println("\nFacebook post succed!\n");
  Serial.println("\nWaiting for Sunlight...\n");
  LastSun = millis();
  }
}
