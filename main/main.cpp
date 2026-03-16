// #include "../secrets.h"
// #include "CommonSetup.h"
// #include "network/NetworkManager.h"
// #include "secrets.h" //TODO create this file as explained in secrets_mock.h
// #include "CommonConstants.h"
// #include "TasksInitializer.h"
// #include "SensorsInitializer.h"

// #include "network/http/WifiHttpController.h"
// #include "network/mqtt/WifiMqttController.h"
// #include "network/connection_provider/WifiNetworkConnectionController.h"


// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// void setServer();
// void setupCredentialsAndConnectToServer();

// String DEVICE_UUID_EXTERN = DEVICE_UUID;

// void setup() {
//   Serial.begin(115200);
//   while (!Serial) {
//     vTaskDelay(100 / portTICK_PERIOD_MS);
//   }

//   esp_task_wdt_init(100, true); // Set timeout to 10 seconds
//   esp_task_wdt_add(NULL);      // Add loopTask to WDT monitoring

//   setServer();

//   NetworkManager::instance().setWifiCredentials(Secrets::WIFI_SSID, Secrets::WIFI_PASSWORD);
//   NetworkManager::instance().setServerAddress(Secrets::SERVER_ADDRESS, Secrets::SERVER_PORT, Secrets::SERVER_PORT_MQTT);

//   //Registration internal to IoT device. After the connection is established with the server, the device will send this information to said server if necessary.
//   SensorsInitializer::initSensors();
//   TasksInitializer::initTasks();

//   setupCredentialsAndConnectToServer();
// }

// void setServer(){

//   const int PIN_SERVER_SELECTION = 33;

//   pinMode(PIN_SERVER_SELECTION, INPUT);
//   int pinServeState = digitalRead(PIN_SERVER_SELECTION);

//   if(pinServeState == HIGH){
//     Secrets::SERVER_ADDRESS = Secrets::SERVER_ADDRESS_REMOTE;
//   } else {
//     Secrets::SERVER_ADDRESS = Secrets::SERVER_ADDRESS_LOCAL;
//   }

//   Serial.print("### Current server: "); Serial.println(Secrets::SERVER_ADDRESS);
// }

// void setupCredentialsAndConnectToServer(){
//   UserCredentials actualCredentials = UserCredentials(ACCOUNT_EMAIL, ACCOUNT_UUID, ACCOUNT_PASSWORD, DEVICE_UUID, "", DEVICE_NAME, DEVICE_DESCRIPTION);

//   HttpProvider::set(new WifiHttpController());
//   MqttProvider::set(new WifiMqttController());

//   auto *networkConnectionController = new WifiNetworkConnectionController();
//   bool setupOk = CommonSetup::instance().setup(actualCredentials, networkConnectionController);

//   if(setupOk){
      
//   } else {
//     //TODO
//   }

// }

// /*void loop() {
//   //CommonSetup::instance().loop();
//   vTaskDelay(10000 / portTICK_PERIOD_MS); //TODO see if we remove this, will the loop consume CPU cycles, even if it doesn't do anything?
//   esp_task_wdt_reset();
// }*/



// extern "C" void app_main(void) {
//     // Tu código aquí
//        while (true) {
//         vTaskDelay(pdMS_TO_TICKS(10));
//     }
// }




#include "../secrets.h"
#include "CommonSetup.h"
//#inc123learlude "cro123niot/Commo123nSetup.h" // si estás en otro componente, depende de la estructura

#include "network/NetworkManager.h"
#include "CommonConstants.h"
#include "TasksInitializer.h"
#include "SensorsInitializer.h"

#include "network/http/WifiHttpController.h"
#include "network/mqtt/WifiMqttController.h"
#include "network/connection_provider/WifiNetworkConnectionController.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"

static const char *TAG = "Main";

// CPU load measurement: low-priority task counts iterations when CPU is free
static volatile uint32_t idleCounter = 0;
static uint32_t maxIdleCount = 0; // calibrated at startup

static void cpuIdleTask(void*) {
    while (true) {
        idleCounter++;
        // Small yield to not starve watchdog
        if ((idleCounter % 1000) == 0) {
            taskYIELD();
        }
    }
}

void setServer();
void setupCredentialsAndConnectToServer();

std::string DEVICE_UUID_EXTERN = DEVICE_UUID;

void SetupTask(void*);


