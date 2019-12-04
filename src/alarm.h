struct Alarm {
  char status = 'F';  // O=On, F=Off, A=Activada, S=Sonando
  long refreshTime = 250;  // 0.25 Segundo
  long lastRefreshTime = 0;
  long timeToSound = 2000;  // 20000;  // Tiempo para desactivar la alarma y (salir despues de activar - NO POR AHORA)
  long lastTimeToSound = 0;
  long soundTime = 20000;  // 60000 * 60;  // Tiempo que va a sonar la alarma
  long lastSoundTime = 0;
} alarm;

#define hornPin D4
#define buzzerPin D2
#define senSize 8
DigitalSensor sensors[senSize];

// #define eventSize 10
// struct Event {
//   long from;
//   long to;
//   char type;
//   uint8_t sensor;
// } events[eventSize] = {};

// TOPICS
#define TOPIC_ON "DOMO/ES_AL_1/alarm/on"
#define TOPIC_OFF "DOMO/ES_AL_1/alarm/off"
#define TOPIC_ACTIVATE "DOMO/ES_AL_1/alarm/activate"
#define TOPIC_SOUND "DOMO/ES_AL_1/alarm/sound"

void initAlarm() {
  alarm.status = 'F';
  pinMode(buzzerPin, OUTPUT);
}

void onAlarm() {
  alarm.status = 'O';
  if (clientMqtt.connected()) {
    StaticJsonDocument<50> jsonBuffer;
    char JSONmessageBuffer[50];
    jsonBuffer["uniqueId"] = String(ESP.getChipId(), HEX);
    jsonBuffer["status"] = String(alarm.status);
    serializeJson(jsonBuffer, JSONmessageBuffer);

    clientMqtt.publish(TOPIC_ON, JSONmessageBuffer);
  }
}

void offAlarm() {
  alarm.status = 'F';
  if (clientMqtt.connected()) {
    StaticJsonDocument<50> jsonBuffer;
    char JSONmessageBuffer[50];
    jsonBuffer["uniqueId"] = String(ESP.getChipId(), HEX);
    jsonBuffer["status"] = String(alarm.status);
    serializeJson(jsonBuffer, JSONmessageBuffer);

    clientMqtt.publish(TOPIC_OFF, JSONmessageBuffer);
  }
}

void activeAlarm() {
  alarm.status = 'A';
  alarm.lastTimeToSound = millis();
  if (clientMqtt.connected()) {
    StaticJsonDocument<50> jsonBuffer;
    char JSONmessageBuffer[50];
    jsonBuffer["uniqueId"] = String(ESP.getChipId(), HEX);
    jsonBuffer["status"] = String(alarm.status);
    serializeJson(jsonBuffer, JSONmessageBuffer);

    clientMqtt.publish(TOPIC_ACTIVATE, JSONmessageBuffer);
  }
}

void soundAlarm() {
  alarm.status = 'S';
  alarm.lastSoundTime = millis();
  if (clientMqtt.connected()) {
    StaticJsonDocument<50> jsonBuffer;
    char JSONmessageBuffer[50];
    jsonBuffer["uniqueId"] = String(ESP.getChipId(), HEX);
    jsonBuffer["status"] = String(alarm.status);
    serializeJson(jsonBuffer, JSONmessageBuffer);

    clientMqtt.publish(TOPIC_SOUND, JSONmessageBuffer);
  }
}

void resetAlarm() {
  if (alarm.status == 'A' || alarm.status == 'S') {
    alarm.status = 'O';
  }
}

void updateSensors() {
  if(millis() - alarm.lastRefreshTime >= alarm.refreshTime || millis() - alarm.lastRefreshTime < 0) {
    // Digital Sensor
    for (uint8_t i= 0; i < senSize; i++) {
      if(sensors[i].getStarted()){
        sensors[i].updateSensor();
      }
    }

    // Actualizo el tiempo
    alarm.lastRefreshTime = millis();
  }
}

void checkAlarm() {
  if (alarm.status == 'O') {
    // Chequeo si alguno de los sensores se activo
    for (uint8_t i= 0; i < senSize; i++) {
      if(sensors[i].getStarted()){
        if (sensors[i].getIn()) {
          activeAlarm();
          break;
        }
      }
    }
  } else if (alarm.status == 'A') {
    // CALCULAR EL TIEMPO EN QUE LA ALARMA SE LANZARA
    // if(millis() - alarm.lastTimeToSound >= alarm.timeToSound || millis() - alarm.lastTimeToSound < 0) {
    if (isTime(alarm.lastTimeToSound, alarm.timeToSound)) {
      soundAlarm();
    }
  } else if (alarm.status == 'S') {
    // CALCULAR TIEMPO QUE DEBE SONAR
    // if(millis() - alarm.lastSoundTime >= alarm.soundTime || millis() - alarm.lastSoundTime < 0) {
    if (isTime(alarm.lastSoundTime, alarm.soundTime)) {
      resetAlarm();
    }
  }

  if (alarm.status == 'A') {
    // Lanza buzzer
    digitalWrite(buzzerPin, HIGH);
  } else {
    // Para buzzer
    digitalWrite(buzzerPin, LOW);
  }

  if (alarm.status == 'S') {
    // Lanza alarma (sonido)
    digitalWrite(hornPin, HIGH);
  } else {
    // Para alarma (sonido)
    digitalWrite(hornPin, LOW);
  }
}
