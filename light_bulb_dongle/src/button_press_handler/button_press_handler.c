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

bool is_button_pressed(struct gpio_dt_spec gpio) {
    return gpio_pin_get_dt(&gpio);
}

void get_debounced_press(struct Button_Press_Handler* h) {
    int button_is_pressed = is_button_pressed(h->gpio);
    // int button_is_pressed = is_button_pressed();
    if (button_is_pressed) {
        h->debounce_timer_ms += h->poll_interval_ms;
    }

    if (button_is_pressed != h->debounce_is_pressed) {
        // Increment when there is a difference
        h->debounce_timer_ms += h->poll_interval_ms;
    }

    if (button_is_pressed == h->debounce_is_pressed) {
        // Reset timer
        h->debounce_timer_ms = 0;
    }

    if (h->debounce_is_pressed) {
        h->press_timer_ms += h->poll_interval_ms;
    }

    if (h->debounce_timer_ms > 100) { // > 100ms
        // debounce time surpassed, debounced state change
        h->debounce_is_pressed = button_is_pressed;

        if (button_is_pressed == false) {
            // Rising edge/finger lifted
            h->press_handled = false;
            h->completed_button_press_thresh = h->press_timer_ms;
            h->press_timer_ms = 0;
        }
    }
}
