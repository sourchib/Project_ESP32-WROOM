// Library Wifi
#include "WiFi.h"
WiFiClient client;

// Library DHT11 Sensor
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11

// Deklarasi variabel DHT11
DHT dht(DHTPIN, DHTTYPE);

// Deklarasi variabel thingspeak
String thingSpeakAddress = "api.thingspeak.com";
String writeAPIKey;
String tsfield1Name;
String request_string;
// Library BLE
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
// define uuid BLE
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

//Setup setting BLE dan WiFi
void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32 WROOM BLE");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Berhasil Konek ke BLC ESP32");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Smart Phone Woke");

  //WiFi Clinet Connect...
  
   Serial.begin(9600);
   WiFi.disconnect();
   WiFi.begin("WiFi", "saya2133");
   while ((!(WiFi.status() == WL_CONNECTED))) {
   delay(300);
   Serial.print(".");
   }
   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   dht.begin();
}

// Loop proses data sensor dht11 
void loop() {
   delay(2000);
   float h = dht.readHumidity();
   float t = dht.readTemperature();
   float f = dht.readTemperature(true);
   // pemangil fungsi void thingspeak.....
   kirim_thingspeak(t, h);
   if (isnan(h) || isnan(t) || isnan(f)) {
   Serial.println("Sensor Tidak Terbaca");
   return;
   }
}

//fungsi thingspeak.. sensor
void kirim_thingspeak(float suhu, float hum) {
   if (client.connect("api.thingspeak.com", 80)) {
   request_string = "/update?";
   request_string += "key=";
   request_string += "Z74CCGG3A8MEAZ8G"; //key thingspeak....
   request_string += "&";
   request_string += "field1";
   request_string += "=";
   request_string += suhu; //variabel void dan pada thingspeak..
   request_string += "&";
   request_string += "field2";
   request_string += "=";
   request_string += hum; //variabel void dan pada thingspeak..

   // Get manampilkan ke Serial http
   Serial.println(String("GET ") + request_string + " HTTP/1.1\r\n" +
   "Host: " + thingSpeakAddress + "\r\n" +
   "Connection: close\r\n\r\n");

  // Get manampilkan ke HTTP...
   client.print(String("GET ") + request_string + " HTTP/1.1\r\n" +
   "Host: " + thingSpeakAddress + "\r\n" +
   "Connection: close\r\n\r\n");
   unsigned long timeout = millis();
   while (client.available() == 0) {
   if (millis() - timeout > 5000) {
   Serial.println(">>> Client Timeout !");
   client.stop();
    return;
   }
   }
   while (client.available()) {
   String line = client.readStringUntil('\r');
   Serial.print(line);
   }
   Serial.println();
   Serial.println("Terhubung ke Thingspeek");
   }
}
