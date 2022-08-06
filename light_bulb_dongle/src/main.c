/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Simple Zigbee network coordinator implementation
 */

#include <zephyr/types.h>
#include <zephyr.h>
#include <device.h>
#include <soc.h>
#include <drivers/pwm.h>
#include <logging/log.h>
#include <dk_buttons_and_leds.h>
#include <drivers/gpio.h>
#include <settings/settings.h>

#define ZB_ZCL_SUPPORT_CLUSTER_OCCUPANCY_SENSING 1 // Likely don't need this, but keep for the meantime
// #define ZB_HA_DEFINE_DEVICE_ON_OFF_SWITCH

#include <zboss_api.h>
#include <zboss_api_addons.h>
#include <zb_mem_config_med.h>
#include <zigbee/zigbee_app_utils.h>
#include <zigbee/zigbee_error_handler.h>
#include <zigbee/zigbee_zcl_scenes.h>
// #include <zb_zcl_occupancy_sensing.h>
#include <zb_nrf_platform.h>
#include <zb_zcl_on_off.h>
#include "zb_zcl_occupancy_sensing_addons.h"
#include "zb_zcl_on_off_switch_conf_addons.h"
#include "zb_ha_occupancy_sensor.h"
#include "zcl_occupancy_sensing_2.h"
#include "zb_dimmable_light.h"
#include "zb_ha_on_off_switch.h"
#include "button_press_handler/button_press_handler.h"
// #include <zb_zcl_occupancy_sensing.h>

// #define INCLUDE_DONGLE_USB_LOGGING 1 // If this is uncommented, USB output will be enabled. In its current state, it only transmits what it receives. It is still a POC for eventually setting up USB logging backend of the dongle
#ifdef INCLUDE_DONGLE_USB_LOGGING
#include "usb_logging.h"
#endif

#define RUN_STATUS_LED                  DK_LED1
#define RUN_LED_BLINK_INTERVAL          25

/* Device endpoint, used to receive light controlling commands. */
#define HA_DIMMABLE_LIGHT_ENDPOINT      10

/* Device endpoint, used to receive occupancy sensing commands. */
#define HA_OCCUPANCY_SENSING_ENDPOINT   11

/* Device endpoint, used to receive on off switch commands. */
#define HA_ON_OFF_SWITCH_ENDPOINT   12

/* Version of the application software (1 byte). */
#define BULB_INIT_BASIC_APP_VERSION     01

/* Version of the implementation of the Zigbee stack (1 byte). */
#define BULB_INIT_BASIC_STACK_VERSION   10

/* Version of the hardware of the device (1 byte). */
#define BULB_INIT_BASIC_HW_VERSION      11

/* Manufacturer name (32 bytes). */
#define BULB_INIT_BASIC_MANUF_NAME      "Nordic"

/* Model number assigned by manufacturer (32-bytes long string). */
#define BULB_INIT_BASIC_MODEL_ID        "Dimable_Light_v0.1" // DK
// #define BULB_INIT_BASIC_MODEL_ID        "Dimable_Light_v0.1" // Dongle

/* First 8 bytes specify the date of manufacturer of the device
 * in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific.
 */
#define BULB_INIT_BASIC_DATE_CODE       "20200329"

/* Type of power sources available for the device.
 * For possible values see section 3.2.2.2.8 of ZCL specification.
 */
#define BULB_INIT_BASIC_POWER_SOURCE    ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE

/* Describes the physical location of the device (16 bytes).
 * May be modified during commisioning process.
 */
#define BULB_INIT_BASIC_LOCATION_DESC   "Office desk"

/* Describes the type of physical environment.
 * For possible values see section 3.2.2.2.10 of ZCL specification.
 */
#define BULB_INIT_BASIC_PH_ENV          ZB_ZCL_BASIC_ENV_UNSPECIFIED

/* LED indicating that light switch successfully joind Zigbee network. */
#define ZIGBEE_NETWORK_STATE_LED        DK_LED3

/* LED immitaing dimmable light bulb - define for informational
 * purposes only.
 */
#define BULB_LED                        DK_LED4

/* Button used to enter the Bulb into the Identify mode. */
#define IDENTIFY_MODE_BUTTON            DK_BTN4_MSK

