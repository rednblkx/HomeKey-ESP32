/*
 * Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA
 * Copyright (c) 2021-2022 Espressif Systems (Shanghai) PTE LTD
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Espressif Systems
 *    integrated circuit in a product or a software update for such product,
 *    must reproduce the above copyright notice, this list of conditions and
 *    the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/* PURPOSE: ZGPD context and macros to configure it
*/

#ifndef ZB_ZGPD_H
#define ZB_ZGPD_H 1

#include "zgp/zgp_internal.h"

#ifdef ZB_ZGPD_TEST_MODE
#include "zb_zgpd_test.h"
#else
#define ZGPD_FN(func_name) func_name
#endif

/** @addtogroup zgp_zgpd */
/** @{ */
/********************************************************************/
/************* Type declarations specific to ZGPD *******************/
/********************************************************************/

/**
 * @brief ZGPD commissioning method
 * @see ZGP spec, A. 3.9
 */
typedef enum zb_zgpd_commissioning_method_e
{
  ZB_ZGPD_COMMISSIONING_BIDIR,         /**< Bidirectional commissioning */
  ZB_ZGPD_COMMISSIONING_UNIDIR,        /**< Unidirectional commissioning */
  ZB_ZGPD_COMMISSIONING_AUTO           /**< Auto-commissioning */
}
zb_zgpd_commissioning_method_t;


typedef enum zb_zgpd_comm_result_e
{
  ZB_ZGPD_COMM_SUCCESS,     /**< Commissioning is successful */
  ZB_ZGPD_COMM_FAILED,      /**< Commissioning failed */
}
zb_zgpd_comm_result_t;


typedef enum zb_zgpd_comm_state_e
{
  ZB_ZGPD_STATE_NOT_COMMISSIONED,     /**< ZGPD is not commissioned */
  ZB_ZGPD_STATE_COMM_IN_PROGRESS,     /**< Commissioning is in progress */
  ZB_ZGPD_STATE_COMM_SENT_SUCCESS,    /**< Sent success */
  ZB_ZGPD_STATE_COMMISSIONED,         /**< ZGPD has successfully commissioned */
}
zb_zgpd_comm_state_t;

#define ZGPD_COMMISSIONING_IN_PROGRESS() \
  ((ZGPD->comm_state == ZB_ZGPD_STATE_COMM_IN_PROGRESS)||(ZGPD->comm_state == ZB_ZGPD_STATE_COMM_SENT_SUCCESS))

#define ZGPD_IS_COMMISSIONED() \
  (ZGPD->comm_state == ZB_ZGPD_STATE_COMMISSIONED)

/********************************************************************/
/****** Global ZGPD context and macros for changing it  *************/
/********************************************************************/
#define ZB_ZGPD_CHACK_GET_LO(option) ((ZGPD->options_to_modify)&(1<<option))
#define ZB_ZGPD_CHACK_SET_LO(option) ((ZGPD->options_to_modify)|=(1<<option))
#define ZB_ZGPD_CHACK_GET_HI(option) ((ZGPD->options_to_modify)&(1<<option))
#define ZB_ZGPD_CHACK_SET_HI(option) ((ZGPD->options_to_modify)|=(1<<option))

#define ZB_ZGPD_CHACK_GET(option) ((option)>31?((ZGPD->options_to_modify_hi)&(1<<((option)-32))):((ZGPD->options_to_modify_lo)&(1<<(option))))
#define ZB_ZGPD_CHACK_SET(option) ((option)>31?((ZGPD->options_to_modify_hi)|=(1<<((option)-32))):((ZGPD->options_to_modify_lo)|=(1<<(option))))

#define ZB_ZGPD_CHACK_RESET_ALL() {ZGPD->options_to_modify_lo = 0;ZGPD->options_to_modify_hi = 0;}

