#ifndef RPS_STATE_H
#define RPS_STATE_H
// Dummy macro to prevent multiple definitions of the same enums in different files

enum rps_choices
{
    ROCK,
    PAPER,
    SCISSORS
} typedef rps_choice;

enum rps_outcomes
{
    DRAW,
    WIN,
    LOSE
} typedef rps_outcome;

#endif // RPS_STATE_H