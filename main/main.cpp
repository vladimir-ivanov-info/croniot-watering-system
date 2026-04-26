#include "../secrets.h"
#include "CommonSetup.h"
#include "CroniotConfig.h"

#include "CommonConstants.h"
#include "TasksInitializer.h"
#include "SensorsInitializer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"

static const char *TAG = "Main";

static volatile uint32_t idleCounter = 0;
static uint32_t maxIdleCount = 0;

static void cpuIdleTask(void*) {
    while (true) {
        idleCounter++;
        if ((idleCounter % 1000) == 0) {
            taskYIELD();
        }
    }
}

void setServer();
void SetupTask(void*);

extern "C" void app_main(void) {
    esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGI(TAG, "Starting setup");

    xTaskCreate(cpuIdleTask, "cpuIdle", 1024, nullptr, 0, nullptr);

    vTaskPrioritySet(nullptr, 0);
    idleCounter = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));
    maxIdleCount = idleCounter;
    vTaskPrioritySet(nullptr, 1);
    ESP_LOGW(TAG, "CPU calibration: maxIdleCount=%lu/sec", maxIdleCount);

    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 100000,
        .idle_core_mask = BIT(0),
        .trigger_panic = true,
    };
    esp_task_wdt_init(&wdt_config);
    esp_task_wdt_add(NULL);

    setServer();

    SensorsInitializer::initSensors();
    TasksInitializer::initTasks();

    xTaskCreatePinnedToCore(SetupTask, "SetupTask", 20480, NULL, 5, NULL, tskNO_AFFINITY);

    ESP_LOGI("maincpp", ">>>> setup() END <<<<");
    while (true) {
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void SetupTask(void* pvParameters) {
    croniot::CroniotConfig config;
    config.deviceUuid        = DEVICE_UUID;
    config.deviceName        = DEVICE_NAME;
    config.deviceDescription = DEVICE_DESCRIPTION;
    config.accountEmail      = ACCOUNT_EMAIL;
    config.accountUuid       = ACCOUNT_UUID;
    config.accountPassword   = ACCOUNT_PASSWORD;

    config.channels = { croniot::ChannelType::Remote/*, croniot::ChannelType::Ble */};

    config.remote.transport       = croniot::RemoteTransport::Wifi;
    config.remote.serverAddress   = Secrets::SERVER_ADDRESS;
    config.remote.serverHttpPort  = Secrets::SERVER_PORT;
    config.remote.serverMqttPort  = Secrets::SERVER_PORT_MQTT;
    config.remote.wifiSsid        = Secrets::WIFI_SSID;
    config.remote.wifiPassword    = Secrets::WIFI_PASSWORD;

    config.ble.localName = DEVICE_NAME;

    if (!CommonSetup::instance().setup(config)) {
        ESP_LOGE(TAG, "Setup failed");
    }

    vTaskDelete(NULL);
}

void setServer() {
    const gpio_num_t PIN_SERVER_SELECTION = GPIO_NUM_10;

    gpio_reset_pin(PIN_SERVER_SELECTION);
    gpio_set_direction(PIN_SERVER_SELECTION, GPIO_MODE_INPUT);
    int pinServeState = gpio_get_level(PIN_SERVER_SELECTION);
    (void)pinServeState;

    Secrets::SERVER_ADDRESS = Secrets::SERVER_ADDRESS_LOCAL;

    ESP_LOGI(TAG, "### Current server: %s", Secrets::SERVER_ADDRESS.c_str());
}
