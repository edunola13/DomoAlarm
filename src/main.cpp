#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#define DOMO_DEBUG
#define DOMO_SPEED 9600
//#define USE_WDT
//#define WDT_TIME WDTO_8S

#define DEVICE_TYPE "ES_AL_1"

#include <common_initial.h>
#include <DigitalSensor.h>
#include "messages.h"

String device_type = "ES_AL_1";

bool isTime(long startTime, long maxTime) {
  long now = millis();
  if (now >= startTime) {
    return (now - startTime) >= maxTime;
  } else {
    startTime = startTime - 4294967296;
    return (now - startTime) >= maxTime;
  }
}

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

//BearSSL::ESP8266WebServerSecure http_rest_server_ssh(443);
ESP8266WebServer http_rest_server(HTTP_REST_PORT);

/*static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC6jCCAlOgAwIBAgIUV4hbn80tCIONpKF4Et6b3Ynq/X8wDQYJKoZIhvcNAQEL
BQAwejELMAkGA1UEBhMCUk8xCjAIBgNVBAgMAUIxEjAQBgNVBAcMCUJ1Y2hhcmVz
dDEbMBkGA1UECgwST25lVHJhbnNpc3RvciBbUk9dMRYwFAYDVQQLDA1PbmVUcmFu
c2lzdG9yMRYwFAYDVQQDDA1lc3A4MjY2LmxvY2FsMB4XDTE5MDkxNjAyMDAwM1oX
DTIwMDkxNTAyMDAwM1owejELMAkGA1UEBhMCUk8xCjAIBgNVBAgMAUIxEjAQBgNV
BAcMCUJ1Y2hhcmVzdDEbMBkGA1UECgwST25lVHJhbnNpc3RvciBbUk9dMRYwFAYD
VQQLDA1PbmVUcmFuc2lzdG9yMRYwFAYDVQQDDA1lc3A4MjY2LmxvY2FsMIGfMA0G
CSqGSIb3DQEBAQUAA4GNADCBiQKBgQDZvgnP4fGGNZ93VUcTlFUEFjumHMPeoQJk
UXlhbYR3pwRtSgpQjKElHe9X9GVOm9PwrHXc604woexcpEQDED297V1/FT5pU0i+
VWXGm1cSs33lR1//3XNfF5oi36HvJGP/Iqz8eH5oRDpfGvAIp9QEcL9S1CWJzidc
No+TiKGbnwIDAQABo20wazAdBgNVHQ4EFgQUDHDXgGd7kfoER5IJf4/g3izoEx8w
HwYDVR0jBBgwFoAUDHDXgGd7kfoER5IJf4/g3izoEx8wDwYDVR0TAQH/BAUwAwEB
/zAYBgNVHREEETAPgg1lc3A4MjY2LmxvY2FsMA0GCSqGSIb3DQEBCwUAA4GBALjo
8Tp9PcCmFrP6JcvnYWwxhIMutaEfFf9c1AX/FWIgmx1CUr+ewncR5heZKbzso8mF
1HmX9N2mukb/0gzOsPge5eSrdZjw56PDQkg4+MQN08Ha8E40SUK+OGpnPMwAVnvd
YPxoQ6GpFURq5rp5ru2E9+n+/9gFLkOVVEeLO8jN
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----
MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBANm+Cc/h8YY1n3dV
RxOUVQQWO6Ycw96hAmRReWFthHenBG1KClCMoSUd71f0ZU6b0/CsddzrTjCh7Fyk
RAMQPb3tXX8VPmlTSL5VZcabVxKzfeVHX//dc18XmiLfoe8kY/8irPx4fmhEOl8a
8Ain1ARwv1LUJYnOJ1w2j5OIoZufAgMBAAECgYEAiHgOCEeRK8+h8ZX2JTRbkGMq
4XK35Gm/aQaTb9fHJYL9SE4WZuOs/+liIBsh/4G09OvyNxMXf22NCYc+xTjBqIcQ
NFLeHlQtTskzW7oHTSfwV1KuOXrATuPENr0CtEo/pld5mLhEvX0lsvbd82xTt/9R
GvpxzxlRCHG+yAng84ECQQDsM5XZe+OVMC4CrmVUTxPykt8eqSnV9qjCX4dePq85
Es+NtZWWmZgujnnC9tGmbta0EHNidJg38JmKj2enw3TBAkEA6/5bHpbIEScMXySe
FE+a/Qo3UiT7Z9yVM1LPs4NQZFpJsf9vMoiEaQrfTQLUbjOMFzWkgZKrRvK4weGu
8ANIXwJANjopnwKoXynuhkMPlGmLRNefTeS8bBjy6Z0Q8PwnMk01RW146Fhe7eFb
5vzPaQxtUm2sb+Agykb8mSkPPR7MgQJBANJm3lpxWEJirBcPkJjPOIGt4BCuxC6f
ba5qgJ+tzbkK/nViJYPKTzNP7DK++SKfdqEixF55o5cHxE2nps56PsUCQGNeROlX
jR0oV96dz/cL0aZYc0mU88N1mWJNu8Dvz4OKREDPzNoWsirJikQNxb3daEBh6EFh
dyHrT0u/Tnxe9kQ=
-----END PRIVATE KEY-----
)EOF";*/

#include "config.h"
#include "alarm.h"
#include "memory.h"
#include "controllers.h"

void setup(void) {
    Serial.begin(9600);
    initialGeneric();
    DEB_DO_PRINTLN(MSG_START);

    // Leer config de Epprom
    loadConfig();
    // Inicializo WiFi
    initWifi();
    // Inicializar sensores
    initAlarm();

    // Config rest server routing
    config_rest_server_routing();
    Serial.println("HTTP REST Server Started");
}

void loop(void) {
    // actualizar sensores
    updateSensors();
    // analizar lanzar alarma
    checkAlarm();
    // avisar a servidor que se activo la alarma
    // ---

    // Handle Requests
    http_rest_server.handleClient();
    // http_rest_server_ssh.handleClient();

    if (!clientMqtt.connected()) {
      reconnect();
    }
    clientMqtt.loop();
}
