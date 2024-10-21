#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <stdio.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <ESP8266HTTPClient.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const char* ssid = "NOBERTO";
const char* password = "thaybevbeto";

String url = "https://m1desenvolvimento.com.br/comedouro/requestApi.php";
int  readingInterval = 60;
int  apertureTime    = 30;
bool startNow        = false;
bool stopAll         = true;
bool forceReadingApi = true;
int  triggerTime[24];

bool wait            = false;
int  startedHour     = 0;
int  n               = 0;
long unsigned int t  = 0;

void setup() {
  Serial.println("Iniciando setup...");
  pinMode(D4, OUTPUT);
  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
  digitalWrite(D4, LOW);
  Serial.begin(115200);

  // WIFI
  Serial.println("Iniciando wifi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Conexao falhou! Reiniciando...");
    delay(5000);
    ESP.restart();
  }
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
 
  // OTA
  Serial.println("Iniciando OTA...");
  ArduinoOTA.onStart([]() {
    Serial.println("Inicio...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("nFim!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Autenticacao Falhou");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Falha no Inicio");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Falha na Conexao");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha na Recepcao");
    else if (error == OTA_END_ERROR) Serial.println("Falha no Fim");
  });
  ArduinoOTA.begin();
  
  // NTPClient
  Serial.println("Iniciando NTPClient...");
  timeClient.begin();
  timeClient.setTimeOffset(-10800);
  delay(10000);

  Serial.println("Fim de setup...");
}

void stopMeal(){
  digitalWrite(D1, LOW);
  digitalWrite(D4, LOW);
  Serial.println("Meal stopped...");
}

void startMeal(){
  digitalWrite(D1, HIGH);
  digitalWrite(D4, HIGH);
  delay(apertureTime*1000);
  stopMeal();
  Serial.println("Meal started...");
}
 
void loop() {
  ArduinoOTA.handle(); // requisita o OTA (Over The Air)
  timeClient.update(); // atualiza data e hora
  
  time_t epochTime     = timeClient.getEpochTime();
  int hour             = timeClient.getHours();
  String formattedTime = timeClient.getFormattedTime();

  if (forceReadingApi == true){
      wait = false;
  }
  else{
    if((epochTime-n)>=readingInterval*1000){
        wait = false;
    }
  }
  
  if(wait == false){
      if (WiFi.status() == WL_CONNECTED) {
        t = epochTime;
        wait = true;
        WiFiClientSecure client; 
        client.setInsecure();
        HTTPClient https;
        String data = "";
        String fullUrl = url + data;
        Serial.println("Requesting " + fullUrl);
        if (https.begin(client, fullUrl)) {
          int httpCode = https.GET();
          Serial.println("============== Response code: " + String(httpCode));
          if (httpCode > 0) {
            JsonDocument doc;
            String msgJson = https.getString();
            const char* json = msgJson.c_str();
            DeserializationError error = deserializeJson(doc, json);
            if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              return;
            }
            Serial.println(https.getString());
            readingInterval = doc["readingInterval"];
            apertureTime    = doc["apertureTime"];
            startNow        = doc["startNow"];
            stopAll         = doc["stopAll"];
            // startNow        = bool.Parse(doc["startNow"]);
            // stopAll         = bool.Parse(doc["stopAll"]);
            wait            = doc["wait"];
            forceReadingApi = doc["forceReadingApi"];
            // forceReadingApi = bool.Parse(doc["forceReadingApi"]);
            n               = doc["triggerTime"].size();
            for(int i=0;i<n;i++){
                triggerTime[i] = doc["triggerTime"][i];
            }
            Serial.println("readingInterval: " + String(readingInterval));
            Serial.println("apertureTime: " + String(apertureTime));
          }
          https.end();
        } else {
          Serial.printf("[HTTPS] Unable to connect\n");
        }
    }
  }

  if(stopAll == true){
    delay(5000);
    Serial.println("Stopping meal by command...");
    stopMeal();
  }
  else{
      if(startNow == true){
        delay(5000);
        Serial.println("Starting meal by command...");
        startMeal();
      }
      else{
        if (startedHour != hour){
          for(int i=0;i<n;i++){
              if (hour == triggerTime[i]){
                  Serial.println("Starting meal by hour...");
                  startedHour = hour;
                  startMeal();
              }
          }
        }
      }
  }

}