
#include <string.h>

typedef struct {
    char* text;
    int page;
} displayQueue_t;

void display_init();
void display_text(const char* text);
void display_write_page(const char* text, int page, int isCenter);
void display_clear();