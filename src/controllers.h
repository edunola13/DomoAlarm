#include <ESP8266WebServer.h>

void get_status();
void get_config();
void put_config();
void get_alarm();
void put_alarm();
void put_alarm_on();
void put_alarm_off();
void post_activate_alarm();
void post_deactivate_alarm();

void config_rest_server_routing() {
    http_rest_server.on("/", HTTP_GET, []() {
        http_rest_server.send(200, "text/html", "Running");
    });

    // HANDLE
    http_rest_server.on("/status", HTTP_GET, get_status);  // Estado de Conexion
    http_rest_server.on("/config", HTTP_GET, get_config);  // Configuracion de Conexion
    http_rest_server.on("/config", HTTP_PUT, put_config);
    http_rest_server.on("/alarm", HTTP_GET, get_alarm);  // Alarma - Configuracion y estado
    http_rest_server.on("/alarm", HTTP_PUT, put_alarm);
    http_rest_server.on("/alarm/on", HTTP_PUT, put_alarm_on);
    http_rest_server.on("/alarm/off", HTTP_PUT, put_alarm_off);
    http_rest_server.on("/alarm/activate", HTTP_POST, post_activate_alarm);
    http_rest_server.on("/alarm/deactivate", HTTP_POST, post_deactivate_alarm);
    // http_rest_server.on('route', function);  // No filtra por METHOD -> despues pedir con http_rest_server.method()
}

void _response_config() {
    StaticJsonDocument<255> jsonBuffer;
    char JSONmessageBuffer[255];

    jsonBuffer["ssid"] = config.ssid;
    // jsonBuffer["passwd"] = config.passwd;
    jsonBuffer["ap_ssid"] = config.ap_ssid;
    // jsonBuffer["ap_passwd"] = config.ap_passwd;
    jsonBuffer["staticIp"] = config.staticIp;
    jsonBuffer["ip"] = config.ip.toString();
    jsonBuffer["gateway"] = config.gateway.toString();
    jsonBuffer["subnet"] = config.subnet.toString();

    serializeJsonPretty(jsonBuffer, JSONmessageBuffer);
    http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_status() {
    StaticJsonDocument<100> jsonBuffer;
    char JSONmessageBuffer[100];

    // Status
    jsonBuffer["status"] = String(status.status);
    jsonBuffer["actualIp"] = status.ip.toString();

    serializeJsonPretty(jsonBuffer, JSONmessageBuffer);
    http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_config() {
    _response_config();
}

void put_config() {
  StaticJsonDocument<500> jsonBuffer;
  String post_body = http_rest_server.arg("plain");
  Serial.println(post_body);

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(jsonBuffer, post_body);
  if (error) {
      Serial.println("error in parsin json body");
      http_rest_server.send(400, "application/json", "{\"error\": \"Invalid Json\"}");
  }
  else {
      // String value = "";
      if (jsonBuffer.containsKey("ssid")) {
        String value = jsonBuffer["ssid"];
        value.toCharArray(config.ssid, 30);
      }
      if (jsonBuffer.containsKey("passwd")) {
        String value = jsonBuffer["passwd"];
        value.toCharArray(config.passwd, 30);
      }
      if (jsonBuffer.containsKey("ap_ssid")) {
        String value = jsonBuffer["ap_ssid"];
        value.toCharArray(config.ap_ssid, 20);
      }
      if (jsonBuffer.containsKey("ap_passwd")) {
        String value = jsonBuffer["ap_passwd"];
        value.toCharArray(config.ap_passwd, 20);
      }
      if (jsonBuffer.containsKey("staticIp")) {
          config.staticIp = jsonBuffer["staticIp"];//.as<bool*>();
      }
      // jsonBuffer["ip"] = config.ip.toString();
      // jsonBuffer["gateway"] = config.gateway.toString();
      // jsonBuffer["subnet"] = config.subnet.toString();

      _response_config();
      // Wait 3 seconds -> If no wait dont receive response the client
      long initial = millis() + 3000;
      while (initial > millis()) {
        delay(5);
      }
      initWifi();
  }
}

void get_alarm() {

}

void put_alarm() {

}

void put_alarm_on() {

}

void put_alarm_off() {

}

void post_activate_alarm() {

}

void post_deactivate_alarm() {

}
