#include <ESP8266WebServer.h>

void get_status();
void get_config();
void put_config();
void post_config();
void get_alarm();
void put_alarm();
void put_alarm_on();
void put_alarm_off();
void put_activate_alarm();
void put_sound_alarm();
void put_reset_alarm();
void get_sensors();
void get_sensor();
void put_sensor();

void config_rest_server_routing() {
  /* SIN HTTPS */
  http_rest_server.on("/", HTTP_GET, []() {
      http_rest_server.send(200, "text/html", "Running");
  });

  // HANDLE
  http_rest_server.on("/status", HTTP_GET, get_status);  // Estado de Conexion
  http_rest_server.on("/config", HTTP_GET, get_config);  // Configuracion de Conexion
  http_rest_server.on("/config", HTTP_PUT, put_config);
  http_rest_server.on("/config", HTTP_POST, post_config);
  http_rest_server.on("/alarm", HTTP_GET, get_alarm);  // Alarma - Configuracion y estado
  http_rest_server.on("/alarm", HTTP_PUT, put_alarm);
  http_rest_server.on("/alarm/on", HTTP_PUT, put_alarm_on);
  http_rest_server.on("/alarm/off", HTTP_PUT, put_alarm_off);
  http_rest_server.on("/alarm/activate", HTTP_PUT, put_activate_alarm);
  http_rest_server.on("/alarm/sound", HTTP_PUT, put_sound_alarm);
  http_rest_server.on("/alarm/reset", HTTP_PUT, put_reset_alarm);
  http_rest_server.on("/sensors", HTTP_GET, get_sensors);  // Sensores
  http_rest_server.on("/sensors/detail", HTTP_GET, get_sensor);
  http_rest_server.on("/sensors/detail", HTTP_PUT, put_sensor);
  // http_rest_server.on('route', function);  // No filtra por METHOD -> despues pedir con http_rest_server.method()
  http_rest_server.begin();

  /* CON HTTPS */
  /*http_rest_server.on("/", []() {
    http_rest_server.sendHeader("Location", String("https://") + WiFi.localIP().toString(), true);
    http_rest_server.send(301, "text/plain", "");
  });
  http_rest_server.begin();

  http_rest_server_ssh.setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  http_rest_server_ssh.on("/", HTTP_GET, []() {
      http_rest_server_ssh.send(200, "text/html", "Running");
  });

  // HANDLE
  http_rest_server_ssh.on("/status", HTTP_GET, get_status);  // Estado de Conexion
  http_rest_server_ssh.on("/config", HTTP_GET, get_config);  // Configuracion de Conexion
  http_rest_server_ssh.on("/config", HTTP_PUT, put_config);
  http_rest_server_ssh.on("/config", HTTP_POST, post_config);
  http_rest_server_ssh.on("/alarm", HTTP_GET, get_alarm);  // Alarma - Configuracion y estado
  http_rest_server_ssh.on("/alarm", HTTP_PUT, put_alarm);
  http_rest_server_ssh.on("/alarm/on", HTTP_PUT, put_alarm_on);
  http_rest_server_ssh.on("/alarm/off", HTTP_PUT, put_alarm_off);
  http_rest_server_ssh.on("/alarm/activate", HTTP_PUT, put_activate_alarm);
  http_rest_server_ssh.on("/alarm/sound", HTTP_PUT, put_sound_alarm);
  http_rest_server_ssh.on("/alarm/reset", HTTP_PUT, put_reset_alarm);
  http_rest_server_ssh.on("/sensors", HTTP_GET, get_sensors);  // Sensores
  http_rest_server_ssh.on("/sensors/detail", HTTP_GET, get_sensor);
  http_rest_server_ssh.on("/sensors/detail", HTTP_PUT, put_sensor);
  // http_rest_server_ssh.on('route', function);  // No filtra por METHOD -> despues pedir con http_rest_server_ssh.method()
  http_rest_server_ssh.begin();*/
}

