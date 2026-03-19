#include <string.h>

#define SAMPLE_RATE 8000

enum soundOpts {
    SOUND_MAIN_MENU,
    SOUND_DRAW,
    SOUND_LOST,
    SOUND_WIN,
    SOUND_SELECT
};

void init_sound();

// void play_main_menu_sound();
// void play_draw_sound();
// void play_lost_sound();
// void play_win_sound();
// void play_select_sound();


void play_sound(enum soundOpts soundType);

