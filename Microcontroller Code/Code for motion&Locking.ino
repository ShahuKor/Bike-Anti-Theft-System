#define BLYNK_TEMPLATE_ID "Enter Template ID"
#define BLYNK_TEMPLATE_NAME "Enter Template Name"
#define BLYNK_AUTH_TOKEN "Enter you token"


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TinyGPS++.h>
#include <WiFiClientSecure.h>
#include <Base64.h>
#include <ThingSpeak.h>
#include <ESP8266HTTPClient.h> // Add this library for HTTP requests

char auth[] = "Enter your authentication token";
const char *ssid = "your hotspot ID";
const char *password = "your hotspot password";

const int RELAY_PIN = D4;
const int VIBRATION_SENSOR_PIN = D6;

// ThingSpeak credentials
unsigned long myChannelNumber =  //Enter your channel number ;
const char * myWriteAPIKey = "Enter your API key";

TinyGPSPlus gps;
WiFiClient client;

BlynkTimer timer;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(VIBRATION_SENSOR_PIN, INPUT_PULLUP); // Assuming it's active LOW, use INPUT_PULLUP
  
  Blynk.begin(auth, ssid, password);

  timer.setInterval(10000L, sendGPSData); // Send GPS data every 10 seconds
  Serial.println("Setup completed. Ready to send GPS data to ThingSpeak.");
  
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  Blynk.run();
  timer.run();
  readVibrationSensor(); // Read vibration sensor state continuously
  sendGPSData();
}

void sendGPSData() {
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

BLYNK_WRITE(V0) { // Virtual pin V0 for controlling relay
  int state = param.asInt();
  digitalWrite(RELAY_PIN, state);
  Serial.print("Relay state set to: ");
  Serial.println(state);
}

void readVibrationSensor() {
  int vibrationState = digitalRead(VIBRATION_SENSOR_PIN);
  
  if (vibrationState == LOW) {
    Serial.println("No vibration detected.");
  } else {
    // Log theft event
    Blynk.logEvent("theft", "Vibration Detected");
    Serial.println("Vibration detected. Theft event logged.");
  }
}