/* Use onboard led4 to act as a light bulb.
 * The app.overlay file has this at node label "pwm_led3" in /pwmleds.
 */
#define PWM_DK_LED4_NODE                DT_ALIAS(pwm_on_board)

/* Nordic PWM nodes don't have flags cells in their specifiers, so
 * this is just future-proofing.
 */
#define FLAGS_OR_ZERO(node) \
	COND_CODE_1(DT_PHA_HAS_CELL(node, pwms, flags), \
		    (DT_PWMS_FLAGS(node)), (0))

#if DT_NODE_HAS_STATUS(PWM_DK_LED4_NODE, okay)
/* Get the defines from overlay file. */
#define PWM_DK_LED4_CTLR                DT_PWMS_CTLR(PWM_DK_LED4_NODE)
#define PWM_DK_LED4_CHANNEL             DT_PWMS_CHANNEL(PWM_DK_LED4_NODE)
#define PWM_DK_LED4_FLAGS               FLAGS_OR_ZERO(PWM_DK_LED4_NODE)
#else
#error "Choose supported PWM driver"
#endif

/* Use onboard led4 to act as a light bulb.
 * The app.overlay file has this at node label "pwm_led3" in /pwmleds.
 */
#define PWM_PIN_NODE                DT_NODELABEL(ext_pwm_led)

// TODO: the #if check should be on PWM_PIN_NODE, not PWM_DK_LED4_NODE
#if DT_NODE_HAS_STATUS(PWM_DK_LED4_NODE, okay)
/* Get the defines from overlay file. */
#define PWM_PIN_CTLR                DT_PWMS_CTLR(PWM_PIN_NODE)
#define PWM_PIN_CHANNEL             DT_PWMS_CHANNEL(PWM_PIN_NODE)
#define PWM_PIN_FLAGS               FLAGS_OR_ZERO(PWM_PIN_NODE)
#else
#error "Choose supported PWM driver"
#endif


#define PRESENCE_NODE	DT_ALIAS(presence_sensor)
#if !DT_NODE_HAS_STATUS(PRESENCE_NODE, okay)
#error "Unsupported board: presence_sensor devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(PRESENCE_NODE, gpios, {0});
static struct gpio_callback button_cb_data;


/* Led PWM period, calculated for 50 Hz signal - in microseconds. */
#define LED_PWM_PERIOD_US               (USEC_PER_SEC / 25000U)

// #ifndef ZB_ROUTER_ROLE
// #error Define ZB_ROUTER_ROLE to compile router source code.
// #endif

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);


/* Main application customizable context.
 * Stores all settings and static values.
 */
typedef struct {
    zb_zcl_basic_attrs_ext_t         basic_attr;
    zb_zcl_identify_attrs_t          identify_attr;
    zb_zcl_scenes_attrs_t            scenes_attr;
    zb_zcl_groups_attrs_t            groups_attr;
    zb_zcl_on_off_attrs_t            on_off_attr;
    zb_zcl_level_control_attrs_t     level_control_attr;

    zb_zcl_occupancy_sensing_attrs_t occupancy_sensing_attr;

    zb_zcl_on_off_switch_configuration_attrs_t     switch_on_off_switch_conf_attr;
    zb_zcl_scenes_attrs_t            switch_scenes_attr;
    zb_zcl_groups_attrs_t            switch_groups_attr;
    zb_zcl_on_off_attrs_t            switch_on_off_attr;
} bulb_device_ctx_t;

/* Zigbee device application context storage. */
static bulb_device_ctx_t dev_ctx;

/* Pointer to PWM device controlling leds with pwm signal. */
static const struct device *led_pwm_dev;
static const struct device *pin_pwm_dev;

