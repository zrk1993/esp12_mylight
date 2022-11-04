#ifndef CONFIG_h
#define CONFIG_h

#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>

/**
* 结构体
*/
struct config_type {
    char stassid[32];
    char stapsw[64];
    char cuid[40];
    char ctopic[32];
};

config_type config = { "stassid", "stapsw", "cuid", "ctopic" };

/**
* 加载存储的信息
*/
uint8_t *p = (uint8_t*)(&config);
void loadConfig() {
    EEPROM.begin(512);
    for (int i = 0; i < sizeof(config); i++) {
        *(p + i) = EEPROM.read(i);
    }
    EEPROM.commit();
}

String getConfigTxt() {
    String str = "";
    str += "stassid=";
    str += config.stassid;
    str += "&";
    str += "stapsw=";
    str += config.stapsw;
    str += "&";
    str += "cuid=";
    str += config.cuid;
    str += "&";
    str += "ctopic=";
    str += config.ctopic;
    str += "&";
    return str;
}

/**
* 存储信息
*/
void saveConfig() {
    EEPROM.begin(512);
    uint8_t *p = (uint8_t*)(&config);
    for (int i = 0; i < sizeof(config); i++) {
        EEPROM.write(i, *(p + i));
    }
    EEPROM.commit();
}

#endif
