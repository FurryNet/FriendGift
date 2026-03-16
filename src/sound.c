#include <sound.h>
#include "driver/ledc.h"
#include <string.h>
#include <esp_timer.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO 7
#define SAMPLE_PERIOD_US (1000000 / SAMPLE_RATE)

void pcm_playback_cb();
struct playback_data {
    uint8_t * buffer;
    size_t buffer_size;
    uint32_t curIndex;
};

void init_sound() {
    ledc_timer_config_t timerconf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_channel_config_t channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_timer_config(&timerconf);
    ledc_channel_config(&channel);
}

static void play_tone(int freq, int duration_ms)
{
    if (freq == 0) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    } else {
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 128); // 50% duty (for 8-bit resolution)
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }

    vTaskDelay(pdMS_TO_TICKS(duration_ms));
}

void play_main_menu_sound() {
    int freqs[] = {500, 700, 900};
    for (int i = 0; i < 3; i++) {
        play_tone(freqs[i], 200);
        play_tone(0, 100);  // short pause
    }
}

void play_win_sound() {

    int short_freqs[] = {400, 500, 600};
    for (int i = 0; i < 3; i++) {
        play_tone(short_freqs[i], 150);
        play_tone(0, 80);  // short pause
    }

    int long_freqs[] = {700, 900, 1200};
    for (int i = 0; i < 3; i++) {
        play_tone(long_freqs[i], 250);
    }

    play_tone(1500, 800);  // sustained high tone
    play_tone(0, 0);     // silence at end
}

void play_lost_sound()
{
    play_tone(1400, 120);
    play_tone(0, 80);

    play_tone(1100, 120);
    play_tone(0, 80);

    play_tone(900, 120);
    play_tone(0, 120);

    play_tone(700, 200);
    play_tone(500, 300);
    play_tone(300, 600);

    play_tone(0, 0); // silence at end
}

play_draw_sound() {
    int freqs[] = {600, 600, 600};
    for (int i = 0; i < 3; i++) {
        play_tone(freqs[i], 200);
        play_tone(0, 100);  // short pause
    }
}

void play_select_sound() {
    play_tone(2500, 250);
    play_tone(0, 0);
}