// ZB_ZCL_DECLARE_OCCUPANCY_SENSING_ATTRIB_LIST(
//     occupancy_sensing_attr_list,
//     &dev_ctx.occupancy_sensing_attr.occupancy,
//     &dev_ctx.occupancy_sensing_attr.sensor_type,
//     &dev_ctx.occupancy_sensing_attr.bitmap
// );

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(
    identify_attr_list,
    &dev_ctx.identify_attr.identify_time);

ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(
    groups_attr_list,
    &dev_ctx.groups_attr.name_support);

ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(
    scenes_attr_list,
    &dev_ctx.scenes_attr.scene_count,
    &dev_ctx.scenes_attr.current_scene,
    &dev_ctx.scenes_attr.current_group,
    &dev_ctx.scenes_attr.scene_valid,
    &dev_ctx.scenes_attr.name_support);

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(
    basic_attr_list,
    &dev_ctx.basic_attr.zcl_version,
    &dev_ctx.basic_attr.app_version,
    &dev_ctx.basic_attr.stack_version,
    &dev_ctx.basic_attr.hw_version,
    dev_ctx.basic_attr.mf_name,
    dev_ctx.basic_attr.model_id,
    dev_ctx.basic_attr.date_code,
    &dev_ctx.basic_attr.power_source,
    dev_ctx.basic_attr.location_id,
    &dev_ctx.basic_attr.ph_env,
    dev_ctx.basic_attr.sw_ver);

// Attributes For switch

// ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(
// 	switch_client_identify_attr_list,
// 	&dev_ctx.identify_attr.identify_time);

// ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(
// 	switch_groups_attr_list,
// 	&dev_ctx.groups_attr.name_support);

// ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(
// 	switch_scenes_attr_list,
// 	&dev_ctx.scenes_attr.scene_count,
// 	&dev_ctx.scenes_attr.current_scene,
// 	&dev_ctx.scenes_attr.current_group,
// 	&dev_ctx.scenes_attr.scene_valid,
// 	&dev_ctx.scenes_attr.name_support);

// ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(
// 	switch_on_off_attr_list,
// 	&dev_ctx.on_off_attr.on_off);

ZB_ZCL_DECLARE_ON_OFF_SWITCH_CONFIGURATION_ATTRIB_LIST(
    switch_on_off_switch_conf_attr_list,
    &dev_ctx.switch_on_off_switch_conf_attr.switch_type,
    &dev_ctx.switch_on_off_switch_conf_attr.switch_actions
);


ZB_HA_DECLARE_ON_OFF_SWITCH_CLUSTER_LIST(
    on_off_switch_clusters,
    switch_on_off_switch_conf_attr_list, // on off switch config
    identify_attr_list, // identify
    basic_attr_list // basic
    // switch_on_off_attr_list, // on off
    // switch_scenes_attr_list, // scenes
    // switch_client_identify_attr_list, // identify - need to make the client version of identify (look at cluster spec for client)
    // switch_groups_attr_list // groups
    );

/* On/Off cluster attributes additions data */
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(
    on_off_attr_list,
    &dev_ctx.on_off_attr.on_off);

ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST(
    level_control_attr_list,
    &dev_ctx.level_control_attr.current_level,
    &dev_ctx.level_control_attr.remaining_time);

ZB_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(
    dimmable_light_clusters,
    basic_attr_list,
    identify_attr_list,
    groups_attr_list,
    scenes_attr_list,
    on_off_attr_list,
    level_control_attr_list);


// ZB_HA_DECLARE_OCCUPANCY_SENSING_CLUSTER_LIST(
//     occupancy_sensing_clusters,
//     basic_attr_list,
//     identify_attr_list,
//     occupancy_sensing_attr_list);


ZB_DECLARE_DIMMABLE_LIGHT_EP(
    dimmable_light_ep,
    HA_DIMMABLE_LIGHT_ENDPOINT,
    dimmable_light_clusters);

// ZB_HA_DECLARE_OCCUPANCY_SENSING_EP(
//     occupancy_sensing_ep,
//     HA_OCCUPANCY_SENSING_ENDPOINT,
//     occupancy_sensing_clusters,
//     dimmable_light_ctx);

ZB_HA_DECLARE_ON_OFF_SWITCH_EP(
    on_off_switch_ep,
    HA_ON_OFF_SWITCH_ENDPOINT,
    on_off_switch_clusters);

// ZB_HA_DECLARE_DIMMABLE_LIGHT_CTX(
// 	dimmable_light_ctx,
// 	dimmable_light_ep);

ZBOSS_DECLARE_DEVICE_CTX_2_EP(
    dimmable_light_ctx,
    dimmable_light_ep,
    on_off_switch_ep
);


/**@brief Callback for button events.
 *
 * @param[in]   button_state  Bitmask containing buttons state.
 * @param[in]   has_changed   Bitmask containing buttons
 *                            that have changed their state.
 */
