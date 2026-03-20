
#include <string.h>

enum TaskOpt {
    DISPLAY_CLEAR,
    DISPLAY_WRITE
};

typedef struct {
    enum TaskOpt taskType;
    char text[17];
    int page;
} displayQueue_t;

void display_init();
void display_text(char* text);
void display_write_page(const char* text, int page, int isCenter);
void display_clear();