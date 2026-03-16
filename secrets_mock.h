#ifndef SECRETS_MOCK_H
#define SECRETS_MOCK_H

//RENAME THIS FILE TO "secrets.h" AND RENAME THE FIRST 2 LINES TO:
//  #ifndef SECRETS_H
//  #define SECRETS_H
//AFTER THAT, ADJUST THE CONSTANTS BELOW:

#define ACCOUNT_EMAIL "email1@gmail.com"
#define ACCOUNT_UUID "account1Uuid"
#define ACCOUNT_PASSWORD "password1"
#define DEVICE_UUID "watering_system_1"
#define DEVICE_NAME "Watering System"
#define DEVICE_DESCTIPION "This device is for watering plants. You can choose different tasks: do you want to water the plants now or set an alarm for automatic watering later?"

#include <Arduino.h>

class SecretsMock{

    public:
        inline static String WIFI_SSID = "WIFI_SSID"; 
        inline static String WIFI_PASSWORD = "WIFI_PASSWORD"; 

        inline static String SERVER_ADDRESS = "";
        inline static const String SERVER_ADDRESS_LOCAL = "192.168.x.x";
        inline static const String SERVER_ADDRESS_REMOTE = "a.b.c.d";

        inline static int SERVER_PORT = 8090;
        inline static int SERVER_PORT_MQTT = 1883;
        
    private:



};

#endif