static void button_changed(uint32_t button_state, uint32_t has_changed)
{
    zb_ret_t zb_err_code;

    /* Calculate bitmask of buttons that are pressed
     * and have changed their state.
     */
    uint32_t buttons = button_state & has_changed;

    if (buttons & IDENTIFY_MODE_BUTTON) {
        /* Check if endpoint is in identifying mode,
         * if not put desired endpoint in identifying mode.
         */
        if (dev_ctx.identify_attr.identify_time ==
            ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE) {
            LOG_INF("Bulb put in identifying mode");
            zb_err_code = zb_bdb_finding_binding_target(
                HA_DIMMABLE_LIGHT_ENDPOINT);
            ZB_ERROR_CHECK(zb_err_code);
        } else {
            LOG_INF("Cancel F&B target procedure");
            zb_bdb_finding_binding_target_cancel();
        }
    }
}

/**@brief Function for initializing additional PWM leds. */
static void pwm_led_init(void)
{
    led_pwm_dev = DEVICE_DT_GET(PWM_DK_LED4_CTLR);
    if (!device_is_ready(led_pwm_dev)) {
        LOG_ERR("Error: PWM device %s is not ready",
            led_pwm_dev->name);
    }
}

/**@brief Function for initializing additional PWM pin. */
static void pwm_pin_init(void)
{
    pin_pwm_dev = DEVICE_DT_GET(PWM_PIN_CTLR);
    if (!device_is_ready(pin_pwm_dev)) {
        LOG_ERR("Error: PWM device %s is not ready",
            pin_pwm_dev->name);
    }
}

/**@brief Function for initializing LEDs and Buttons. */
static void configure_gpio(void)
{
    int err;

    err = dk_buttons_init(button_changed);
    if (err) {
        LOG_ERR("Cannot init buttons (err: %d)", err);
    }

    err = dk_leds_init();
    if (err) {
        LOG_ERR("Cannot init LEDs (err: %d)", err);
    }

    pwm_led_init();
    pwm_pin_init();
}

/**@brief Sets brightness of bulb luminous executive element
 *
 * @param[in] brightness_level Brightness level, allowed values 0 ... 255,
 *                             0 - turn off, 255 - full brightness.
 */
static void light_bulb_set_brightness(zb_uint8_t brightness_level)
{
    uint32_t pulse = brightness_level * LED_PWM_PERIOD_US / 255U;

    if (pwm_pin_set_usec(led_pwm_dev, PWM_DK_LED4_CHANNEL,
                 LED_PWM_PERIOD_US, pulse, PWM_DK_LED4_FLAGS)) {
        LOG_ERR("Pwm led 4 set fails:\n");
        return;
    }
}

/**@brief Sets brightness of bulb luminous executive element
 *
 * @param[in] brightness_level Brightness level, allowed values 0 ... 255,
 *                             0 - turn off, 255 - full brightness.
 */
static void external_light_bulb_set_brightness(zb_uint8_t brightness_level)
{
    uint32_t pulse = (255U - brightness_level) * LED_PWM_PERIOD_US / 255U;

    if (pwm_pin_set_usec(pin_pwm_dev, PWM_PIN_CHANNEL,
                 LED_PWM_PERIOD_US, pulse, PWM_PIN_FLAGS)) {
        LOG_ERR("External PWM pin set fails:\n");
        return;
    }
}

/**@brief Function for setting the light bulb brightness.
 *
 * @param[in] new_level   Light bulb brightness value.
 */
static void level_control_set_value(zb_uint16_t new_level)
{
    LOG_INF("Set level value: %i", new_level);

    ZB_ZCL_SET_ATTRIBUTE(
        HA_DIMMABLE_LIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID,
        (zb_uint8_t *)&new_level,
        ZB_FALSE);

    /* According to the table 7.3 of Home Automation Profile Specification
     * v 1.2 rev 29, chapter 7.1.3.
     */
    if (new_level == 0) {
        zb_uint8_t value = ZB_FALSE;

        ZB_ZCL_SET_ATTRIBUTE(
            HA_DIMMABLE_LIGHT_ENDPOINT,
            ZB_ZCL_CLUSTER_ID_ON_OFF,
            ZB_ZCL_CLUSTER_SERVER_ROLE,
            ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
            &value,
            ZB_FALSE);
    } else {
        zb_uint8_t value = ZB_TRUE;

        ZB_ZCL_SET_ATTRIBUTE(
            HA_DIMMABLE_LIGHT_ENDPOINT,
            ZB_ZCL_CLUSTER_ID_ON_OFF,
            ZB_ZCL_CLUSTER_SERVER_ROLE,
            ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
            &value,
            ZB_FALSE);
    }

    light_bulb_set_brightness(new_level);
    external_light_bulb_set_brightness(new_level);
}