typedef enum zb_zgpd_ch_options_e
{
  ZB_ZGPD_CH_REPLACE_KEY            =0,  // wrong key
  ZB_ZGPD_CH_REPLACE_KEY_TYPE       =1,  // wrong KeyType // set keytype to individual to securitykey 1
  ZB_ZGPD_CH_REPLACE_SEC_LEVEL      =2,  // wrong security Level // changes only in data GPDF
  ZB_ZGPD_CH_REPLACE_FRAME_COUNTER  =3,  // wrong frame counter
  ZB_ZGPD_CH_REPLACE_FRAME_TYPE     =4,  // wrong frame type
  ZB_ZGPD_CH_REPLACE_PROTO_VERSION  =5,  // wrong protocol version
  ZB_ZGPD_CH_REPLACE_RXAFTERTX      =6,  // wrong txafterrx
  ZB_ZGPD_CH_REPLACE_AUTO_COMM      =7,  // malformed frame
  ZB_ZGPD_CH_REPLACE_NWK_FC         =8,  // NWK frame control field value
  ZB_ZGPD_CH_REPLACE_APPID          =9,  // wrong appID
  ZB_ZGPD_CH_REPLACE_DIRECTION      =10, // wrong Direction
  ZB_ZGPD_CH_REPLACE_SRCID          =11, // wrong SrcID
  ZB_ZGPD_CH_INCR_SFC               =12, // increment security frame counter
  ZB_ZGPD_CH_REPLACE_EXTNWK_FC_FLAG =13, // NWK ext frame control field
  ZB_ZGPD_CH_INSERT_EXTNWK_FC_DATA  =14, // NWK ext frame control field
  ZB_ZGPD_CH_INSERT_FC              =15, // insert frame counter field
  ZB_ZGPD_CH_UNPROTECT_PACKET       =16, // send unprotect packet with correct sec_lvl
  ZB_ZGPD_CH_MAX_PAYLOAD            =17, // send GPDF with max payload for current APP
  ZB_ZGPD_CH_MAX_COMM_PAYLOAD       =18, // send commissioning GPDF with max payload for current APP
  ZB_ZGPD_CH_REPLACE_SKP            =19, // replace security_key_present in commissioning ext options
  ZB_ZGPD_CH_INSERT_SK              =20, // insert security key field in commissioning frame 0 - remove, other - insert
  ZB_ZGPD_CH_REPLACE_SKE            =21, // replace security_key_encryption in commissioning ext options
  ZB_ZGPD_CH_INSERT_EP              =22, // insert endpoint in packet (default for app_id 010)
  ZB_ZGPD_CH_INSERT_COMM_EXTRA_PLD  =23, // insert additional bytes in commissioning packet
  ZB_ZGPD_CH_COMM_OPT_RESERVED_SET  =24, // set reserved bit of the commissioning options to 0b1
  ZB_ZGPD_CH_MISSING_IEEE           =25, // missing IEEE addr
  ZB_ZGPD_CH_EXIT_AFTER_CH_REQ      =26, // exit immediately after sending channel request
  ZB_ZGPD_CH_EXIT_AFTER_REQ_SERIES  =27, // exit after channel request one series completed
  //ZB_ZGPD_CH_SKIP_CH_INC          =28, // skip channel incrementing in channel request command send
  ZB_ZGPD_CH_REPLACE_CR_NEXT_CH     =28, // replace next channel in channel request
  ZB_ZGPD_CH_REPLACE_CR_TRANSMIT_CH =29, // replace channel request transmit channel
  ZB_ZGPD_CH_REPLACE_CR_START_CH    =30, // replace channel request start channel
  ZB_ZGPD_CH_SKIP_FIRST_COMM_REPLY  =31, // skip first commissioning reply from sink/proxy
  ZB_ZGPD_CH_REPLACE_CMD            =32, // replace command in buffer before send
  ZB_ZGPD_CH_STOP_AFTER_1_COMMREQ   =33, // stop bidir commissioning process after 1 comm command request sent
  ZB_ZGPD_CH_USE_CH_SEC_LEVEL       =34, // use sec_level from channel hacks (for encryption of the packet)
  ZB_ZGPD_CH_UNPROTECT_SUCCESS      =35, // send success cmd unprotected
  ZB_ZGPD_CH_DO_NOT_SEND_SUCCESS    =36, // do not send success, simply exit
  ZB_ZGPD_CH_CORRUPT_IEEE_ADDR      =37, // IEEE address different by 1 bit
  ZB_ZGPD_CH_BZERO_IEEE_ADDR        =38, // set IEEE address to 0
  ZB_ZGPD_CH_REPLACE_COMM_SEC_LEVEL =39, // replace security level in commissioning extended options
  ZB_ZGPD_CH_CORRUPT_COMM_MIC       =40  // corrupt MIC in commissioning command
}
zb_zgpd_ch_options_t;