extern "C" void app_main(void) {

    //vTaskDelay(pdMS_TO_TICKS(5000));  // 5 segundos para que OpenOCD conecte

    esp_log_level_set(TAG, ESP_LOG_INFO); // Para controlar el nivel de log

    ESP_LOGI(TAG, "Starting setup");

    // Create CPU load measurement task at lowest priority
    xTaskCreate(cpuIdleTask, "cpuIdle", 1024, nullptr, 0, nullptr);

    // Calibrate: temporarily drop main priority so cpuIdleTask gets full CPU
    vTaskPrioritySet(nullptr, 0);
    idleCounter = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));
    maxIdleCount = idleCounter;
    vTaskPrioritySet(nullptr, 1); // restore
    ESP_LOGW(TAG, "CPU calibration: maxIdleCount=%lu/sec", maxIdleCount);

    // Inicialización watchdog
    //Deprecated: esp_task_wdt_init(10, true);

    esp_task_wdt_config_t wdt_config = {
      .timeout_ms = 100000,   // 100 segundos
      //.idle_core_mask = BIT(CONFIG_ESP_TASK_WDT_IDLE_CORE),
      .idle_core_mask = BIT(0), //watches the core 0
      .trigger_panic = true,
  };

  esp_task_wdt_init(&wdt_config);

    esp_task_wdt_add(NULL);



    setServer();

    NetworkManager::instance().setWifiCredentials(Secrets::WIFI_SSID, Secrets::WIFI_PASSWORD);
    NetworkManager::instance().setServerAddress(Secrets::SERVER_ADDRESS, Secrets::SERVER_PORT, Secrets::SERVER_PORT_MQTT);

    SensorsInitializer::initSensors();
    TasksInitializer::initTasks();

    //setupCredentialsAndConnectToServer();

   // xTaskCreatePinnedToCore(SetupTask, "SetupTask", 81920, NULL, 5, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(SetupTask, "SetupTask", 20480, NULL, 5, NULL, tskNO_AFFINITY);


ESP_LOGI("maincpp", ">>>> setup() END <<<<");
    // Loop principal
    while (true) {
        // CommonSetup::instance().loop(); // Si es necesario
        esp_task_wdt_reset();
/*
        // Print CPU + memory info every 10 seconds
        static int counter = 0;
        if (++counter >= 10) {
            counter = 0;
            uint64_t currentIdle = idleCounter;
            idleCounter = 0;
            uint64_t maxPer10s = (uint64_t)maxIdleCount * 10;
            int cpuLoad = (maxPer10s > 0) ? 100 - (int)(currentIdle * 100 / maxPer10s) : -1;
            if (cpuLoad < 0) cpuLoad = 0;
            if (cpuLoad > 100) cpuLoad = 100;

            ESP_LOGW("MONITOR", "CPU: %d%% | Free heap: %lu KB | Min free: %lu KB | Tasks: %lu",
                     cpuLoad,
                     esp_get_free_heap_size() / 1024,
                     esp_get_minimum_free_heap_size() / 1024,
                     (unsigned long)uxTaskGetNumberOfTasks());
        }*/

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_LOGI("maincpp", ">>>> maincpp setup() END <<<<");
}



// 🆕 Esta es la tarea que se ejecutará aparte
void SetupTask(void* pvParameters) {
    setupCredentialsAndConnectToServer();
    vTaskDelete(NULL);  // Finaliza esta tarea
}


void setServer() {
    const gpio_num_t PIN_SERVER_SELECTION = GPIO_NUM_10;

    gpio_reset_pin(PIN_SERVER_SELECTION);
    gpio_set_direction(PIN_SERVER_SELECTION, GPIO_MODE_INPUT);
    int pinServeState = gpio_get_level(PIN_SERVER_SELECTION);


    Secrets::SERVER_ADDRESS = Secrets::SERVER_ADDRESS_LOCAL; //TODO

    /*if (pinServeState == 1) {
        Secrets::SERVER_ADDRESS = Secrets::SERVER_ADDRESS_REMOTE;
    } else {
        Secrets::SERVER_ADDRESS = Secrets::SERVER_ADDRESS_LOCAL;
    }*/

    ESP_LOGI(TAG, "### Current server: %s", Secrets::SERVER_ADDRESS.c_str());
}

void setupCredentialsAndConnectToServer() {
    UserCredentials actualCredentials(
        ACCOUNT_EMAIL,
        ACCOUNT_UUID,
        ACCOUNT_PASSWORD,
        DEVICE_UUID,
        "",
        DEVICE_NAME,
        DEVICE_DESCRIPTION
    );

    HttpProvider::set(new WifiHttpController());
    MqttProvider::set(new WifiMqttController());

    //NetworkConnectionProvider

    //auto *networkConnectionController = new WifiNetworkConnectionController();
    auto *networkConnectionController = &WifiNetworkConnectionController::instance();

    bool setupOk = CommonSetup::instance().setup(actualCredentials, networkConnectionController);

    if (!setupOk) {
        ESP_LOGE(TAG, "Setup failed");
    }
}
