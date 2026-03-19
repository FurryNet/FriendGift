#include <sound.h>
#include "driver/ledc.h"
#include <string.h>
#include <esp_timer.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/queue.h>

#define GPIO 7
#define SAMPLE_PERIOD_US (1000000 / SAMPLE_RATE)
QueueHandle_t soundQueue;

void play_main_menu_sound();
void play_draw_sound();
void play_lost_sound();
void play_win_sound();
void play_select_sound();

void play_sound(enum soundOpts soundType) {
    xQueueSend(soundQueue, &soundType, 0);
}

void sound_queue_process() {
    while(1) {
		enum soundOpts soundOpt;
		if(xQueueReceive(soundQueue, &soundOpt, portMAX_DELAY) == pdTRUE) {
			switch(soundOpt) {
                case SOUND_MAIN_MENU:
                    play_main_menu_sound();
                    break;
                case SOUND_DRAW:
                    play_draw_sound();
                    break;
                case SOUND_LOST:
                    play_lost_sound();
                    break;
                case SOUND_WIN:
                    play_win_sound();
                    break;
                case SOUND_SELECT:
                    play_select_sound();
                    break;
            }
		}
		else
			vTaskDelay(pdMS_TO_TICKS(5));
	}
}

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

    // Audio Queue Handles
    soundQueue = xQueueCreate(8, sizeof(enum soundOpts));
    xTaskCreate(sound_queue_process, "sound_queue_process", 2048, NULL, 5, NULL);
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

void play_draw_sound() {
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