#ifdef ZB_CERTIFICATION_HACKS
  void zgpd_send_success_cmd_delayed(zb_uint8_t);
#endif

#define ZGPD_COMM_MS_OPT_FILL(manuf_prsnt, model_prsnt, gpd_cmds_prsnt, clsts_list_prsnt)\
  ((!!(manuf_prsnt)) | ((!!(model_prsnt)) << 1) | ((!!(gpd_cmds_prsnt)) << 2) | ((!!(clsts_list_prsnt)) << 3))

#define ZGPD_COMM_MS_OPT_GET_MANUF_PRESENT(opt)\
  ((opt) & 1)

#define ZGPD_COMM_MS_OPT_GET_MODEL_PRESENT(opt)\
  (((opt) >> 1) & 1)

#define ZGPD_COMM_MS_OPT_GET_GPD_COMMANDS_PRESENT(opt)\
  (((opt) >> 2) & 1)

#define ZGPD_COMM_MS_OPT_GET_CLUSTERS_LIST_PRESENT(opt)\
  (((opt) >> 3) & 1)

#define ZGPD_MAX_MS_CMDS 4

#define ZGPD_MAX_MS_CLST_PER_DIR 2
#define ZGPD_MAX_MS_CLUSTERS (ZGPD_MAX_MS_CLST_PER_DIR * 2)

#define ZGPD_MS_CLUSTERS_INFO_FILL(srv, cli)\
  (((srv) & 0x0F) | (((cli) & 0x0F) << 4))

#define ZGPD_MS_CLUSTERS_GET_SRV_COUNT()\
  (ZGPD->clsts_list_size & 0x0F)

#define ZGPD_MS_CLUSTERS_GET_CLI_COUNT()\
  ((ZGPD->clsts_list_size & 0xF0) >> 4)

typedef struct zb_zgpd_app_descr_s
{
  zb_uint8_t total_reports;
  zb_uint8_t next_report;
  zgp_report_desc_t reports[10];
}zb_zgpd_app_descr_t;

/**
 * @brief ZGPD global context
 */
