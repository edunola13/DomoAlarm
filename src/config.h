#include <ESP8266WiFi.h>
// ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

struct Config {
  char name[20] = "ESP Alarm";
  char access_key[30] = "ESP Alarm";
  char mq_server[15] = "";
  char mq_user[10] = "";
  char mq_pass[10] = "";

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

WiFiClient espClient;
PubSubClient clientMqtt(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    // digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    // digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

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

    if (String(config.mq_server) != "") {
      clientMqtt.setServer(config.mq_server, 1883);
      clientMqtt.setCallback(callback);
    }
}

void reconnect() {
  if (String(config.mq_server) == "") {
    return;
  }

  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (clientMqtt.connect("ESP8266Client", config.mq_user, config.mq_pass)) {
    Serial.println("connected");
    // ... and resubscribe
    // clientMqtt.subscribe("casa/despacho/luz");
  }
}
