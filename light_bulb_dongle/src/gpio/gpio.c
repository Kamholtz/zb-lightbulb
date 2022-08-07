#include "gpio.h"

int gpio_pin_get_TEST(struct gpio_dt_spec* a) {
    return gpio_pin_get_dt(a);
}