typedef struct zb_zgpd_ctx_s
{
  zb_zgpd_id_t      id;               /**< ZGPD ID (@ref zb_zgpd_id_t) */
  zb_uint8_t        device_id;        /**< ZGPD Device ID */

  zb_gpdf_comm_app_info_options_t app_info_options; /**< ZGPD Application information field */
  zb_zgpd_app_descr_t app_descr;
  zb_uint16_t       manuf_model_id;   /**< ZGPD Manufacturer-specific model ID
                                       * if device_id == ZB_ZGP_MANUF_SPECIFIC_DEV_ID */
  zb_uint16_t       manuf_id;  /**< ZGPD Manufacturer ID, if device_id ==
                                * ZB_ZGP_MANUF_SPECIFIC_DEV_ID */
  zb_uint8_t        gpd_cmds[ZGPD_MAX_MS_CMDS + 1]; // with size of array
  zb_uint8_t        clsts_list_size;
  zb_uint16_t       clsts_list[ZGPD_MAX_MS_CLUSTERS];
  zb_gpdf_comm_switch_gen_cfg_t switch_config;

  zb_uint8_t        commissioning_method; /**< Commissioning method @ref zb_zgpd_commissioning_method_t */
  zb_uint8_t        use_random_seq_num;   /**< If ZB_TRUE, then ZGPD uses random MAC sequence number.
                                             Otherwise, ZGPD uses incremental MAC sequence number.
                                             This field is ignored at current implementation. */
  zb_uint8_t        security_level;      /**< Security level used by ZGPD */
  zb_uint8_t        security_key_type;   /**< Security key type (@ref zb_zgp_security_key_type_e) */
  zb_uint8_t        security_key[ZB_CCM_KEY_SIZE]; /**< Security key */
  zb_uint8_t        oob_key[ZB_CCM_KEY_SIZE]; /**< Security out-of-the-box key */
  zb_bool_t         oob_key_present;

  zb_uint32_t       security_frame_counter; /**< Security frame counter */
  zb_uint8_t        gp_link_key[ZB_CCM_KEY_SIZE]; /**< The gpLinkKey attribute stores the Link Key, used to encrypt
                                                    * the key transmitted in the Commissioning GPDF and Commissioning Reply GPDF. */

  /** If ZB_TRUE, then auto-commissioning was requested by
   * user application. Therefore next command should be sent
   * with auto-commissioning bit set to 1 */
  zb_uint8_t        auto_commissioning_pending;
  zb_bool_t         maint_frame_for_channel_req; /** If ZB_TRUE, then Maintenance frame type
                                                   is used for Channel request command */

  zb_uint8_t        tx_cmd;               /**< Command will be transmitted */

  zb_uint8_t        tx_buf_ref;           /**< Buffer reference with data */

  /** If ZB_TRUE, then use src IEEE addr in MAC header
   * for commissioning frame even if appId is 0x00 */
  zb_bool_t         send_ieee_addr_in_comm_frame;
  zb_uint8_t        channel;
  zb_uint8_t        toggle_channel;
  zb_uint8_t        rx_on;

  /** commissioning values */
  zb_uint8_t        rx_on_capability; /**< rx on capability */
  zb_uint8_t        pan_id_request;  /**< Pan ID request */
  zb_uint8_t        gpd_security_key_request; /**< GPD security key request */
  zb_uint8_t        fixed_location;  /**< Fixed location */
  zb_uint8_t        security_key_present;  /**< Security key present */
  zb_uint8_t        security_key_encryption;  /**< Security key encryption */
  zb_uint8_t        gpd_outgoing_counter_present;  /**< GPD outgoing counter present */
  zb_uint8_t        application_info_present;  /**< Application Information Present */
  zb_uint16_t       pan_id;                        /**< Received Pan ID */

  /* Value of macRxOnWhenIdle PIB attribute that will be set during startup */
  zb_bool_t         rx_on_when_idle;

  zb_uint8_t        comm_state;

  zb_callback_t     user_cb;     /**< User callback for incoming data */
  zb_callback_t     comm_cb;     /**< Callback that is called after commissioning completes */
  zb_callback_t     startup_cb;  /**< Callback that is called after ZGPD startup completes */
  zb_callback_t     mac_dsn_cb;  /**< Callback that is called after ZGPD confirm update MAC DSN */
  zb_uint8_t        mac_dsn;     /**< New mac dsn value, set via ZB_PIB_ATTRIBUTE_DSN mlme request
                                  * */
  zb_uint8_t        ext_nwk_present; /**< Extended NWK field always present if true */
#ifdef ZB_CERTIFICATION_HACKS
  zb_uint32_t       options_to_modify_lo;
  zb_uint32_t       options_to_modify_hi;

  zb_uint8_t        ch_replace_key[ZB_CCM_KEY_SIZE];
  zb_uint8_t        ch_replace_key_type;
  zb_uint8_t        ch_replace_sec_level;
  zb_uint32_t       ch_replace_frame_counter;
  zb_uint8_t        ch_replace_frame_type;
  zb_uint8_t        ch_replace_proto_version;
  zb_uint8_t        ch_replace_rxaftertx;
  zb_uint8_t        ch_replace_autocomm;
  zb_uint8_t        ch_replace_nwk_fc;
  zb_uint8_t        ch_insert_extnwk_data;
  zb_uint8_t        ch_replace_extnwk_flag;
  zb_uint8_t        ch_replace_app_id;
  zb_uint8_t        ch_replace_direction;
  zb_uint32_t       ch_replace_src_id;
  zb_uint8_t        ch_tmp_nwk_hdr;
  zb_uint8_t        ch_tmp_ext_nwk_hdr;
  zb_uint8_t        ch_insert_frame_counter;
  zb_uint8_t        ch_replace_security_key_present;
  zb_uint8_t        ch_replace_security_key_encrypted;
  zb_uint8_t        ch_insert_security_key;
  zb_uint8_t        ch_insert_endpoint;
  zb_uint8_t        ch_max_comm_payload;
  zb_uint8_t        ch_comm_extra_payload_start_byte;
  zb_uint8_t        ch_comm_extra_payload;
  zb_uint8_t        ch_replace_cr_next_ch;
  zb_uint8_t        ch_replace_cr_transmit_ch;
  zb_uint8_t        ch_replace_cr_start_ch;
  zb_uint8_t        ch_skip_first_n_comm_reply;
  zb_uint8_t        ch_resend_success_gpdf;
  zb_uint8_t        ch_replace_cmd;
  zb_uint8_t        ch_replace_comm_sec_level;
#endif
}
zb_zgpd_ctx_t;

