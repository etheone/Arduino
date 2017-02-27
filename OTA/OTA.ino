#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>

/*#include <ESP8266WiFi.h>*/
#include <ESP8266WiFiMulti.h>
#include <DHT.h>
#define DHTTYPE DHT11
#define DHTPIN  2

ESP8266WiFiMulti WiFiMulti;

int i = 0;
float humidity, temp_c;
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  WiFiMulti.addAP("Joels-iPhone", "qweqwe123");

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void gettemperature() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_c = dht.readTemperature();     // Read temperature as Fahrenheit
    if (isnan(humidity) || isnan(temp_c)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}

boolean httpPost(char * host, uint16_t port, char * url)
{
  HTTPClient http;
  bool ret = false;
  bool httpCode;

  unsigned long start = millis();

  String toSend = "{\"deviceId\": \"9f8bc3bc-b8ce-4e8a-b91b-973fed280ff4\", \"data\": { \"temp\": " + String(temp_c, DEC) + ", \"humid\": " + String(humidity, DEC) + "}}";
  http.begin(host, port, url);

  http.addHeader("Content-Type", "application/json");
  httpCode = http.POST(toSend);
  http.writeToStream(&Serial);
  http.end();
  if (httpCode == 200) {
    return true;
  } else {
    return false;
  }
}

void loop() {

  if (i % 5 == 0) {
    t_httpUpdate_return ret = ESPhttpUpdate.update("http://www.devota.se/api/OTA/update.bin");
  
    switch(ret) {
      case HTTP_UPDATE_FAILED:
        Serial.println("HTTP_UPDATE_FAILED Error (%d): %s\n");
        Serial.println(ESPhttpUpdate.getLastError());
        Serial.println(ESPhttpUpdate.getLastErrorString().c_str());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;
      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
  
  uint16_t port = 80;
  char* host = "devota.se"; // ip or dns
  char* route = "/api/data/addreal"; //route

  Serial.println("Trying to read temperature");
  gettemperature();

  Serial.println("Trying to do POST request");
  bool httpCode = httpPost(host, port, route);

  Serial.println("Was POSTrequest OK? " + httpCode);

  i++;

  Serial.println("wait 10 seconds...");
  delay(10000);
}




