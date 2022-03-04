
#ifndef ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_ADDONS_H
#define ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_ADDONS_H 1

#include <zb_zcl_on_off_switch_conf.h>

typedef struct
{
    enum zb_zcl_on_off_switch_configuration_switch_type_e switch_type;
    enum zb_zcl_on_off_switch_configuration_switch_actions_e switch_actions;
} zb_zcl_on_off_switch_configuration_attrs_t;

#endif /* ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_ADDONS_H */