extern zb_zgpd_ctx_t g_zgpd_ctx;

/**
 * @brief Macro to access ZGPD global context
 */
#define ZGPD (&g_zgpd_ctx)

/**
 * @brief Initialize ZGPD global context
 *
 * After initialization the security is turned off and incremental
 * MAC sequence number is used.
 *
 * @param [in] _app_id         ZGPD application ID
 * @param [in] comm_method    commissioning method  @ref zb_zgpd_commissioning_method_t
 * @param [in] dev_id         ZGPD Device ID
 */
#define ZB_ZGPD_INIT_ZGPD_CTX(_app_id, comm_method, dev_id) \
{                                                           \
  memset(&g_zgpd_ctx, 0, sizeof(g_zgpd_ctx));               \
  g_zgpd_ctx.id.app_id = (_app_id);                         \
  g_zgpd_ctx.commissioning_method = (comm_method);          \
  g_zgpd_ctx.device_id = (dev_id);                          \
  g_zgpd_ctx.security_frame_counter = 0;                    \
  g_zgpd_ctx.rx_on_when_idle = ZB_TRUE;                     \
  zb_zgpd_device_reset_security();                          \
}


#define ZB_ZGPD_REQUEST_SECURITY_KEY()     \
{                                          \
  ZB_ASSERT(g_zgpd_ctx.security_level>ZB_ZGP_SEC_LEVEL_REDUCED);\
  g_zgpd_ctx.gpd_security_key_request = 1; \
}
/**
 * @brief Set ZGPD SrcID
 *
 * @param [in] _id   ZGPD SrcId
 */
#define ZB_ZGPD_SET_SRC_ID(_id) \
  g_zgpd_ctx.id.addr.src_id = (_id)


/**
 * @brief Set ZGPD manufacturer id and manufacturer-specific device id
 *
 * @param [in] manuf_id       manufacturer id
 * @param [in] model_id       manufacturer model id
 */
#define ZB_ZGPD_SET_MANUF_SPECIFIC_DEV(_manuf_id, _model_id)       \
{                                                               \
  g_zgpd_ctx.manuf_id = (_manuf_id);                              \
  g_zgpd_ctx.manuf_model_id = (_model_id);                          \
}

#define ZB_ZGPD_SET_SECURITY_LEVEL(level) {g_zgpd_ctx.security_level = level;}
#define ZB_ZGPD_SET_SECURITY_KEY_TYPE(kt) {g_zgpd_ctx.security_key_type = kt;}

#define ZB_ZGPD_SET_SECURITY_KEY(key)              \
{                                                  \
  if (g_zgpd_ctx.security_key_type > ZB_ZGP_SEC_KEY_TYPE_NO_KEY)       \
  {                                                \
    ZB_MEMCPY(g_zgpd_ctx.security_key, key, sizeof(g_zgpd_ctx.security_key));  \
  }                                                \
}

#define ZB_ZGPD_SET_SECURITY(level, key_type, key) \
{                                                  \
  g_zgpd_ctx.security_level    = level;            \
  g_zgpd_ctx.security_key_type = key_type;         \
  if (level > ZB_ZGP_SEC_LEVEL_NO_SECURITY)        \
  {                                                \
    ZB_MEMCPY(g_zgpd_ctx.security_key, key, sizeof(g_zgpd_ctx.security_key));  \
  }                                                \
}

