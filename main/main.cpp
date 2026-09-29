#include "../secrets.h"
#include "CommonSetup.h"
#include "CroniotConfig.h"
#include "log/Log.h"

#include "CommonConstants.h"
#include "TasksInitializer.h"
#include "SensorsInitializer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_app_desc.h"
#include "esp_system.h"

static const char *TAG = "Main";

static volatile uint32_t idleCounter = 0;
static uint32_t maxIdleCount = 0;

// Minimal, self-contained boot counter. Deliberately not using croniot-iot's
// espp::Nvs wrapper: that has zero production callers today, and a boot
// counter isn't the moment to introduce it as a new dependency.
static constexpr const char *BOOT_NVS_NAMESPACE = "croniot_boot";
static constexpr const char *BOOT_COUNT_KEY = "count";

static uint32_t incrementAndGetBootCount() {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(BOOT_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open(%s) failed: %s", BOOT_NVS_NAMESPACE, esp_err_to_name(err));
        return 0;
    }

    uint32_t count = 0;
    err = nvs_get_u32(handle, BOOT_COUNT_KEY, &count);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "nvs_get_u32(%s) failed: %s", BOOT_COUNT_KEY, esp_err_to_name(err));
    }

    count++;

    err = nvs_set_u32(handle, BOOT_COUNT_KEY, count);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_set_u32(%s) failed: %s", BOOT_COUNT_KEY, esp_err_to_name(err));
    } else {
        err = nvs_commit(handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "nvs_commit failed: %s", esp_err_to_name(err));
        }
    }

    nvs_close(handle);
    return count;
}

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
    // First line, on purpose: CommonSetup::setup() only runs later, inside
    // SetupTask (see SetupTask() below), so installing the hook here is
    // what captures initSensors()/initTasks() and anything that fails
    // before that point instead of missing it.
    croniot::log::LogConfig logCfg;
    logCfg.profile = croniot::log::Profile::RealTime;  // plugged in; croniot::log::Profile::Batched for a battery build
    logCfg.capture = croniot::log::Level::Info;
    logCfg.tags = {{"TaskWaterPlants", croniot::log::Level::Debug}};
    croniot::log::init(logCfg);

    esp_log_level_set(TAG, ESP_LOG_INFO);

    // NVS must be up before the boot counter below reads/writes it. This is
    // earlier than WifiNetworkConnectionController::init()'s own
    // nvs_flash_init() call (which only runs later, asynchronously, inside
    // SetupTask). That later call is safe: nvs_flash_init() is idempotent
    // per-partition (NVSPartitionManager::init_partition() returns ESP_OK
    // immediately if the "nvs" partition is already initialized), confirmed
    // against the ESP-IDF 5.5.3 nvs_flash source.
    esp_err_t nvsErr = nvs_flash_init();
    if (nvsErr == ESP_ERR_NVS_NO_FREE_PAGES || nvsErr == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition needs erase (%s); erasing and retrying", esp_err_to_name(nvsErr));
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvsErr = nvs_flash_init();
    }
    if (nvsErr != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init failed: %s", esp_err_to_name(nvsErr));
    }

    const uint32_t bootCount = incrementAndGetBootCount();
    const esp_app_desc_t *appDesc = esp_app_get_description();
    const esp_reset_reason_t resetReason = esp_reset_reason();
    ESP_LOGI(TAG, "Boot #%lu | version=%s | built=%s %s | idf=%s | reset_reason=%d",
             bootCount, appDesc->version, appDesc->date, appDesc->time, appDesc->idf_ver, (int)resetReason);

    ESP_LOGI(TAG, "Starting setup");

    xTaskCreate(cpuIdleTask, "cpuIdle", 1024, nullptr, 0, nullptr);

    vTaskPrioritySet(nullptr, 0);
    idleCounter = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));
    maxIdleCount = idleCounter;
    vTaskPrioritySet(nullptr, 1);
    ESP_LOGW(TAG, "CPU calibration: maxIdleCount=%lu/sec", maxIdleCount);

    // CONFIG_ESP_TASK_WDT_INIT=y in this project's sdkconfig means the IDF
    // startup code already auto-initializes the TWDT (with the default 5s
    // config) before app_main() runs. Calling esp_task_wdt_init() again here
    // therefore returns ESP_ERR_INVALID_STATE, and the previous code ignored
    // that return value entirely -- silently leaving the *default* 5s config
    // active instead of the intended 100s/panic one. Reconfigure the
    // already-running instance instead in that case.
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 100000,
        .idle_core_mask = BIT(0),
        .trigger_panic = true,
    };
    esp_err_t wdtErr = esp_task_wdt_init(&wdt_config);
    if (wdtErr == ESP_ERR_INVALID_STATE) {
        wdtErr = esp_task_wdt_reconfigure(&wdt_config);
        if (wdtErr != ESP_OK) {
            ESP_LOGE(TAG, "esp_task_wdt_reconfigure failed: %s", esp_err_to_name(wdtErr));
        }
    } else if (wdtErr != ESP_OK) {
        ESP_LOGE(TAG, "esp_task_wdt_init failed: %s", esp_err_to_name(wdtErr));
    }
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
    //config.channels = { croniot::ChannelType::Ble };
    
    config.remote.transport       = croniot::RemoteTransport::Wifi;
    config.remote.serverAddress   = Secrets::SERVER_ADDRESS;
    config.remote.serverHttpPort  = Secrets::SERVER_PORT;
    config.remote.serverMqttPort  = Secrets::SERVER_PORT_MQTT;
    config.remote.wifiSsid        = Secrets::WIFI_SSID;
    config.remote.wifiPassword    = Secrets::WIFI_PASSWORD;

    config.ble.localName = DEVICE_NAME;
    config.ble.password  = BLE_PASSWORD;

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
