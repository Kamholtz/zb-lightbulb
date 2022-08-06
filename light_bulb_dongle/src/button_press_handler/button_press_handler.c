#include "button_press_handler.h"

struct Button_Press_Handler get_button_press_handler(struct gpio_dt_spec button) {
    struct Button_Press_Handler button_press_handler;
    button_press_handler.gpio = button;
    button_press_handler.press_timer_ms = 0;
    button_press_handler.debounce_timer_ms = 0;
    button_press_handler.time_thresh = 0;
    button_press_handler.completed_button_press_thresh = 0;

    return button_press_handler;
}
