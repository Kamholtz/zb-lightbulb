#include "../../src/button_press_handler/button_press_handler.h"
#include <unity.h>
#include "drivers/mock_gpio.h"


#define RUN_LED_BLINK_INTERVAL          25

void test_get_button_press_handler() {
	// zassert_equal(0, 1, "0 is 0");
    struct Button_Press_Handler handler = get_button_press_handler();
	TEST_ASSERT_EQUAL(0, handler.completed_button_press_thresh);
}

void test_get_debounced_press() {
    struct Button_Press_Handler handler = get_button_press_handler();
    __wrap_gpio_pin_get_dt_ExpectAnyArgsAndReturn(0);
    handler.poll_interval_ms = 30;
    handler.debounce_timer_ms = 200;
    handler.press_timer_ms = 4970;
    handler.is_debounced = true;
    get_debounced_press(&handler);

    TEST_ASSERT_EQUAL(5000, handler.completed_button_press_thresh);
}

/* It is required to be added to each test. That is because unity is using
 * different main signature (returns int) and zephyr expects main which does
 * not return value.
 */
extern int unity_main(void);

void main(void)
{
	(void)unity_main();
}
