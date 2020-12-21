#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h"
#include "WiFi.h" //ganti karakter "" dengan '>' jika error
WiFiClient client;
#define DHTPIN 15
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);
String thingSpeakAddress = "api.thingspeak.com";
String writeAPIKey;
String tsfield1Name;
String request_string;
void setup()
{
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
void loop()
{
 delay(2000);
 float h = dht.readHumidity();
 // Read temperature as Celsius (the default)
 float t = dht.readTemperature();
 // Read temperature as Fahrenheit (isFahrenheit = true)
 float f = dht.readTemperature(true);
 kirim_thingspeak(t, h);
 if (isnan(h) || isnan(t) || isnan(f)) {
 Serial.println("Failed to read from DHT sensor!");
 return;
 }
}
void kirim_thingspeak(float suhu, float hum) {
 if (client.connect("api.thingspeak.com", 80)) {
 request_string = "/update?";
 request_string += "key=";
 request_string += "Z74CCGG3A8MEAZ8G";
 request_string += "&";
 request_string += "field1";
 request_string += "=";
 request_string += suhu;
 request_string += "&";
 request_string += "field2";
 request_string += "=";
 request_string += hum;
 Serial.println(String("GET ") + request_string + " HTTP/1.1\r\n" +
 "Host: " + thingSpeakAddress + "\r\n" +
 "Connection: close\r\n\r\n");

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
 Serial.println("closing connection");
 }
}
