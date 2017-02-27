#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>

/*
    This sketch sends a message to a TCP server

*/

/*



   WORKING!
   TO UPLOAD:
    ESP -- MIDDLE -- MEGA
    VCC --------------- 3.3V
    RESET ------------- NOT CONNECTED
    CH_PD -- 10K RES -- 3.3V
    TX ---------------- TX
    RX ---------------- RX
    GPIO 0 - 10K RES -- GND
    GPIO2 ------------- NOT CONNECTED
    GND --------------- GND

    TO RUN UPLOADED SKETCH (AND NOT ENTER BOOT/FLASH MODE:
    DISCONNECT GPIO0 - 10K RES - GND!

*/


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <DHT.h>
#define DHTTYPE DHT11
#define DHTPIN  2

ESP8266WiFiMulti WiFiMulti;

float humidity, temp_c;
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266
//StaticJsonBuffer<200> jsonBuffer;

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

  t_httpUpdate_return ret = ESPhttpUpdate.update("http://www.devota.se/api/OTA/update");

  switch(ret) {
    case HTTP_UPDATE_FAILED:
      break;
    case HTTP_UPDATE_NO_UPDATES:
      break;
    case HTTP_UPDATE_OK:
      break;
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_c = dht.readTemperature();     // Read temperature as Fahrenheit
    // Check if any reads failed and exit early (to try again).
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
  /*JsonObject& data = jsonBuffer.createObject();
  data["deviceId"] = "9f8bc3bc-b8ce-4e8a-b91b-973fed280ff4";
  data["temp"] = temp_c;
  data["humidity"] = humidity;

  char buffer[256];
  data.prettyPrintTo(buffer, sizeof(buffer));*/
  String toSend = "{\"deviceId\": \"9f8bc3bc-b8ce-4e8a-b91b-973fed280ff4\", \"data\": { \"temperature\": " + String(temp_c, DEC) + ", \"humidity\": " + String(humidity, DEC) + "}}";
 // Serial.println("Buffer is: " + buffer);
  // configure target server and url
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
  uint16_t port = 80;
  char* host = "devota.se"; // ip or dns
  char* route = "/api/data/addreal"; //route

  Serial.println("Trying to read temperature");
  gettemperature();

  Serial.println("Trying to do POST request");
  bool httpCode = httpPost(host, port, route);

  Serial.println("Was POSTrequest OK? " + httpCode);


  /* Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
       Serial.println("connection failed");
       Serial.println("waiting 10 sec...");
       delay(10000);
       return;
    }

    // This will send the request to the server
    client.print("Send this data to server");

    //read back one line from server
    String line = client.readStringUntil('\r');
    client.println(line);

    Serial.println("closing connection");
    client.stop();*/

  Serial.println("wait 120 seconds...");
  delay(120000);
}




