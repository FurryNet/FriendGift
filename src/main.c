#include <display.h>
#include <esp_log.h>
#include <driver/i2c.h>
#include <ledcontrol.h>
#include <sound.h>

#define sda_pin 21
#define scl_pin 20
#define frequency 400000

void setup_i2c();
void conf_led(int enable);
void app_main() {
    ESP_LOGI("main", "Starting application");
    setup_i2c();
    init_led();
    display_init();
    init_sound();
    display_text("Goodbye world!");
    while(true) {
        set_led(1);
        sample_start();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        set_led(0);
        sample_stop();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup_i2c() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda_pin;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = scl_pin;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = frequency;
    conf.clk_flags = 0;
    i2c_param_config(I2C_NUM_0, &conf);
    if(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) == ESP_OK)
        ESP_LOGI("I2C_Setup", "hdc2080 driver installed successfully");
    else
        ESP_LOGE("I2C_Setup", "hdc2080 driver failed to install");
}