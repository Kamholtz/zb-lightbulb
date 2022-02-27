#ifndef ZB_ZCL_OCCUPANCY_SENSING_ADDONS_H
#define ZB_ZCL_OCCUPANCY_SENSING_ADDONS_H 1

#include <zb_zcl_occupancy_sensing.h>

typedef struct
{
    enum zb_zcl_occupancy_sensing_occupancy_e  occupancy;
    enum zb_zcl_occupancy_sensing_occupancy_sensor_type_e  sensor_type;
    uint16_t bitmap;
} zb_zcl_occupancy_sensing_attrs_t;

#endif /* ZB_ZCL_OCCUPANCY_SENSING_ADDONS_H */