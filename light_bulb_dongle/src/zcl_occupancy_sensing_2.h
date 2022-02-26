/* PURPOSE: Occupancy Sensing cluster definitions
*/

#ifndef ZB_ZCL_OCCUPANCY_SENSING_2_H
#define ZB_ZCL_OCCUPANCY_SENSING_2_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/*! @} */ /* Occupancy Sensing cluster commands */

/*! @} */ /* ZCL Occupancy Sensing cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_occupancy_sensing_init_server(void);
void zb_zcl_occupancy_sensing_init_client(void);

// #ifdef ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_SERVER_ROLE_INIT
#undef ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_SERVER_ROLE_INIT
// #endif

// #ifdef ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_CLIENT_ROLE_INIT
#undef ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_CLIENT_ROLE_INIT
// #endif

#define ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_SERVER_ROLE_INIT zb_zcl_occupancy_sensing_init_server
#define ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_CLIENT_ROLE_INIT zb_zcl_occupancy_sensing_init_client


#endif /* ZB_ZCL_OCCUPANCY_SENSING_2_H */
