#include <ESP8266WiFi.h>
// ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>

struct Config {
  char ssid[30] = "FibertelEdu18";
  char passwd[30] = "0143507310";
  char ap_ssid[20] = "DomoAlarm";
  char ap_passwd[20] = "DomoAlarm";
  bool staticIp = false;
  IPAddress ip = IPAddress(192, 168, 0, 53);
  IPAddress gateway = IPAddress(192, 168, 0, 1);
  IPAddress subnet = IPAddress(255, 255, 255, 0);
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
      WiFi.config(config.ip, config.gateway, config.subnet);
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

//
//CONFIGURACION
// void loadConfig() {
//   DEB_DO_PRINTLN(LOAD_CONF);
//   int pos= 0;
//   //Compruebo que se hayan grabado datos, si no utilizo valores por defecto
//   uint8_t load= EEPROM.read(pos++);
//   if(load == 1) {
//     //Lee configuracion global
//     // readConfWifi(pos);
//
//     //Lee configuracion de componentes
//     pos= 200;
//     //Sensor Digital
//     // for (uint8_t i= 0; i < digSenSize; i++) {
//     //   digitalSen[i].readFromEeprom(pos);
//     //   pos+= digitalSen[i].positions();
//     // }
//   } else{
//     DEB_DO_PRINTLN(NO_CONF);
//   }
//   DEB_DO_PRINTLN(OK_CONF);
// }
//
// void saveConfig(){
//   DEB_DO_PRINTLN(SAVE_CONF);
//   int pos= 0;
//   EEPROM.update(pos++, 1);
//   //Escribo configuracion global
//   // saveConfWifi(pos);
//
//   pos= 200;
//   //Sensor Digital
//   // for (uint8_t i= 0; i < digSenSize; i++) {
//   //   digitalSen[i].saveInEeprom(pos);
//   //   pos+= digitalSen[i].positions();
//   // }
//
//   DEB_DO_PRINTLN(OK_SAVE_CONF);
// }
