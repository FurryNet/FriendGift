#include <string.h>

#define SAMPLE_RATE 8000

void init_sound();
void sample_start();
void sample_stop();
void play_pcm(uint8_t *pcm, size_t size);