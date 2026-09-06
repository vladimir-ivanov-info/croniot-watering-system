#ifndef SECRETS_MOCK_H
#define SECRETS_MOCK_H

//RENAME THIS FILE TO "secrets.h" AND RENAME THE FIRST 2 LINES TO:
//  #ifndef SECRETS_H
//  #define SECRETS_H
//AFTER THAT, ADJUST THE CONSTANTS BELOW, AND RENAME THE CLASS TO "Secrets".

#define ACCOUNT_EMAIL "email1@gmail.com"
#define ACCOUNT_UUID "account1Uuid"
#define ACCOUNT_PASSWORD "password1"
#define DEVICE_UUID "watering_system_1"
#define DEVICE_NAME "Watering System"
#define DEVICE_DESCRIPTION "This device is for watering plants. You can choose different tasks: do you want to water the plants now or set an alarm for automatic watering later?"
#define BLE_PASSWORD "123456" // BLE passkey: exactly 6 numeric digits

#include <string>

class SecretsMock {

    public:
        inline static std::string WIFI_SSID = "WIFI_SSID";
        inline static std::string WIFI_PASSWORD = "WIFI_PASSWORD";

        inline static std::string SERVER_ADDRESS = "";
        inline static const std::string SERVER_ADDRESS_LOCAL = "192.168.x.x";
        inline static const std::string SERVER_ADDRESS_REMOTE = "a.b.c.d";

        inline static int SERVER_PORT = 8090;
        inline static int SERVER_PORT_MQTT = 1883;
};

#endif
