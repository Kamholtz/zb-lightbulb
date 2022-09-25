#ifndef ZBOSS_API_AF_EXT_H
#define ZBOSS_API_AF_EXT_H 1

/**
  Declare application's device context for three-endpoint device

  @note Device has an additional Green Power endpoint if it is ZC or ZR
  and GPPB feature (Mandatory for Zigbee 3.0 ZC/ZR) is enabled.

  @param device_ctx_name - device context variable name
  @param ep1_name - variable holding context for endpoint 1
  @param ep2_name - variable holding context for endpoint 2
  @param ep3_name - variable holding context for endpoint 3
  @param ep4_name - variable holding context for endpoint 4
  @param ep5_name - variable holding context for endpoint 5
*/
#define ZBOSS_DECLARE_DEVICE_CTX_5_EP(device_ctx_name, ep1_name, ep2_name, ep3_name, ep4_name, ep5_name) \
  ZB_AF_START_DECLARE_ENDPOINT_LIST(ep_list_##device_ctx_name)          \
    &ep1_name,                                                          \
    &ep2_name,                                                          \
    &ep3_name,                                                          \
    &ep4_name,                                                          \
    &ep5_name,                                                          \
  ZB_AF_FINISH_DECLARE_ENDPOINT_LIST;                                   \
  ZBOSS_DECLARE_DEVICE_CTX(device_ctx_name, ep_list_##device_ctx_name,  \
                           (ZB_ZCL_ARRAY_SIZE(ep_list_##device_ctx_name, zb_af_endpoint_desc_t*)))

#endif /* ZBOSS_API_AF_EXT_H */