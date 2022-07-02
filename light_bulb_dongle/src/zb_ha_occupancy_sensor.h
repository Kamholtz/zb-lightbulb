#ifndef ZB_ZCL_OCCUPANCY_SENSOR_H
#define ZB_ZCL_OCCUPANCY_SENSOR_H 1

#include <zb_zcl_occupancy_sensing.h>

// Declaring a cluster list, which includes declaring multiple clusters
// TODO: This probably shouldn't be here
#define ZB_HA_DECLARE_OCCUPANCY_SENSING_CLUSTER_LIST(            \
  cluster_list_name,                                             \
  basic_attr_list,                                               \
  identify_attr_list,                                            \
  occupancy_sensing_attr_list)                                   \
  zb_zcl_cluster_desc_t cluster_list_name[] =                    \
  {                                                              \
    ZB_ZCL_CLUSTER_DESC(                                         \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),      \
      (identify_attr_list),                                      \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
      ZB_ZCL_MANUF_CODE_INVALID                                  \
    ),                                                           \
    ZB_ZCL_CLUSTER_DESC(                                         \
      ZB_ZCL_CLUSTER_ID_BASIC,                                   \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),         \
      (basic_attr_list),                                         \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
      ZB_ZCL_MANUF_CODE_INVALID                                  \
    ),                                                           \
    ZB_ZCL_CLUSTER_DESC(                                         \
      ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING,                       \
      ZB_ZCL_ARRAY_SIZE(occupancy_sensing_attr_list, zb_zcl_attr_t),     \
      (occupancy_sensing_attr_list),                             \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
      ZB_ZCL_MANUF_CODE_INVALID                                  \
    )                                                            \
  }

/** @cond internals_doc */

#define ZB_HA_OCCUPANCY_SENSING_IN_CLUSTER_NUM 3  /*!< Occupancy Sensing IN (server) clusters number */
#define ZB_HA_OCCUPANCY_SENSING_OUT_CLUSTER_NUM 0 /*!< Occupancy Sensing OUT (client) clusters number */

/** Occupancy sensing total (IN+OUT) cluster number */
#define ZB_HA_OCCUPANCY_SENSING_CLUSTER_NUM                                \
  (ZB_HA_OCCUPANCY_SENSING_IN_CLUSTER_NUM +  ZB_HA_OCCUPANCY_SENSING_OUT_CLUSTER_NUM)

/*! Number of attribute for reporting on Occupancy Sensing device */
#define ZB_HA_OCCUPANCY_SENSING_REPORT_ATTR_COUNT         \
  (ZB_ZCL_OCCUPANCY_SENSING_REPORT_ATTR_COUNT)

/** @endcond */


// https://www.nxp.com/docs/en/user-guide/JN-UG-3076.pdf
#define ZB_HA_OCCUPANCY_SENSING_DEVICE_ID  0x107
#define ZB_HA_DEVICE_VER_OCCUPANCY_SENSING 1

/*! @cond internals_doc */
/*!
  @brief Declare simple descriptor for Occupancy Sensing device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_HA_OCCUPANCY_SENSING_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                  \
  {                                                                                            \
    ep_id,                                                                                     \
    ZB_AF_HA_PROFILE_ID,                                                                       \
    ZB_HA_OCCUPANCY_SENSING_DEVICE_ID,                                                            \
    ZB_HA_DEVICE_VER_OCCUPANCY_SENSING,                                                           \
    0,                                                                                         \
    in_clust_num,                                                                              \
    out_clust_num,                                                                             \
    {                                                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                 \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                              \
      ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING,                                                     \
    }                                                                                          \
  }


/*!
  @brief Declare endpoint for Occupancy Sensing device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
  @param device_ctx_name - device context name (to avoid name collisions)
 */
/* TODO: add scenes? */
#define ZB_HA_DECLARE_OCCUPANCY_SENSING_EP(ep_name, ep_id, cluster_list, device_ctx_name)            \
  ZB_ZCL_DECLARE_HA_OCCUPANCY_SENSING_SIMPLE_DESC(ep_name, ep_id,                      \
      ZB_HA_OCCUPANCY_SENSING_IN_CLUSTER_NUM, ZB_HA_OCCUPANCY_SENSING_OUT_CLUSTER_NUM); \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,          \
                                     ZB_HA_OCCUPANCY_SENSING_REPORT_ATTR_COUNT);    \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID, 0, NULL, \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,       \
                           (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,    \
                           ZB_HA_OCCUPANCY_SENSING_REPORT_ATTR_COUNT,               \
                           reporting_info## device_ctx_name, 0, NULL)



#endif /* ZB_ZCL_OCCUPANCY_SENSOR_H */


