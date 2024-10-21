#include <stdio.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>


const char* ssid = "NOBERTO";
const char* password = "thaybevbeto";
 
void setup() {
  pinMode(D4, OUTPUT);
  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
  digitalWrite(D4, LOW);
  Serial.begin(115200);
  Serial.println("Iniciando...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Conexao falhou! Reiniciando...");
    delay(5000);
    ESP.restart();
  }
 
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
  Serial.println("Pronto");
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());
}
 
void loop() {
  ArduinoOTA.handle(); // requisita o OTA (Over The Air)
  digitalWrite(D1,HIGH);
  digitalWrite(D4,HIGH);
  delay(2000);
  digitalWrite(D1,LOW);
  digitalWrite(D4,LOW);
  delay(2000);
  digitalWrite(D1,HIGH);
  digitalWrite(D4,HIGH);
  delay(2000);
  digitalWrite(D1,LOW);
  digitalWrite(D4,LOW);
  delay(2000);  

}
