
// CONFIG
void loadConfig() {
  EEPROM.begin(512);
  uint8_t load = 0;
  int pos = 0;
  EEPROM.get(0, load);
  pos = 1;
  if(load == 1) {
    EEPROM.get(pos, config);

    pos += sizeof(struct Config);

    EEPROM.get(pos, alarm);
    pos += sizeof(struct Alarm);

    for (uint8_t i= 0; i < senSize; i++) {
      sensors[i].readFromEeprom(pos);
      pos += sensors[i].positions();
    }
  } else {
     DEB_DO_PRINTLN(NO_CONF);
  }
  DEB_DO_PRINTLN(OK_CONF);
  EEPROM.end();
}

void saveConfig() {
  int pos= 0;
  EEPROM.begin(512);

  EEPROM.put(pos++, 1);
  EEPROM.put(pos, config);

  pos += sizeof(struct Config);

  EEPROM.put(pos, alarm);
  pos += sizeof(struct Alarm);

  for (uint8_t i= 0; i < senSize; i++) {
    sensors[i].saveInEeprom(pos);
    pos+= sensors[i].positions();
  }

  EEPROM.commit();
  DEB_DO_PRINTLN(OK_SAVE_CONF);
}
