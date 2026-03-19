#include <btnctrl.h>
#include <button_gpio.h>
#include <esp_log.h>

button_handle_t select_btn = NULL;
button_handle_t confirm_btn = NULL;

void btnctrl_init() {
    button_config_t select_btn_cfg = {0};
    button_config_t confirm_btn_cfg = {0};
    button_gpio_config_t select_btn_gpio_cfg = {
        .gpio_num = 22,
        .active_level = 0,
    };
    button_gpio_config_t confirm_btn_gpio_cfg = {
        .gpio_num = 23,
        .active_level = 0,
    };
    esp_err_t s_res = iot_button_new_gpio_device(&select_btn_cfg, &select_btn_gpio_cfg, &select_btn);
    esp_err_t c_res = iot_button_new_gpio_device(&confirm_btn_cfg, &confirm_btn_gpio_cfg, &confirm_btn);
    if(select_btn == NULL || confirm_btn == NULL || s_res != ESP_OK || c_res != ESP_OK) {
        ESP_LOGE("btnctrl", "Failed to initialize buttons");
    }
}

// Register button events for select and confirm actions
void btnctrl_register_event(button_cb_t select_cb, button_cb_t confirm_cb) {
    if (select_cb != NULL)
        iot_button_register_cb(select_btn, BUTTON_SINGLE_CLICK, NULL, select_cb, NULL);
    
    if (confirm_cb != NULL)
        iot_button_register_cb(confirm_btn, BUTTON_SINGLE_CLICK, NULL, confirm_cb, NULL);
}

// Unregister button events (used when transitioning between screens to prevent unintended actions)
void btnctrl_unregister_event() {
    iot_button_unregister_cb(select_btn, BUTTON_SINGLE_CLICK, NULL);
    iot_button_unregister_cb(confirm_btn, BUTTON_SINGLE_CLICK, NULL);
}
