#include <ledcontrol.h>
#include <driver/gpio.h>

#define GPIO 6

void init_led() {
    gpio_reset_pin(GPIO);
    gpio_set_direction(GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO, 0);
}

void set_led(int on) {
    gpio_set_level(GPIO, on);
}