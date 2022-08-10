#ifndef BUTTON_PRESS_HANDLER_H
#define BUTTON_PRESS_HANDLER_H 1

#include <zephyr/types.h>
#include <drivers/gpio.h>

struct Button_Press_Handler {
    struct gpio_dt_spec gpio;
    int press_timer_ms;
    int debounce_timer_ms;
    int poll_interval_ms;
    int* time_thresh;
    int completed_button_press_thresh;
    bool is_debounced;
    bool press_handled;
};

struct Button_Press_Handler get_button_press_handler();
void get_debounced_press(struct Button_Press_Handler* h);


#endif /* BUTTON_PRESS_HANDLER_H */
