#include "../../src/button_press_handler/button_press_handler.h"
#include <unity.h>


void test_get_button_press_handler() {
	// zassert_equal(0, 1, "0 is 0");
    struct Button_Press_Handler handler = get_button_press_handler();
	TEST_ASSERT_EQUAL(1, handler.completed_button_press_thresh);
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
