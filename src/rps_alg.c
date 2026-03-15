#include "rps_alg.h"
#include <esp_random.h>

// Produces CPU input, compares against player's input, and determines game outcome
rps_outcome rps_algorithm(rps_choice player_input)
{
    rps_choice cpu_input = (rps_choice)(esp_random() % 3); // Generate random CPU choice
    
    // Determine outcome
    if (player_input == cpu_input) {
        return DRAW;
    } else if ((player_input == ROCK && cpu_input == SCISSORS) ||
               (player_input == SCISSORS && cpu_input == PAPER) ||
               (player_input == PAPER && cpu_input == ROCK)) {
        return WIN;
    } else {
        return LOSE; 
    }
}