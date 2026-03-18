#include <iot_button.h>

void btnctrl_init();
void btnctrl_register_event(button_cb_t select_cb, button_cb_t confirm_cb);
void btnctrl_unregister_event();