/**@brief Function for turning ON/OFF the light bulb.
 *
 * @param[in]   on   Boolean light bulb state.
 */
static void on_off_set_value(zb_bool_t on)
{
    LOG_INF("Set ON/OFF value: %i", on);

    ZB_ZCL_SET_ATTRIBUTE(
        HA_DIMMABLE_LIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ON_OFF,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
        (zb_uint8_t *)&on,
        ZB_FALSE);

    if (on) {
        level_control_set_value(
            dev_ctx.level_control_attr.current_level);
    } else {
        light_bulb_set_brightness(0U);
        external_light_bulb_set_brightness(0U);
    }
}

volatile zb_bool_t toggle_occupancy_flag = false;

void button_pressed(const struct device *dev, struct gpio_callback *cb,
            uint32_t pins)
{
    // LOG_INF("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
    toggle_occupancy_flag = true;
}

// Context for testing purposes... should be merged with other context
struct bulb_context {
    zb_uint8_t     endpoint;
    zb_uint16_t    short_addr;
    struct k_timer find_alarm;
};

static struct bulb_context bulb_ctx;

/**@brief Function for sending ON/OFF requests to the light bulb.
 *
 * @param[in]   bufid    Non-zero reference to Zigbee stack buffer that will be
 *                       used to construct on/off request.
 * @param[in]   cmd_id   ZCL command id.
 */
static void light_switch_send_on_off(zb_bufid_t bufid, zb_uint16_t cmd_id)
{
    LOG_INF("Send ON/OFF command: %d", cmd_id);

    ZB_ZCL_ON_OFF_SEND_REQ(bufid,
                   bulb_ctx.short_addr,
                   ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                   bulb_ctx.endpoint,
                   HA_ON_OFF_SWITCH_ENDPOINT,
                   ZB_AF_HA_PROFILE_ID,
                   ZB_ZCL_DISABLE_DEFAULT_RESPONSE,
                   cmd_id,
                   NULL);
}


zb_uint16_t get_on_off_cmd_id(bool isOn) {
    zb_uint16_t cmd_id;
    if (isOn)
    {
         cmd_id = ZB_ZCL_CMD_ON_OFF_ON_ID;
    }
    else
    {
           cmd_id = ZB_ZCL_CMD_ON_OFF_OFF_ID;
    }

    return cmd_id;
}

void send_on_off_cmd(bool isOn) {
    zb_ret_t zb_err_code;

    zb_uindex_t cmd_id = get_on_off_cmd_id(isOn);

    // TODO: Look into why this is "delayed"
    zb_err_code = zb_buf_get_out_delayed_ext(
        light_switch_send_on_off, cmd_id, 0);
}

void send_on_off_toggle_cmd() {
    zb_ret_t zb_err_code;

    // TODO: Look into why this is "delayed"
    zb_err_code = zb_buf_get_out_delayed_ext(
        light_switch_send_on_off, ZB_ZCL_CMD_ON_OFF_TOGGLE_ID, 0);
}

enum zb_zcl_occupancy_sensing_occupancy_e get_opposite_occupancy_state () {
    enum zb_zcl_occupancy_sensing_occupancy_e occupancy_state = dev_ctx.occupancy_sensing_attr.occupancy;

    // Toggle
    if (occupancy_state == ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_UNOCCUPIED)
    {
        occupancy_state = ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_OCCUPIED;
    }
    else
    {
        occupancy_state = ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_UNOCCUPIED;
    }

    return occupancy_state;
}

