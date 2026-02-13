#include <rps_state.h>

void init_main_menu();
void init_game_screen();
void update_game_screen(rps_choice player_choice);
void init_result_screen(rps_outcome outcome, rps_choice player_choice, rps_choice cpu_choice, char* gift_card_code);