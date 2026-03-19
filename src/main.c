#include <display.h>
#include <esp_log.h>
#include <driver/i2c.h>
#include <ledcontrol.h>
#include <btnctrl.h>
#include <ui.h>
#include <rps_alg.h>
#include <sound.h>
#include <esp_random.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define sda_pin 21
#define scl_pin 20
#define frequency 400000

typedef enum {
    EVT_START_GAME,
    EVT_CYCLE_CHOICE,
    EVT_CONFIRM_CHOICE,
} game_event_t;

static QueueHandle_t game_queue;

void setup_i2c();
void start_game();
void cycle_choice();
void confirm_choice();
static void game_task(void *pvParameters);

// Button callbacks — just enqueue an event and return immediately
static void on_start_pressed(void *arg, void *data)   { game_event_t e = EVT_START_GAME;    xQueueSend(game_queue, &e, 0); }
static void on_cycle_pressed(void *arg, void *data)    { game_event_t e = EVT_CYCLE_CHOICE;   xQueueSend(game_queue, &e, 0); }
static void on_confirm_pressed(void *arg, void *data)  { game_event_t e = EVT_CONFIRM_CHOICE; xQueueSend(game_queue, &e, 0); }

rps_choice playerChoice = ROCK; // Default choice is rock, player can cycle through choices

// Game flow: main menu -> game screen -> result screen -> back to game screen (if not win)

void app_main() {
    // Initialize all components
    ESP_LOGI("main", "Starting application");
    setup_i2c();
    init_led();
    display_init();
    btnctrl_init();
    init_sound();

    set_led(1); // Turn on LED to indicate device is ready

    game_queue = xQueueCreate(4, sizeof(game_event_t));

    // Initiate main menu
    init_main_menu();
    play_main_menu_sound();
    btnctrl_register_event(NULL, on_start_pressed);

    // Game task runs all blocking logic (I2C display, sounds) on its own stack
    xTaskCreate(game_task, "game_task", 4096, NULL, 5, NULL);
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

// Game task: processes button events on a dedicated stack
static void game_task(void *pvParameters) {
    game_event_t evt;
    while (1) {
        if (xQueueReceive(game_queue, &evt, portMAX_DELAY) == pdTRUE) {
            switch (evt) {
                case EVT_START_GAME:    start_game();     break;
                case EVT_CYCLE_CHOICE:  cycle_choice();   break;
                case EVT_CONFIRM_CHOICE: confirm_choice(); break;
            }
        }
    }
}

// Initialize game screen and register button events for cycling and confirming choice
void start_game() {
    btnctrl_unregister_event(); // Clear main menu event
    xQueueReset(game_queue);   // Discard any stale button events

    init_game_screen(playerChoice);
    play_select_sound();

    btnctrl_register_event(on_cycle_pressed, on_confirm_pressed);
}

// Update game screen with current player choice when cycle button is pressed
void cycle_choice() {
    playerChoice = (playerChoice + 1) % 3; // Cycle through ROCK, PAPER, SCISSORS
    update_game_screen(playerChoice);
    play_select_sound();
}


// Determine and display the game outcome
void confirm_choice() {
    btnctrl_unregister_event(); // Clear start_game events
    xQueueReset(game_queue);   // Discard any stale button events

    rps_choice cpuChoice = (rps_choice)(esp_random() % 3); // Generate random CPU choice
    rps_outcome outcome = determine_rps_outcome(playerChoice, cpuChoice); // Determine game outcome
    init_result_screen(outcome, playerChoice, cpuChoice); // Update screen with outcome and choices

    // Play sound based on outcome and register event to start new game if not a win
    switch (outcome) {
        case WIN:
            play_win_sound();
            break;
        case LOSE:
            play_lost_sound();
            btnctrl_register_event(NULL, on_start_pressed);
            break;
        case DRAW:
            play_draw_sound();
            btnctrl_register_event(NULL, on_start_pressed);
            break;
    }

}