void set_occupancy_attr(enum zb_zcl_occupancy_sensing_occupancy_e occupancy_state)
{
    LOG_INF("Occupancy state: %d\n", occupancy_state);

    ZB_ZCL_SET_ATTRIBUTE(
        HA_OCCUPANCY_SENSING_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID,
        (zb_uint8_t *)&occupancy_state,
        ZB_FALSE);

    // LOG_INF("Setting LED based off occupancy");
    // on_off_set_value((zb_bool_t)occupancy_state);
}

/**@brief Function for initializing all clusters attributes.
 */
static void bulb_clusters_attr_init(void)
{
    /* Basic cluster attributes data */
    dev_ctx.basic_attr.zcl_version   = ZB_ZCL_VERSION;
    dev_ctx.basic_attr.app_version   = BULB_INIT_BASIC_APP_VERSION;
    dev_ctx.basic_attr.stack_version = BULB_INIT_BASIC_STACK_VERSION;
    dev_ctx.basic_attr.hw_version    = BULB_INIT_BASIC_HW_VERSION;

    /* Use ZB_ZCL_SET_STRING_VAL to set strings, because the first byte
     * should contain string length without trailing zero.
     *
     * For example "test" string wil be encoded as:
     *   [(0x4), 't', 'e', 's', 't']
     */
    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.mf_name,
        BULB_INIT_BASIC_MANUF_NAME,
        ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.model_id,
        BULB_INIT_BASIC_MODEL_ID,
        ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.date_code,
        BULB_INIT_BASIC_DATE_CODE,
        ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_DATE_CODE));

    dev_ctx.basic_attr.power_source = BULB_INIT_BASIC_POWER_SOURCE;

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.location_id,
        BULB_INIT_BASIC_LOCATION_DESC,
        ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_LOCATION_DESC));

    dev_ctx.basic_attr.ph_env = BULB_INIT_BASIC_PH_ENV;

    /* Identify cluster attributes data. */
    dev_ctx.identify_attr.identify_time =
        ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    /* On/Off cluster attributes data. */
    dev_ctx.on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;

    dev_ctx.level_control_attr.current_level =
        ZB_ZCL_LEVEL_CONTROL_LEVEL_MAX_VALUE;
    dev_ctx.level_control_attr.remaining_time =
        ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE;

    ZB_ZCL_SET_ATTRIBUTE(
        HA_DIMMABLE_LIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ON_OFF,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
        (zb_uint8_t *)&dev_ctx.on_off_attr.on_off,
        ZB_FALSE);

    ZB_ZCL_SET_ATTRIBUTE(
        HA_DIMMABLE_LIGHT_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID,
        (zb_uint8_t *)&dev_ctx.level_control_attr.current_level,
        ZB_FALSE);

    dev_ctx.occupancy_sensing_attr.occupancy = ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_OCCUPIED;

    // ZB_ZCL_SET_ATTRIBUTE(
    //     HA_OCCUPANCY_SENSING_ENDPOINT,
    //     ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING,
    //     ZB_ZCL_CLUSTER_SERVER_ROLE,
    //     ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID,
    //     (zb_uint8_t *)&dev_ctx.occupancy_sensing_attr.occupancy,
    //     ZB_FALSE);
}

/**@brief Callback function for handling ZCL commands.
 *
 * @param[in]   bufid   Reference to Zigbee stack buffer
 *                      used to pass received data.
 */
static void zcl_device_cb(zb_bufid_t bufid)
{
    zb_uint8_t cluster_id;
    zb_uint8_t attr_id;
    zb_zcl_device_callback_param_t  *device_cb_param =
        ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    LOG_INF("%s id %hd", __func__, device_cb_param->device_cb_id);

    /* Set default response value. */
    device_cb_param->status = RET_OK;

    switch (device_cb_param->device_cb_id) {
    case ZB_ZCL_LEVEL_CONTROL_SET_VALUE_CB_ID:
        LOG_INF("Level control setting to %d",
            device_cb_param->cb_param.level_control_set_value_param
                .new_value);
        level_control_set_value(
            device_cb_param->cb_param.level_control_set_value_param
                .new_value);
        break;

    case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
        cluster_id = device_cb_param->cb_param.
                 set_attr_value_param.cluster_id;
        attr_id = device_cb_param->cb_param.
              set_attr_value_param.attr_id;

        if (cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF) {
            uint8_t value =
                device_cb_param->cb_param.set_attr_value_param
                .values.data8;

            LOG_INF("on/off attribute setting to %hd", value);
            if (attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID) {
                on_off_set_value((zb_bool_t)value);
            }
        } else if (cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) {
            uint16_t value = device_cb_param->cb_param.
                      set_attr_value_param.values.data16;

            LOG_INF("level control attribute setting to %hd",
                value);
            if (attr_id ==
                ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID) {
                level_control_set_value(value);
            }
        } else {
            /* Other clusters can be processed here */
            LOG_INF("Unhandled cluster attribute id: %d",
                cluster_id);
        }
        break;

    default:
        if (zcl_scenes_cb(bufid) == ZB_FALSE) {
            device_cb_param->status = RET_ERROR;
        }
        break;
    }

    LOG_INF("%s status: %hd", __func__, device_cb_param->status);
}

