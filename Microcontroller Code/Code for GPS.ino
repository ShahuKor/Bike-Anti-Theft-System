#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h> 


// WiFi credentials
const char* ssid = " ";     // your network SSID (name)
const char* password = " "; // your network password

// ThingSpeak credentials
unsigned long myChannelNumber =  0000000 ; //enter you channel number
const char * myWriteAPIKey = " "; //enter your API key of thingspeak

// GPS module connected to ESP8266 RX and TX
TinyGPSPlus gps;

WiFiClient client;


void setup() {
  Serial.begin(9600);
  Serial.println("ESP8266 GPS Tracker");

  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
Serial.println(WiFi.localIP());
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  while (Serial.available() > 0) {
    if (gps.encode(Serial.read())) {
      if (gps.location.isValid()) {
        float lat = static_cast<float>(gps.location.lat());
        float lng = static_cast<float>(gps.location.lng());

        
        // Display on Serial Monitor
        Serial.print("Latitude: ");
        Serial.println(lat, 6);
        Serial.print("Longitude: ");
        Serial.println(lng, 6);

        // Send the data to ThingSpeak
        ThingSpeak.setField(1, lat);
        ThingSpeak.setField(2, lng);

        int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        if (x == 200) {
          Serial.println("Location sent to ThingSpeak.");
        } else {
          Serial.println("Problem sending to ThingSpeak.");
        }
      }
    }
  }
}
