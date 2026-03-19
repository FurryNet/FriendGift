#include <display.h>
#include <string.h>
#include "driver/i2c.h"
#include <ssd1306.h>
#include <font8x8_basic.h>
#include <esp_log.h>
#include <math.h>
#include <freertos/queue.h>

#define SDA_PIN 25
#define SCL_PIN 26

#define TAG "display"

SSD1306_t dev;
QueueHandle_t writePageQueue;

// Initalize the display pins
void display_clear();
void display_write_queue(void *pvParameters);
void display_init()
{
	// Init stuff
	writePageQueue = xQueueCreate(32, sizeof(displayQueue_t*));
	// Addr Stuff
	dev._address = 0x3C;
	dev._flip = false;
	ssd1306_init(&dev, 128, 64);
	display_clear();
	xTaskCreate(display_write_queue, "display_write_queue", 4096, NULL, 5, NULL);
}

// Clean the display
void display_clear() {
	//ssd1306_clear_screen(&dev, false);
	displayQueue_t* data = calloc(1, sizeof(displayQueue_t));
	data->taskType = DISPLAY_CLEAR;
	xQueueSend(writePageQueue, &data, 0);
}

// Render text on the display
void display_text(char* text) {
	uint8_t text_len = strlen(text);
    ssd1306_display_text(&dev, 0, text, text_len, false);
}

// Write text to a specific line on the display (isCenter is used to center the text on the line)
void display_write_page(const char* text, int page, int isCenter) {
	displayQueue_t* data = calloc(1, sizeof(displayQueue_t));
	data->taskType = DISPLAY_WRITE;
	data->page = page;

	// Get the length and allocate the space
	size_t text_len = strlen(text); // Each line only supports 16 characters
	text_len = text_len > 16 ? 16 : text_len;

	// Check if the text needs to be centered
	if(isCenter == 1) {
		int padLen = ceil((16 - text_len) / 2);
		for(int i = 0; i < padLen; i++)
			data->text[i] = ' ';
		for(int i = padLen; i < padLen+text_len; i++)
			data->text[i] = text[i - padLen];
		text_len+=padLen;
	} else strncpy(data->text, text, 16);

	// Add it to the queue
	xQueueSend(writePageQueue, &data, 0);
}

/*
Internal function to handle writeLineQueue
Executing ssd1306 display command simultaneously causes the display to glitch out
*/
void display_write_queue(void *pvParameters) {
	while(1) {
		displayQueue_t* data;
		if(xQueueReceive(writePageQueue, &data, portMAX_DELAY) == pdTRUE) {
			// Dedicated Clear Task
			if(data->taskType == DISPLAY_CLEAR) {
				ssd1306_clear_screen(&dev, false);
				free(data);
				continue;
			}

			/* Write Task */
			if(data->taskType != DISPLAY_WRITE) {
				free(data);
				continue; // Might be bad memory data?
			}

			char zeros[16] = {0};
			// The reason to write 0s is because the display sometime persist the text from previous queue and 0s somehow stops this issue
			ssd1306_display_text(&dev, data->page, zeros, 16, false);
			vTaskDelay(pdMS_TO_TICKS(10));
			ssd1306_display_text(&dev, data->page, data->text, 16, false);
			free(data);
		}
		else
			vTaskDelay(pdMS_TO_TICKS(5));
	}
}