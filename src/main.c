#include "main.h"
#include <display.h>
#include <esp_log.h>
#include <driver/i2c.h>
#include <ledcontrol.h>
#include <btnctrl.h>
#include <ui.h>
#include <rps_alg.h>
#include <sound.h>
#include <esp_random.h>

rps_choice playerChoice = ROCK; // Default choice is rock, player can cycle through choices
enum GameState currentState = STATE_MENU;

// Game flow: main menu -> game screen -> result screen -> back to game screen (if not win)

QueueHandle_t actionQueue;

void queueHandle() {
    while(1) {
		enum ActionType actionOpt;
		if(xQueueReceive(actionQueue, &actionOpt, portMAX_DELAY) == pdTRUE) {
            if(actionOpt == Action_Confirm) {
                switch(currentState) {
                    case STATE_MENU:
                        start_game();
                        break;
                    case STATE_PLAYING:
                        confirm_choice();
                        break;
                    case STATE_OVER_LOST:
                        start_game();
                        break;
                    case STATE_OVER_WIN:
                        break;
                }
            }
			if(actionOpt == Action_Select && currentState == STATE_PLAYING)
                cycle_choice();
		}
		else
			vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void selectHandle() {
    enum ActionType curAction = Action_Select;
    xQueueSend(actionQueue, &curAction, 0);
}

void confirmHandle() {
    enum ActionType curAction = Action_Confirm;
    xQueueSend(actionQueue, &curAction, 0);
}

void app_main() {
    // Initialize all components
    ESP_LOGI("main", "Starting application");
    setup_i2c();
    init_led();
    display_init();
    btnctrl_init();
    init_sound();
    
    // Queue Managers
    actionQueue = xQueueCreate(8, sizeof(enum ActionType));
    xTaskCreate(queueHandle, "main_control_handle", 4096, NULL, 5, NULL);

    set_led(1); // Turn on LED to indicate device is ready

    // Initiate main menu
    init_main_menu();
    play_sound(SOUND_MAIN_MENU);
    btnctrl_register_event(selectHandle, confirmHandle);
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

// Initialize game screen and register button events for cycling and confirming choice
void start_game() {
    init_game_screen(playerChoice);
    play_sound(SOUND_SELECT);
    currentState = STATE_PLAYING;
}

// Update game screen with current player choice when cycle button is pressed
void cycle_choice() {
    playerChoice = (playerChoice + 1) % 3; // Cycle through ROCK, PAPER, SCISSORS
    update_game_screen(playerChoice);
    play_sound(SOUND_SELECT);
}


// Determine and display the game outcome
void confirm_choice() {
    rps_choice cpuChoice = (rps_choice)(esp_random() % 3); // Generate random CPU choice
    rps_outcome outcome = determine_rps_outcome(playerChoice, cpuChoice); // Determine game outcome
    init_result_screen(outcome, playerChoice, cpuChoice); // Update screen with outcome and choices

    // Play sound based on outcome and register event to start new game if not a win
    switch (outcome) {
        case WIN:
            play_sound(SOUND_WIN);
            currentState = STATE_OVER_WIN;
            break;
        case LOSE:
            play_sound(SOUND_LOST);
            currentState = STATE_OVER_LOST;
            break;
        case DRAW:
            play_sound(SOUND_DRAW);
            currentState = STATE_OVER_LOST;
            break;
    }

}