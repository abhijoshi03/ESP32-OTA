#include <Update.h>
#include <WiFi.h>
#include "HTTPClient.h"
#include <FirebaseESP32.h>
#define FIREBASE_HOST "ota-update-b4160.firebaseio.com"
#define FIREBASE_AUTH "YclFTkB1djOYCmrncjDclkqsv3QT59y0wDrlix0d"
int reader;
int switch_const,switch_const2,switcher = 1;
const char* ssid = "Abhi";
const char* password = "12345678";
HTTPClient http;
String FIRMWARE_URL;
void main_program();
void firmware_update(String FIRMWARE_URL);
FirebaseData firebaseData;

void setup() {
  //////////////////functional program//////////////////
  pinMode(2, OUTPUT);
  //--------//--------//--------//--------//--------//
  //////////////////OTA Program///////////////////////

    Serial.begin(115200);
    Serial.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    if(WiFi.waitForConnectResult() == WL_CONNECTED)
    {
        Serial.println("WiFi connected");
    }

  //////////// Firebase retrieve url ////////////
    Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);

    if (Firebase.getString(firebaseData, "/firmware")) {

        if (firebaseData.dataType() == "string") {
          FIRMWARE_URL = firebaseData.stringData();
         }
    } 
    else {
    Serial.println(firebaseData.errorReason());
    }
  //////////// Firebase retrieve command1 ////////////
    if (Firebase.getInt(firebaseData, "/command")) {

        if (firebaseData.dataType() == "int") {
          switch_const = firebaseData.intData();
         }
    } 
    else {
    Serial.println(firebaseData.errorReason());
    }
    
  //////////// Firebase retrieve command1 ////////////
    if (Firebase.getInt(firebaseData, "/command2")) {

        if (firebaseData.dataType() == "int") {
          switch_const2 = firebaseData.intData();
         }
    } 
    else {
    Serial.println(firebaseData.errorReason());
    }

    Firebase.deleteNode(firebaseData, "/command2");

    if(switch_const && switch_const2 == 1)
    {
      switcher = 2;
    }
    else
    {
      switcher = 1;
    }

    
while(1){
  switch(switcher){
    case 1: main_program();
    case 2: firmware_update(FIRMWARE_URL);
    }
  }

}

 void firmware_update(String FIRMWARE_URL)
 {

    //// sending data //////
    Serial.println("sending data..");
    // Start pulling down the firmware binary.
    http.begin(FIRMWARE_URL);
    int httpCode = http.GET();
    if (httpCode <= 0) {
      Serial.printf("HTTP failed, error: %s\n", 
      http.errorToString(httpCode).c_str());
      return;
    }
    // Check that we have enough space for the new binary.
    int contentLen = http.getSize();
    Serial.printf("Content-Length: %d\n", contentLen);
    bool canBegin = Update.begin(contentLen);
    if (!canBegin) {
      Serial.println("Not enough space to begin OTA");
      return;
    }
    // Write the HTTP stream to the Update library.
    WiFiClient* client = http.getStreamPtr();
    size_t written = Update.writeStream(*client);
    Serial.printf("OTA: %d/%d bytes written.\n", written, contentLen);
    if (written != contentLen) {
      Serial.println("Wrote partial binary. Giving up.");
      return;
    }
    if (!Update.end()) {
      Serial.println("Error from Update.end(): " + 
      String(Update.getError()));
      return;
    }
    if (Update.isFinished()) {
      Serial.println("Update successfully completed. Rebooting."); 
      // This line is specific to the ESP32 platform:
      ESP.restart();
    }
    else {
      Serial.println("Error from Update.isFinished(): " + 
      String(Update.getError()));
      return;
    }
  
 }

 void main_program()
 {
  while(1) {
      digitalWrite(2, HIGH);   
      delay(500);              
      digitalWrite(2, LOW);    
      delay(500);  
  }         
 }

void loop() {
}
