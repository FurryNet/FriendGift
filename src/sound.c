#include <sound.h>
#include "driver/ledc.h"
#include <string.h>
#include <esp_timer.h>
#include <esp_log.h>

#define GPIO 7
#define SAMPLE_PERIOD_US (1000000 / SAMPLE_RATE)

void pcm_playback_cb();
struct playback_data {
    uint8_t * buffer;
    size_t buffer_size;
    uint32_t curIndex;
};
void free_pb_data(struct playback_data * data) {
    free(data->buffer);
    free(data);
}

void init_sound() {
    ledc_timer_config_t timerconf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 62500,
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

void sample_start() {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 64);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void sample_stop() {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

// Play 8-bit PCM data via 44.1kHz sample rate.
esp_timer_handle_t playback_timer = NULL;
struct playback_data pb_data = {0};
void play_pcm(uint8_t *pcm, size_t size) {
    // Clean up prev audio callback
    if(playback_timer != NULL) {
        ESP_LOGI("sound", "Overriding current audio");
        if(esp_timer_is_active(playback_timer))
            esp_timer_stop(playback_timer);
        esp_timer_delete(playback_timer);
        playback_timer = NULL;

        if(pb_data.buffer != NULL) {
            free(pb_data.buffer);
            memset(&pb_data, 0x0, sizeof(struct playback_data));
        }
    }

    // Initialize playback stuff
    pb_data.buffer = malloc(size*sizeof(uint8_t));
    pb_data.buffer_size = size;
    pb_data.curIndex = 0;
    memcpy(&pb_data.buffer, &pcm, size*sizeof(uint8_t));

    const esp_timer_create_args_t timer_args = {
            .callback = &pcm_playback_cb,
            .arg = NULL,
            .name = "pcm_playback"
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &playback_timer));
    ESP_ERROR_CHECK(esp_timer_start_once(playback_timer, SAMPLE_PERIOD_US));
}


void pcm_playback_cb() {

}
