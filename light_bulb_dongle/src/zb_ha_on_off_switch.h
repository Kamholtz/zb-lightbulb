/*
 * ZBOSS Zigbee 3.0
 *
 * Copyright (c) 2012-2021 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 *
 * Use in source and binary forms, redistribution in binary form only, with
 * or without modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 2. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 3. This software, with or without modification, must only be used with a Nordic
 *    Semiconductor ASA integrated circuit.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* PURPOSE: On/off switch device definition
*/
#ifndef ZB_HA_ON_OFF_SWITCH_H
#define ZB_HA_ON_OFF_SWITCH_H 1

#if 1 || defined DOXYGEN

/** @cond DOXYGEN_HA_SECTION */

/**
 *  @defgroup ha_on_off_switch On/Off Switch
 *  @ingroup ZB_HA_DEVICES
    @{
    @details
    On/Off Switch device has 6 clusters (see spec 7.4.1): \n
        - @ref ZB_ZCL_IDENTIFY \n
        - @ref ZB_ZCL_BASIC \n
        - @ref ZB_ZCL_OOSC \n
        - @ref ZB_ZCL_ON_OFF \n
        - @ref ZB_ZCL_SCENES \n
        - @ref ZB_ZCL_GROUPS

    On/Off Switch device sample

    @par Example

    - Declaring clusters:

        @snippet HA_samples/common/zcl_basic_attr_list.h BASIC_CLUSTER_COMMON
        @snippet HA_samples/on_off_switch/sample_zc.c COMMON_DECLARATION

    - Registering device list:

        @snippet HA_samples/on_off_switch/sample_zc.c REGISTER

    - Example of command handler:

        @snippet HA_samples/on_off_switch/sample_zc.c COMMAND_HANDLER
        @snippet HA_samples/on_off_switch/sample_zc.c ZCL_COMMAND_HANDLER

    @par
*/

/** @cond internals_doc */
#define ZB_HA_DEVICE_VER_ON_OFF_SWITCH 0  /*!< On/Off Switch device version */

#define ZB_HA_ON_OFF_SWITCH_IN_CLUSTER_NUM 3  /*!< On/Off switch IN clusters number */
#define ZB_HA_ON_OFF_SWITCH_OUT_CLUSTER_NUM 4 /*!< On/Off switch OUT clusters number */

#define ZB_HA_ON_OFF_SWITCH_CLUSTER_NUM                                      \
  (ZB_HA_ON_OFF_SWITCH_IN_CLUSTER_NUM + ZB_HA_ON_OFF_SWITCH_OUT_CLUSTER_NUM)

/*! Number of attribute for reporting on On/Off switch device */
#define ZB_HA_ON_OFF_SWITCH_REPORT_ATTR_COUNT \
  (ZB_ZCL_ON_OFF_SWITCH_CONFIG_REPORT_ATTR_COUNT)

/** @endcond */

/** @brief Declare cluster list for On/Off switch device
    @param cluster_list_name - cluster list variable name
    @param on_off_switch_config_attr_list - attribute list for On/off switch configuration cluster
    @param basic_attr_list - attribute list for Basic cluster
    @param identify_attr_list - attribute list for Identify cluster
 */
#define ZB_HA_DECLARE_ON_OFF_SWITCH_CLUSTER_LIST(                           \
      cluster_list_name,                                                    \
      on_off_switch_config_attr_list,                                       \
      basic_attr_list,                                                      \
      identify_attr_list)                                                   \
      zb_zcl_cluster_desc_t cluster_list_name[] =                           \
      {                                                                     \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,                           \
          ZB_ZCL_ARRAY_SIZE(on_off_switch_config_attr_list, zb_zcl_attr_t), \
          (on_off_switch_config_attr_list),                                 \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
          ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),             \
          (identify_attr_list),                                             \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_BASIC,                                          \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                \
          (basic_attr_list),                                                \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_SCENES,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                            \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                              \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_GROUPS,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        )                                                                   \
    }


#define __CAT_VA__(a, b, ...)                               a## __VA_ARGS__## b

#if 0

// The following is here only for reference to create multiple endpoints of the same type (on off switch)

/**@brief Redefinition of ZB_AF_SIMPLE_DESC_TYPE as variadic macro.
 *
 * @param[in]  in_num   Number of input clusters.
 * @param[in]  out_num  Number of output clusters.
 * @param[in]  ...      Optional argument to concatenate to the type name.
 */
#define ZB_AF_SIMPLE_DESC_TYPE_VA(in_num, out_num, ...)     __CAT_VA__(zb_af_simple_desc_, _t, __VA_ARGS__)

/**@brief Redefinition of ZB_DECLARE_SIMPLE_DESC as variadic macro.
 *
 * @param[in]  in_clusters_count   Number of input clusters.
 * @param[in]  out_clusters_count  Number of output clusters.
 * @param[in]  ...                 Optional argument to concatenate to the type name.
 */