bool status_on = true;
zb_zdo_app_signal_type_t g_app_sig;
zb_ret_t g_app_sig_status;

uint8_t get_zb_network_status_led_state(zb_zdo_app_signal_type_t sig, zb_ret_t status)
{
    uint8_t led_state = 0;

	switch (sig) {
	case ZB_BDB_SIGNAL_DEVICE_REBOOT:
		/* fall-through */
	case ZB_BDB_SIGNAL_STEERING:
		if (status == RET_OK) {
            led_state = 1;
		} else {
            led_state = 0;
		}
		break;

	case ZB_ZDO_SIGNAL_LEAVE:
		/* Update network status LED */
        led_state = 0;
		break;

	default:
		break;
	}

    // printk("app_sig: %d, app_sig_status: %d, get_zb_network_status_led_state: %d\n", g_app_sig, g_app_sig_status, led_state);
    return led_state;
}

/**@brief Zigbee stack event handler.
 *
 * @param[in]   bufid   Reference to the Zigbee stack buffer
 *                      used to pass signal.
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
    /* Update network status LED. */
    if (status_on) {
        zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);
    }

    // Store the state globally to restore LED later
	zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
	g_app_sig = zb_get_app_signal(bufid, &p_sg_p);
	g_app_sig_status = ZB_GET_APP_SIGNAL_STATUS(bufid);

    get_zb_network_status_led_state(g_app_sig, g_app_sig_status);

    /* No application-specific behavior is required.
     * Call default signal handler.
     */
    ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));

    /* All callbacks should either reuse or free passed buffers.
     * If bufid == 0, the buffer is invalid (not passed).
     */
    if (bufid) {
        zb_buf_free(bufid);
    }
}

void init_button(void) {

    int ret;
    if (!device_is_ready(button.port)) {
        printk("Error: button device %s is not ready\n",
               button.port->name);
        return;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
               ret, button.port->name, button.pin);
        return;
    }

    // ret = gpio_pin_interrupt_configure_dt(&button,
    //                       GPIO_INT_EDGE_TO_ACTIVE);
    // if (ret != 0) {
    //     printk("Error %d: failed to configure interrupt on %s pin %d\n",
    //         ret, button.port->name, button.pin);
    //     return;
    // }

    // gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    // gpio_add_callback(button.port, &button_cb_data);
    printk("Set up button at %s pin %d\n", button.port->name, button.pin);

    printk("Press the button\n");
}

bool is_button_pressed(void) {
    return gpio_pin_get(button.port, button.pin);
}

struct Button_Press_Handler {
    struct gpio_dt_spec gpio;
    int press_timer_ms;
    int debounce_timer_ms;
    // int poll_interval_ms;
    int* time_thresh;
    int completed_button_press_thresh;
    bool press_handled;
}

