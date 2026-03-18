#include <display.h>
#include <ui.h>
#include <string.h>
#include <stdio.h>

const char* giftCode = "{CODEHERE}";

// Initialize the main menu screen
void init_main_menu() {
    display_clear();
    display_write_page("Welcome to", 0, 1);
    display_write_page("Rock Paper Scissors!", 1, 1);
    display_write_page("Press START", 2, 1);
}

// Initialize the game screen with instructions and default choice
void init_game_screen(rps_choice player_choice) {
    display_clear();

    char choice_str[10] = "";
    switch(player_choice) {
        case ROCK: strcpy(choice_str, "Rock"); break;
        case PAPER: strcpy(choice_str, "Paper"); break;
        case SCISSORS: strcpy(choice_str, "Scissors"); break;
    }

    display_write_page("Press CYCLE", 0, 1);
    display_write_page("to choose:", 1, 1);
    display_write_page(choice_str, 2, 1); // Assume first choice is rock, player cycles otherwise
}

// Update the game screen with the player's current choice
void update_game_screen(rps_choice player_choice) {
    char choice_str[10] = "";
    switch(player_choice) {
        case ROCK: strcpy(choice_str, "Rock"); break;
        case PAPER: strcpy(choice_str, "Paper"); break;
        case SCISSORS: strcpy(choice_str, "Scissors"); break;
    }
    display_write_page(choice_str, 2, 1);
}

// Display game result (+ gift card code if player wins)
void init_result_screen(rps_outcome outcome, rps_choice player_choice, rps_choice cpu_choice) {
    display_clear();
    char outcome_str[15] = "";
    char player_choice_str[10] = "";
    char cpu_choice_str[10] = "";
    char player_vs_cpu_str[30] = "";

    switch(player_choice) {
        case ROCK: strcpy(player_choice_str, "Rock"); break;
        case PAPER: strcpy(player_choice_str, "Paper"); break;
        case SCISSORS: strcpy(player_choice_str, "Scissors"); break;
    }

    switch(cpu_choice) {
        case ROCK: strcpy(cpu_choice_str, "Rock"); break;
        case PAPER: strcpy(cpu_choice_str, "Paper"); break;
        case SCISSORS: strcpy(cpu_choice_str, "Scissors"); break;
    }

    switch(outcome) {
        case DRAW: 
            strcpy(player_vs_cpu_str, player_choice_str);
            strcat(player_vs_cpu_str, " matches ");
            strcat(player_vs_cpu_str, cpu_choice_str);

            strcpy(outcome_str, "It's a draw!"); 

            break;
        case WIN:
            strcpy(player_vs_cpu_str, player_choice_str);
            strcat(player_vs_cpu_str, " beats ");
            strcat(player_vs_cpu_str, cpu_choice_str);

            strcpy(outcome_str, "You win!");

            break;
        case LOSE: 
            strcpy(player_vs_cpu_str, cpu_choice_str);
            strcat(player_vs_cpu_str, " beats ");
            strcat(player_vs_cpu_str, player_choice_str);

            strcpy(outcome_str, "You lose!");
            
            break;
    }

    display_write_page(player_vs_cpu_str, 0, 1);
    display_write_page(outcome_str, 1, 1);

    // If win, display the gift card code, otherwise prompt to try again
    if (outcome == WIN) {
        // char code_str[17]; // 16 horizonal max display + null term
        // snprintf(code_str, sizeof(code_str), "Code: %s", gift_card_code);
        display_write_page("Gift Card Code:", 2, 1);
        display_write_page(giftCode, 3, 1);
    } else {
        display_write_page("Press START to", 2, 1);
        display_write_page("try again!", 3, 1);
    }
}