void _response_config() {
  StaticJsonDocument<500> jsonBuffer;
  char JSONmessageBuffer[500];

  jsonBuffer["timeStart"] = millis();

  jsonBuffer["ssid"] = config.ssid;
  // jsonBuffer["passwd"] = config.passwd;
  jsonBuffer["ap_ssid"] = config.ap_ssid;
  // jsonBuffer["ap_passwd"] = config.ap_passwd;
  jsonBuffer["staticIp"] = config.staticIp;

  JsonArray ipArray = jsonBuffer["ip"].to<JsonArray>();
  ipArray.add(config.ip[0]);
  ipArray.add(config.ip[1]);
  ipArray.add(config.ip[2]);
  ipArray.add(config.ip[3]);

  JsonArray gatewayArray = jsonBuffer["gateway"].to<JsonArray>();
  gatewayArray.add(config.gateway[0]);
  gatewayArray.add(config.gateway[1]);
  gatewayArray.add(config.gateway[2]);
  gatewayArray.add(config.gateway[3]);

  JsonArray subnetArray = jsonBuffer["subnet"].to<JsonArray>();
  subnetArray.add(config.subnet[0]);
  subnetArray.add(config.subnet[1]);
  subnetArray.add(config.subnet[2]);
  subnetArray.add(config.subnet[3]);

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void _response_alarm() {
  StaticJsonDocument<255> jsonBuffer;
  char JSONmessageBuffer[255];

  jsonBuffer["status"] = String(alarm.status);
  jsonBuffer["timeToSound"] = alarm.timeToSound;
  jsonBuffer["soundTime"] = alarm.soundTime;
  jsonBuffer["refreshTime"] = alarm.refreshTime;

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void _response_sensor(uint8_t id) {
  StaticJsonDocument<255> jsonBuffer;
  char JSONmessageBuffer[255];

  jsonBuffer["id"] = id;
  jsonBuffer["start"] = sensors[id].getStarted();
  jsonBuffer["type"] = sensors[id].type;
  jsonBuffer["pin"] = sensors[id].getPin();
  jsonBuffer["in"] = sensors[id].getIn();

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_status() {
  StaticJsonDocument<100> jsonBuffer;
  char JSONmessageBuffer[100];

  // Status
  jsonBuffer["status"] = String(status.status);
  jsonBuffer["actualIp"] = status.ip.toString();

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_config() {
  _response_config();
}

void put_config() {
  StaticJsonDocument<500> jsonBuffer;
  String post_body = http_rest_server.arg("plain");

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

      if (jsonBuffer.containsKey("ip")) {
        JsonArray ipArray = jsonBuffer["ip"].as<JsonArray>();
        if (ipArray.size() == 4) {
          config.ip[0] = ipArray[0];
          config.ip[1] = ipArray[1];
          config.ip[2] = ipArray[2];
          config.ip[3] = ipArray[3];
        }
      }
      if (jsonBuffer.containsKey("gateway")) {
        JsonArray gatewayArray = jsonBuffer["gateway"].as<JsonArray>();
        if (gatewayArray.size() == 4) {
          config.gateway[0] = gatewayArray[0];
          config.gateway[1] = gatewayArray[1];
          config.gateway[2] = gatewayArray[2];
          config.gateway[3] = gatewayArray[3];
        }
      }
      if (jsonBuffer.containsKey("subnet")) {
        JsonArray subnetArray = jsonBuffer["subnet"].as<JsonArray>();
        if (subnetArray.size() == 4) {
          config.subnet[0] = subnetArray[0];
          config.subnet[1] = subnetArray[1];
          config.subnet[2] = subnetArray[2];
          config.subnet[3] = subnetArray[3];
        }
      }

      _response_config();
      // Wait 3 seconds -> If no wait dont receive response the client
      // long initial = millis() + 3000;
      // while (initial > millis()) {
      //   delay(5);
      // }
      // initWifi();
  }
}

void post_config() {
  saveConfig();
  http_rest_server.send(200);
}

void get_alarm() {
  _response_alarm();
}

void put_alarm() {
  StaticJsonDocument<500> jsonBuffer;
  String post_body = http_rest_server.arg("plain");

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(jsonBuffer, post_body);
  if (error) {
      Serial.println("error in parsin json body");
      http_rest_server.send(400, "application/json", "{\"error\": \"Invalid Json\"}");
  }
  else {
      // String value = "";
      // if (jsonBuffer.containsKey("status")) {
      //   alarm.status = jsonBuffer["status"];
      // }
      if (jsonBuffer.containsKey("timeToSound")) {
        alarm.timeToSound = jsonBuffer["timeToSound"];
      }
      if (jsonBuffer.containsKey("refreshTime")) {
        alarm.refreshTime = jsonBuffer["refreshTime"];
      }
      if (jsonBuffer.containsKey("soundTime")) {
        alarm.soundTime = jsonBuffer["soundTime"];
      }

      _response_alarm();
  }
}

void put_alarm_on() {
  for (uint8_t i= 0; i < senSize; i++) {
    if(sensors[i].getStarted()){
      if (sensors[i].getIn()) {
        http_rest_server.send(400, "application/json", "{\"error\": \"Hay un sensor activo\"}");
        return;
      }
    }
  }
  onAlarm();
  http_rest_server.send(200);
}

void put_alarm_off() {
  offAlarm();
  http_rest_server.send(200);
}

void put_activate_alarm() {
  activeAlarm();
  http_rest_server.send(200);
}

void put_sound_alarm() {
  soundAlarm();
  http_rest_server.send(200);
}

void put_reset_alarm() {
  resetAlarm();
  http_rest_server.send(200);
}

void get_sensors() {
  StaticJsonDocument<1000> jsonBuffer;
  char JSONmessageBuffer[1000];
  JsonArray array = jsonBuffer.to<JsonArray>();

  for (uint8_t i= 0; i < senSize; i++) {
    JsonObject nested = array.createNestedObject();
    nested["id"] = i;
    nested["start"] = sensors[i].getStarted();
    nested["type"] = sensors[i].type;
    nested["pin"] = sensors[i].getPin();
    nested["in"] = sensors[i].getIn();
  }

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_sensor() {
  uint8_t id = http_rest_server.arg("id").toInt();
  if (id < 0 || id >= senSize) {
      http_rest_server.send(404);
  }
  _response_sensor(id);
}

void put_sensor() {
  uint8_t id = http_rest_server.arg("id").toInt();
  if (id < 0 || id >= senSize) {
      http_rest_server.send(404);
  }

  StaticJsonDocument<500> jsonBuffer;
  String post_body = http_rest_server.arg("plain");

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(jsonBuffer, post_body);
  if (error) {
      Serial.println("error in parsin json body");
      http_rest_server.send(400, "application/json", "{\"error\": \"Invalid Json\"}");
  }
  else {
    bool start = jsonBuffer["start"];
    sensors[id].type = jsonBuffer["type"];
    sensors[id].setPin(jsonBuffer["pin"]);

    if (start) {
      sensors[id].begin();
    } else {
      sensors[id].stop();
    }

    _response_sensor(id);
  }
}
