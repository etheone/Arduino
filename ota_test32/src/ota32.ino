#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>

/*#include <ESP8266WiFi.h>*/
#include <ESP8266WiFiMulti.h>



ESP8266WiFiMulti WiFiMulti;

int i = 0;

void setup()
{
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  WiFiMulti.addAP("Joels-iPhone", "qweqwe123");

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}

boolean httpPost(char *host, uint16_t port, char *url)
{
  HTTPClient http;
  bool ret = false;
  bool httpCode;

  unsigned long start = millis();

  String toSend = "{\"deviceId\": \"9f8bc3bc-b8ce-4e8a-b91b-973fed280ff4\", \"data\": { \"temperature\": \"20\", \"ota\":\"ota2\", \"humidity\": \"20\"}}";
  http.begin(host, port, url);

  http.addHeader("Content-Type", "application/json");
  httpCode = http.POST(toSend);
  http.writeToStream(&Serial);
  http.end();
  if (httpCode == 200)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void loop()
{

  i++;
  
  uint16_t port = 80;
  char *host = "devota.se";          // ip or dns
  char *route = "/api/data/addreal"; //route

  Serial.println("Trying to read temperature");

  Serial.println("Trying to do POST request");
  bool httpCode = httpPost(host, port, route);

  Serial.println("Was POSTrequest OK? " + httpCode);

  Serial.println("wait 10 seconds...");

  if (i % 5 == 0)
  {
    Serial.println("Updating firmware");
    t_httpUpdate_return ret = ESPhttpUpdate.update("http://www.devota.se/api/OTA/update.bin");

    switch (ret)
    {
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

  delay(10000);
}
