#include <stdio.h>
#include <ESP8266WebServer.h>
// #include <EEPROM.h>
#include <ArduinoJson.h>

#define DOMO_DEBUG
#define DOMO_SPEED 9600
//#define USE_WDT
//#define WDT_TIME WDTO_8S

#include <common_initial.h>
#include "messages.h"

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50
ESP8266WebServer http_rest_server(HTTP_REST_PORT);

#include "config.h"
#include "controllers.h"

struct Alarm {
  char status;  // Desactivada, Activada, Sonando
  uint8_t mov;  // Ver de si usar cantidad o array
  uint8_t sensorapertura;  // Ver como llamarlo
  // Alguna otra config
};

struct Event {
  long from;
  long to;
  char type;
  uint8_t sensor;
};

struct Action {
  long from;
  long to;
  char type;
};

void setup(void) {
    Serial.begin(9600);
    initialGeneric();
    DEB_DO_PRINTLN(MSG_START);

    // Leer config de Epprom
    // loadConfig();
    // Inicializo WiFi
    initWifi();
    // Inicializar sensores

    // Config rest server routing
    config_rest_server_routing();

    http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void loop(void) {
    // actualizar sensores

    // ALARMA ACTIVA

    // analizar eventos: registramos y completamos
    // registrar si hubo algun evento
    // - la fecha "to" la ponemos cuando deja de suceder
    // Hay hasta 10 eventos. Despues de eso empezamos a pisar

    // analizar lanzar alarma
    // lanzar evento si el calculo de eventos da y paso el tiempo de desactivado

    // avisar a servidor que se activo la alarma
    // avisar a servidor los eventos nuevos

    // FIN ALARMA ACTIVA

    // Handle Requests
    http_rest_server.handleClient();
}
