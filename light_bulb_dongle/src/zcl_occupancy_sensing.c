// #include "zb_common.h"

// #if defined (ZB_ZCL_SUPPORT_CLUSTER_OCCUPANCY_SENSING)

// #include "zb_zcl.h"
// #include "zb_aps.h"
// #include "/zboss/src/include/zb_zcl.h"
// #include "/zboss/src/include/zb_aps.h"
#include <zboss_api.h>
#include <zb_zcl_occupancy_sensing.h>
#include "zcl/zb_zcl_common.h"

zb_ret_t check_value_occupancy_sensing_server(zb_uint16_t attr_id, zb_uint8_t endpoint, zb_uint8_t *value);
void zb_zcl_occupancy_sensing_write_attr_hook_server(zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *new_value);

void zb_zcl_occupancy_sensing_init_server()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING,
                              ZB_ZCL_CLUSTER_SERVER_ROLE,
                              check_value_occupancy_sensing_server,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);
}

void zb_zcl_occupancy_sensing_init_client()
{
  zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING,
                              ZB_ZCL_CLUSTER_CLIENT_ROLE,
                              (zb_zcl_cluster_check_value_t)NULL,
                              (zb_zcl_cluster_write_attr_hook_t)NULL,
                              (zb_zcl_cluster_handler_t)NULL);
}

zb_ret_t check_value_occupancy_sensing_server(zb_uint16_t attr_id, zb_uint8_t endpoint, zb_uint8_t *value)
{
  zb_ret_t ret = RET_OK;
  zb_int16_t val = ZB_ZCL_ATTR_GETS16(value);

  TRACE_MSG(TRACE_ZCL1, "> check_value_occupancy_sensing, attr_id %d, val %d",
      (FMT__D_D, attr_id, val));

  ret = RET_OK;
//   switch( attr_id )
//   {
//     case ZB_ZCL_ATTR_OCCUPANCY_SENSING_VALUE_ID:
//       if( ZB_ZCL_ATTR_OCCUPANCY_SENSING_VALUE_UNKNOWN == val )
//       {
//         ret = RET_OK;
//       }
//       else
//       {
//         zb_zcl_attr_t *attr_desc = zb_zcl_get_attr_desc_a(endpoint,
//         ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING, ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_OCCUPANCY_SENSING_MIN_VALUE_ID);
//         ZB_ASSERT(attr_desc);
//
//         ret = (ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc) == ZB_ZCL_ATTR_OCCUPANCY_SENSING_MIN_VALUE_INVALID ||
//                ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc) <= val)
//               ? RET_OK : RET_ERROR;
//
//         if(ret)
//         {
//           attr_desc = zb_zcl_get_attr_desc_a(endpoint,
//           ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING, ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_OCCUPANCY_SENSING_MAX_VALUE_ID);
//           ZB_ASSERT(attr_desc);
//
//           ret = ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc) == ZB_ZCL_ATTR_OCCUPANCY_SENSING_MAX_VALUE_INVALID ||
//                 val <= ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc)
//             ? RET_OK : RET_ERROR;
//         }
//       }
//       break;
//
//     case ZB_ZCL_ATTR_OCCUPANCY_SENSING_MIN_VALUE_ID:
//       ret = ( (ZB_ZCL_ATTR_OCCUPANCY_SENSING_MIN_VALUE_MIN_VALUE <= val) &&
//               (val <= ZB_ZCL_ATTR_OCCUPANCY_SENSING_MIN_VALUE_MAX_VALUE) ) ||
//             (ZB_ZCL_ATTR_OCCUPANCY_SENSING_MIN_VALUE_INVALID == val)
//               ? RET_OK : RET_ERROR;
//       break;
//
//     case ZB_ZCL_ATTR_OCCUPANCY_SENSING_MAX_VALUE_ID:
//       ret = ((ZB_ZCL_ATTR_OCCUPANCY_SENSING_MAX_VALUE_MIN_VALUE <= val
// #if ZB_ZCL_ATTR_OCCUPANCY_SENSING_MAX_VALUE_MAX_VALUE != 0x7fff
//               && val <= ZB_ZCL_ATTR_OCCUPANCY_SENSING_MAX_VALUE_MAX_VALUE
// #endif
//           ) || (ZB_ZCL_ATTR_OCCUPANCY_SENSING_MAX_VALUE_INVALID == val))
//               ? RET_OK : RET_ERROR;
//       break;
//
//     /* TODO: case ZB_ZCL_ATTR_OCCUPANCY_SENSING_TOLERANCE_ID */
//
//     default:
//       break;
//   }

  TRACE_MSG(TRACE_ZCL1, "< check_value_occupancy_sensing ret %hd", (FMT__H, ret));
  return ret;
}

void zb_zcl_occupancy_sensing_write_attr_hook_server(
  zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *new_value)
{
	ZVUNUSED(new_value);
  ZVUNUSED(endpoint);

  TRACE_MSG(TRACE_ZCL1, ">> zb_zcl_occupancy_sensing_write_attr_hook endpoint %hd, attr_id %d",
            (FMT__H_D, endpoint, attr_id));

  if (attr_id == ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID)
  {
	  /* TODO Change min/max temperature by current are not agree
	   * spec/
	   * Need consult with customer !*/
  }

  TRACE_MSG(TRACE_ZCL1, "<< zb_zcl_occupancy_sensing_write_attr_hook", (FMT__0));
}

// #endif /* ZB_ZCL_SUPPORT_CLUSTER_OCCUPANCY_SENSING */