void main(void)
{
    int blink_status = 0;
    int err;

    LOG_INF("Starting ZBOSS Light Bulb example");

    /* Initialize */
    configure_gpio();
    err = settings_subsys_init();
    if (err) {
        LOG_ERR("settings initialization failed");
    }

    // Button interrupt - to become prescence sensor
    init_button();
    // Button end


    /* Register callback for handling ZCL commands. */
    ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

    /* Register dimmer switch device context (endpoints). */
    ZB_AF_REGISTER_DEVICE_CTX(&dimmable_light_ctx);

    bulb_clusters_attr_init();
    level_control_set_value(dev_ctx.level_control_attr.current_level);

    // Legacy support for Sonoff ZigBee Bridge running Tasmota
    zb_bdb_set_legacy_device_support(1);

    /* Initialize ZCL scene table */
    zcl_scenes_init();

    /* Settings should be loaded after zcl_scenes_init */
    err = settings_load();
    if (err) {
        LOG_ERR("settings loading failed");
    }

    // START - USB logging on dongle
#ifdef INCLUDE_DONGLE_USB_LOGGING
    usb_logging_init();
#endif
    // END - USB logging on dongle

    /* Start Zigbee default thread */
    zigbee_enable();

    LOG_INF("ZBOSS Light Bulb example started");


    int button_press_state = 0;


    bool button_is_pressed = false;
    bool debounce_is_pressed = false;
    int press_timer = 0;
    int debounce_timer = 0;


    int loop_count = 0;
    const int blink_toggle = (1000/RUN_LED_BLINK_INTERVAL);
    const uint8_t level_control_inc = UINT8_MAX/3;
    uint8_t level_control_value = 0;

    // struct Button_Press_Handler button_press_handler[1];
    // button_press_handler[0].gpio = button;
    // button_press_handler[0].press_timer_ms = 0;
    // button_press_handler[0].debounce_timer_ms = 0;
    // button_press_handler[0].time_thresh = 0;
    // button_press_handler[0].completed_button_press_thresh = 0;


    while (1) {

        // Status LED
        if (status_on) {
            dk_set_led(RUN_STATUS_LED, (blink_status) % 2);
            if (loop_count % blink_toggle == 0) {
                blink_status++;
            }
            loop_count++;
        }

        k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));

        // Button press + debounce + handlers
        button_is_pressed = is_button_pressed();

        if (button_is_pressed != debounce_is_pressed) {
            // Increment when there is a difference
            debounce_timer += RUN_LED_BLINK_INTERVAL;
        }

        if (button_is_pressed == debounce_is_pressed) {
            // Reset timer
            debounce_timer = 0;
        }

        if (debounce_is_pressed) {
            press_timer += RUN_LED_BLINK_INTERVAL;
        }

        if (debounce_timer > 100) { // > 100ms
            // debounce time surpassed, debounced state change
            debounce_is_pressed = button_is_pressed;
            LOG_INF("Debounced");

            if (button_is_pressed == false) {
                // Rising edge/finger lifted
                if (press_timer > 5000) {
                    // action for 5 second press
                    LOG_INF("5 second button press");
                    LOG_INF("Resetting zigbee network config");
                    zb_bdb_reset_via_local_action(0);
                } else if (press_timer > 1000) { // > 1000ms
                    // action for 1 second press
                    LOG_INF("1 second button press");

                    // Turn everything off
                    status_on = !status_on;
                    if (status_on) {
                        // Let other handlers turn the LEDs back on
                        dk_set_led(ZIGBEE_NETWORK_STATE_LED, get_zb_network_status_led_state(g_app_sig, g_app_sig_status));
                    } else {
                        dk_set_led(RUN_STATUS_LED, 0);
                        dk_set_led(ZIGBEE_NETWORK_STATE_LED, 0);
                        // light_bulb_set_brightness(0);
                    }

                } else {
                    // action for momentary press
                    LOG_INF("Momentary button press");

                    level_control_value = ((int)(dev_ctx.level_control_attr.current_level/level_control_inc)) * level_control_inc;

                    if (level_control_value == UINT8_MAX) {
                        level_control_value = 0;
                    } else {
                        level_control_value += level_control_inc;
                    }
                    level_control_set_value(level_control_value);
                }

                press_timer = 0;
            }
        }


        // if (toggle_occupancy_flag)
        // {
        //     dk_set_led(DK_LED2, (++button_press_state) % 2);

        //     enum zb_zcl_occupancy_sensing_occupancy_e occupancy_state = get_opposite_occupancy_state();
        //     bool buttonCmdState = occupancy_state == ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_OCCUPIED;

        //     // send_on_off_cmd(buttonCmdState);
        //     send_on_off_toggle_cmd();
        //     set_occupancy_attr(occupancy_state);
        //     toggle_occupancy_flag = false;

        //     zb_bdb_reset_via_local_action(0);
        // }
    }

}