#define ZB_ZGPD_SET_OOB_KEY(key) \
{                                                  \
    ZB_MEMCPY(g_zgpd_ctx.oob_key, key, sizeof(g_zgpd_ctx.oob_key));  \
    g_zgpd_ctx.oob_key_present = ZB_TRUE;          \
    g_zgpd_ctx.security_key_present = 1;           \
    if(g_zgpd_ctx.security_key_type == ZB_ZGP_SEC_KEY_TYPE_ZGPD_INDIVIDUAL) \
    { \
      ZB_MEMCPY(g_zgpd_ctx.security_key, key, sizeof(g_zgpd_ctx.security_key));  \
    } \
}
/**
 * @brief Start using random sequence number in outgoing GPDFs
 *
 * Random MAC sequence number is not implemented yet.
 */
#define ZB_ZGPD_USE_RANDOM_SEQ_NUM() \
  g_zgpd_ctx.use_random_seq_num = ZB_TRUE

/**
 * @brief Use Maintenance frame type for outgoing Channel requests
 */
#define ZB_ZGPD_USE_MAINTENANCE_FRAME_FOR_CHANNEL_REQ() \
  g_zgpd_ctx.maint_frame_for_channel_req = ZB_TRUE

/**
 * @brief Fill IEEE Src addr in MAC header for commissioning frame
 */
#define ZB_ZGPD_SEND_IEEE_SRC_ADDR_IN_COMM_REQ() \
  g_zgpd_ctx.send_ieee_addr_in_comm_frame = ZB_TRUE


/**
 * @brief Send data GPDF
 * @param  [in] param   reference to the buffer with GPDF payload
 */
#define ZB_SEND_DATA_GPDF_CMD(param) zb_zgpd_send_data_req((param), ZB_TRUE)


/**
 * @brief Send data GPDF
 * @param  [in] param   reference to the buffer with GPDF payload
 */
#define ZB_SEND_MAINTENANCE_GPDF(param) zb_zgpd_send_maint_req(param)


/**
 * @brief Send GPDF packet where a command has no payload (only command id present)
 *
 * @param buf       [in] Buffer for GPDF packet
 * @param cmd_id    [in] ZGPD command id (see @ref zb_zgpd_cmd_id_e)
 * @param use_secur [in] Whether to protect transmitted packet or not
 */
#define ZB_SEND_PAYLOADLESS_GPDF(buf, cmd_id, use_secur) \
{ \
  zb_uint8_t* ptr = ZB_START_GPDF_PACKET(buf); \
  ptr = zb_buf_alloc_left(buf, 1);              \
  *ptr = cmd_id; \
  ZGPD->tx_cmd = cmd_id; \
  zb_zgpd_send_data_req(buf, use_secur); \
}


/********************************************************************/
/*************************** ZGPD operations API  *******************/
/********************************************************************/

/**
 * @brief ZGPD main loop
 */
void zgpd_main_loop(void);

/**
 * @brief Start commissioning at ZGPD side
 *
 * @param cb - Callback to call after commissioning is completed
 * @see ZGP spec, A.3.9
 */
void zb_zgpd_start_commissioning(zb_callback_t cb);


/**
 * @brief Send decommissioning frame
 *
 * @see ZGP spec, A.3.9
 */
void zb_zgpd_decommission(void);


/**
 * @brief Send Data GPDF frame
 *
 * @param buf_ref    [in]  Buffer with GPDF payload
 * @param use_secur  [in]  Whether to protect outgoing frame or not
 */
void zb_zgpd_send_data_req(zb_uint8_t buf_ref, zb_bool_t use_secur);


/**
 * @brief Send Maintenance GPDF frame
 *
 * @param buf_ref    [in]  Buffer with GPDF payload
 */
void zb_zgpd_send_maint_req(zb_uint8_t buf_ref);


/**
 * @brief ZGPD start function
 *
 * Initialization of ZGPD context with correct values.
 * After initialization is done, provided callback is called
 *
 * @param   cb [in]  Callback to be called after ZGPD startup is complete.
 *                   Buffer parameter contains result of startup in hdr.status field.
 *                   hdr.status has @ref zb_ret_t type
 * @return  RET_OK if startup procedure starts successfully
 */
zb_ret_t zb_zgpd_dev_start(zb_callback_t cb);

void zb_zgpd_device_reset_security(void);

/** @} */ //@addtogroup zgp_zgpd

#endif //ZB_ZGPD_H
