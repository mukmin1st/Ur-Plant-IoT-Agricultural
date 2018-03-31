/*
   Ur-Plant is Agricultural Project Section based on Internet of Things
   Ur-Plant.ino
   Created  by Mukmin on 11/08/2016
   Copyright © 2016 Mukmin. All rights reserved.
*/

#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;

const char *ssid = "SSID-wifi";
const char *pass = "PASS-WIFI";

String isi;
double temp, light, water;

void connect();

void setup () {
  Serial.begin (9600);
  WiFi.begin(ssid, pass);
  client.begin("Server/ip-Server", net);
  pinMode(2, OUTPUT);
  connect();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nconnecting...");
  while (!client.connect("Client-Id","Username", "Password")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe("admintes/urplant/force");
}


void loop () {
  DynamicJsonBuffer jsonBuffer;
  if (Serial.available() > 0) {
    isi = Serial.readString();
    Serial.println(isi);
    JsonObject& root = jsonBuffer.parseObject(isi);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }

    temp = root["temp"];
    light = root["light"];
    water = root ["water"]
  } else {
    //    Serial.print(isi);
    Serial.println("  gagal");
  }

  Serial.println("Data suhu : " + String(temp));
  Serial.println("Data cahaya : " + String(light));
  Serial.println("Data air : " + String(water));
  delay(1000);
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("admintes/urplant/data1", String(temp));  //Light Intensity
    client.publish("admintes/urplant/data2",String (water)); //Water Intensity
    client.publish("admintes/urplant/data3", String(light)); //Temperature
    
  }
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
    if (payload == '5'){
      digitalWrite(2, HIGH); //open valve for irrigation
      delay (5000);
    }else{
       digitalWrite(2, LOW); //close valve for hold irrigation
      delay (5000);
    }
  Serial.println();
}
