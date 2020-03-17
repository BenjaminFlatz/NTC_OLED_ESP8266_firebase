#include "DHT.h"

#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>   


#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 5, /* data=*/ 4, /* reset=*/ 16);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED


#define FIREBASE_HOST "colorrize-a1a0f.firebaseio.com"
#define FIRMWARE_URL "https://firebasestorage.googleapis.com/v0/b/colorrize-a1a0f.appspot.com/o/mqtt.ino.bin?alt=media&token=b2ec949f-46d8-4051-8ac4-3a06bb159008"
#define FIREBASE_AUTH "mv5r81n16mLY6nxPKlSAHMc8xBXZ2HS6HVCwVkkt"

#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);
String header;

FirebaseData firebaseData;
FirebaseJson json;
String path = "/HC-SR04";


float temp;
float humid;

void setup() {  
  pinMode(DHTPIN, INPUT_PULLUP);                // Trigger Pin als Ausgang definieren

  u8g2.begin();
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();

  WiFiManager wifiManager;
  
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  
  server.begin();

  /*
  WiFi.begin(SSID, PSK);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  */
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  
  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

}

void firebase_setString(String pathData, String data){
  
  //Firebase.setInt("/HC-SR04/roman/distance", distance); 
  if (Firebase.setString(firebaseData, pathData, data))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.print("PUSH NAME: ");
    Serial.println(firebaseData.pushName());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
    
}
float read_temp(){

  const double BETA = 3974.0;
  const double ROOM_TEMP = 298.15;
  
  //float temp = (analogRead(A0)*0.004882813);
  float res = 1000*(3.3/(analogRead(A0)*0.00322265625)-1);
  float temp = (BETA * ROOM_TEMP) / (BETA + (ROOM_TEMP * log(res/10000)))-273.15;

  Serial.println(temp);
  return temp;


}
void read_dht(){

  
  temp = dht.readTemperature(); // Gets the values of the temperature
  humid = dht.readHumidity();
  
  Serial.println(String(temp));
  Serial.println(String(humid));


  
}
void loop() {

  temp = read_temp();
  
  u8g2.clearBuffer();         // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.setCursor(0, 10);
  u8g2.print("Temp: "+String(temp)+"°C"); 
  //u8g2.drawStr(5,8,"°C");// write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000); 
  firebase_setString("/wrusl/growbox/temp", String(temp));
  
}
