#include "ADS1115Controller.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

static const char* TAG = "ADS1115Controller";

ADS1115Controller::ADS1115Controller() {
    // Crear instancia de I2C con los pines correctos
    i2c = std::make_unique<espp::I2c>(espp::I2c::Config{
        .port = I2C_NUM_0,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .timeout_ms = 30,
        .clk_speed = 100 * 1000,  // 100kHz Standard Mode (default 400kHz too fast without strong pull-ups)
    });

    // Crear instancia de ADS1115 con lambdas de lectura/escritura
    ads = std::make_unique<espp::Ads1x15>(espp::Ads1x15::Ads1115Config{
        .device_address = espp::Ads1x15::DEFAULT_ADDRESS,
        .write = [this](uint8_t addr, const uint8_t* data, size_t len) {
            return i2c->write(addr, data, len);
        },
        .read = [this](uint8_t addr, uint8_t* data, size_t len) {
            return i2c->read(addr, data, len);
        }
    });

    ESP_LOGI(TAG, "ADS1115 inicializado");
}

ADS1115Controller::~ADS1115Controller() {
    ads.reset();
    i2c.reset();
}

int ADS1115Controller::readValueBlocking(int pin) {
    std::error_code ec;

    // Leer milivoltios desde el canal especificado
    auto value_mv = ads->sample_mv(pin, ec);

    if (ec) {
        ESP_LOGE(TAG, "Error leyendo el canal %d: %s", pin, ec.message().c_str());
        return -1;
    }

    //ESP_LOGI(TAG, "Canal %d: %.3f mV", pin, value_mv);
    return static_cast<int>(value_mv); // Si quieres retornarlo como int en mV
}
