#include <ESP8266WiFi.h>
// ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>

struct Config {
  char ssid[30] = "FibertelEdu18";
  char passwd[30] = "0143507310";
  char ap_ssid[20] = "DomoAlarm";
  char ap_passwd[20] = "DomoAlarm";
  bool staticIp = false;
  uint8_t ip[4] = {192, 168, 0, 53};
  uint8_t gateway[4] = {192, 168, 0, 1};
  uint8_t subnet[4] = {255, 255, 255, 0};
} config;

struct ConfigStatus {
  char status = 'I';  // I=Iniciando, C=Conectado, A=Access Point
  IPAddress ip;
} status;

void initWifi() {
    WiFi.setAutoConnect(false);
    WiFi.disconnect();
    int retries = 0;

    Serial.println("Connecting to WiFi AP..........");

    if (config.staticIp) {
      IPAddress ip = IPAddress(config.ip[0], config.ip[1], config.ip[2], config.ip[3]);
      IPAddress gateway = IPAddress(config.gateway[0], config.gateway[1], config.gateway[2], config.gateway[3]);
      IPAddress subnet = IPAddress(config.subnet[0], config.subnet[1], config.subnet[2], config.subnet[3]);
      WiFi.config(ip, gateway, subnet);
    }
    WiFi.begin(config.ssid, config.passwd);
    WiFi.mode(WIFI_STA);
    // check the status of WiFi connection to be WL_CONNECTED
    while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
        retries++;
        delay(WIFI_RETRY_DELAY);
        Serial.print("#");
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Connetted to");
        Serial.print(config.ssid);
        Serial.print("--- IP: ");
        status.ip = WiFi.localIP();
        Serial.println(status.ip);
        status.status = 'C';
    }
    else {
        Serial.print("Creating AP");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(config.ap_ssid, config.ap_passwd);
        status.ip = WiFi.localIP();
        status.status = 'A';
    }
}