#define ZB_DECLARE_SIMPLE_DESC_VA(in_clusters_count, out_clusters_count, ...)             \
  typedef ZB_PACKED_PRE struct zb_af_simple_desc_## __VA_ARGS__## _s                      \
  {                                                                                       \
    zb_uint8_t    endpoint;                 /* Endpoint */                                \
    zb_uint16_t   app_profile_id;           /* Application profile identifier */          \
    zb_uint16_t   app_device_id;            /* Application device identifier */           \
    zb_bitfield_t app_device_version:4;     /* Application device version */              \
    zb_bitfield_t reserved:4;               /* Reserved */                                \
    zb_uint8_t    app_input_cluster_count;  /* Application input cluster count */         \
    zb_uint8_t    app_output_cluster_count; /* Application output cluster count */        \
    /* Application input and output cluster list */                                       \
    zb_uint16_t   app_cluster_list[(in_clusters_count) + (out_clusters_count)];           \
    zb_uint8_t    cluster_encryption[((in_clusters_count) + (out_clusters_count) + 7)/8]; \
  } ZB_PACKED_STRUCT zb_af_simple_desc_## __VA_ARGS__## _t


#endif

/** @cond DOXYGEN_INTERNAL_DOC */
#define CAT5(a, b, c, d, e) a##b##c##d##e
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/** Generate simple descriptor type name */
// #define ZB_AF_SIMPLE_DESC_TYPE_VA(in_num, out_num, ...)  CAT5(zb_af_simple_desc_,in_num,_,out_num,_t)
#define ZB_AF_SIMPLE_DESC_TYPE_VA(in_num, out_num, ...)  __CAT_VA__(zb_af_simple_desc_,_t, __VA_ARGS__)

// ZB_AF_SIMPLE_DESC_TYPE_VA(1,2,aaa)

/**
   Declares Simple descriptor type

   @param in_clusters_count - number of input clusters in descriptor
   @param out_clusters_count - number of output clusters in descriptor

   @b Example:
   @code
     ZB_DECLARE_SIMPLE_DESC(5, 5);
   @endcode
 */

#define ZB_DECLARE_SIMPLE_DESC_VA(in_clusters_count, out_clusters_count, ...)   \
  typedef ZB_PACKED_PRE struct zb_af_simple_desc_## __VA_ARGS__## _s \
  {                                                                                       \
    zb_uint8_t    endpoint;                 /* Endpoint */                                \
    zb_uint16_t   app_profile_id;           /* Application profile identifier */          \
    zb_uint16_t   app_device_id;            /* Application device identifier */           \
    zb_bitfield_t app_device_version:4;     /* Application device version */              \
    zb_bitfield_t reserved:4;               /* Reserved */                                \
    zb_uint8_t    app_input_cluster_count;  /* Application input cluster count */         \
    zb_uint8_t    app_output_cluster_count; /* Application output cluster count */        \
    /* Application input and output cluster list */                                       \
    zb_uint16_t   app_cluster_list[(in_clusters_count) + (out_clusters_count)];               \
  } ZB_PACKED_STRUCT                                                                      \
  zb_af_simple_desc_## __VA_ARGS__## _t


/** @cond internals_doc */
/** @brief Declare simple descriptor for On/Off switch device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param in_clust_num - number of supported input clusters
    @param out_clust_num - number of supported output clusters
    @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
    definitions, because these values are used to form simple descriptor type name
*/
#define ZB_ZCL_DECLARE_ON_OFF_SWITCH_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                                        \
  ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name) simple_desc_##ep_name =                 \
  {                                                                                           \
    ep_id,                                                                                    \
    ZB_AF_HA_PROFILE_ID,                                                                      \
    ZB_HA_ON_OFF_SWITCH_DEVICE_ID,                                                            \
    ZB_HA_DEVICE_VER_ON_OFF_SWITCH,                                                           \
    0,                                                                                        \
    in_clust_num,                                                                             \
    out_clust_num,                                                                            \
    {                                                                                         \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                             \
      ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,                                                 \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                               \
      ZB_ZCL_CLUSTER_ID_SCENES,                                                               \
      ZB_ZCL_CLUSTER_ID_GROUPS,                                         \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
    }                                                                                         \
  }

/** @endcond */

/** @brief Declare endpoint for On/off Switch device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param cluster_list - endpoint cluster list
 */
#define ZB_HA_DECLARE_ON_OFF_SWITCH_EP(ep_name, ep_id, cluster_list) \
  ZB_ZCL_DECLARE_ON_OFF_SWITCH_SIMPLE_DESC(                          \
      ep_name,                                                       \
      ep_id,                                                         \
      ZB_HA_ON_OFF_SWITCH_IN_CLUSTER_NUM,                            \
      ZB_HA_ON_OFF_SWITCH_OUT_CLUSTER_NUM);                          \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                  \
                              ep_id,                                    \
      ZB_AF_HA_PROFILE_ID,                                           \
      0,                                                             \
      NULL,                                                          \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),        \
      cluster_list,                                                  \
      (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name, \
      0, NULL, /* No reporting ctx */           \
      0, NULL) /* No CVC ctx */

/** @brief Declare On/Off Switch device context.
    @param device_ctx - device context variable name.
    @param ep_name - endpoint variable name.
*/
#define ZB_HA_DECLARE_ON_OFF_SWITCH_CTX(device_ctx, ep_name) \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)


/*! @} */

/** @endcond */ /* DOXYGEN_HA_SECTION */

#endif /* ZB_HA_DEFINE_DEVICE_ON_OFF_SWITCH */

#endif /* ZB_HA_ON_OFF_SWITCH_H */
