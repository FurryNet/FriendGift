#define sda_pin 21
#define scl_pin 20
#define frequency 400000

void setup_i2c();
void start_game();
void cycle_choice();
void confirm_choice();

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_OVER_LOST,
    STATE_OVER_WIN,
};

enum ActionType {
    Action_Select,
    Action_Confirm,
};