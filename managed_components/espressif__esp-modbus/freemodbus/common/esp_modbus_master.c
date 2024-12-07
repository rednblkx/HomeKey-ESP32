/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "esp_err.h"                // for esp_err_t
#include "mbc_master.h"             // for master interface define
#include "esp_modbus_master.h"      // for public interface defines
#include "esp_modbus_callbacks.h"   // for callback functions
#include "sdkconfig.h"

static const char TAG[] __attribute__((unused)) = "MB_CONTROLLER_MASTER";

// This file implements public API for Modbus master controller.
// These functions are wrappers for interface functions of the controller
static mb_master_interface_t* master_interface_ptr = NULL;

void mbc_master_init_iface(void* handler)
{
    master_interface_ptr = (mb_master_interface_t*) handler;
}

/**
 * Modbus controller destroy function
 */
esp_err_t mbc_master_destroy(void)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->destroy != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->destroy();
    MB_MASTER_CHECK((error == ESP_OK),
                    error,
                    "Master destroy failure, error=(0x%x).",
                    (int)error);
    return error;
}

esp_err_t mbc_master_get_cid_info(uint16_t cid, const mb_parameter_descriptor_t** param_info)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->get_cid_info != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->get_cid_info(cid, param_info);
    MB_MASTER_CHECK((error == ESP_OK),
                    error,
                    "Master get cid info failure, error=(0x%x).",
                    (int)error);
    return error;
}

/**
 * Get parameter data for corresponding characteristic
 */
esp_err_t mbc_master_get_parameter(uint16_t cid, char* name, uint8_t* value, uint8_t* type)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->get_parameter != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->get_parameter(cid, name, value, type);
    MB_MASTER_CHECK((error == ESP_OK),
                    error,
                    "Master get parameter failure, error=(0x%x) (%s).",
                    (int)error, esp_err_to_name(error));
    return error;
}

/**
 * Send custom Modbus request defined as mb_param_request_t structure
 */
esp_err_t mbc_master_send_request(mb_param_request_t* request, void* data_ptr)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->send_request != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->send_request(request, data_ptr);
    MB_MASTER_CHECK((error == ESP_OK),
                    error,
                    "Master send request failure error=(0x%x) (%s).",
                    (int)error, esp_err_to_name(error));
    return ESP_OK;
}

/**
 * Set Modbus parameter description table
 */
esp_err_t mbc_master_set_descriptor(const mb_parameter_descriptor_t* descriptor,
                                        const uint16_t num_elements)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->set_descriptor != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->set_descriptor(descriptor, num_elements);
    MB_MASTER_CHECK((error == ESP_OK),
                    error,
                    "Master set descriptor failure, error=(0x%x) (%s).",
                    (int)error, esp_err_to_name(error));
    return ESP_OK;
}

/**
 * Set parameter value for characteristic selected by name and cid
 */
esp_err_t mbc_master_set_parameter(uint16_t cid, char* name, uint8_t* value, uint8_t* type)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->set_parameter != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->set_parameter(cid, name, value, type);
    MB_MASTER_CHECK((error == ESP_OK),
                    error,
                    "Master set parameter failure, error=(0x%x) (%s).",
                    (int)error, esp_err_to_name(error));
    return ESP_OK;
}

/**
 * Setup Modbus controller parameters
 */
esp_err_t mbc_master_setup(void* comm_info)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->setup != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->setup(comm_info);
    MB_MASTER_CHECK((error == ESP_OK),
                    error,
                    "Master setup failure, error=(0x%x) (%s).",
                    (int)error, esp_err_to_name(error));
    return ESP_OK;
}

/**
 * Modbus controller stack start function
 */
esp_err_t mbc_master_start(void)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->start != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->start();
    MB_MASTER_CHECK((error == ESP_OK),
                    error,
                    "Master start failure, error=(0x%x) (%s).",
                    (int)error, esp_err_to_name(error));
    return ESP_OK;
}

eMBErrorCode eMBMasterRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                        USHORT usNDiscrete)
{
    eMBErrorCode error = MB_ENOERR;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->master_reg_cb_discrete != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->master_reg_cb_discrete(pucRegBuffer, usAddress, usNDiscrete);
    return error;
}

eMBErrorCode eMBMasterRegCoilsCB(UCHAR* pucRegBuffer, USHORT usAddress,
                                    USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode error = MB_ENOERR;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->master_reg_cb_coils != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->master_reg_cb_coils(pucRegBuffer, usAddress,
                                                        usNCoils, eMode);
    return error;
}

eMBErrorCode eMBMasterRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                    USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode error = MB_ENOERR;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->master_reg_cb_holding != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->master_reg_cb_holding(pucRegBuffer, usAddress,
                                                        usNRegs, eMode);
    return error;
}

eMBErrorCode eMBMasterRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                    USHORT usNRegs)
{
    eMBErrorCode error = MB_ENOERR;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->master_reg_cb_input != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->master_reg_cb_input(pucRegBuffer, usAddress, usNRegs);
    return error;
}

eMBErrorCode eMBMasterRegCommonCB(UCHAR * pucData, USHORT usAddress,
                                    USHORT usBytes)
{
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    MB_EILLSTATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    MB_EILLSTATE,
                    "Master interface uninitialized.");
    MB_MASTER_CHECK(pucData, MB_EINVAL,
                    "Master stack processing error.");
    mb_master_options_t* popts = &master_interface_ptr->opts;
    USHORT usRegLen = (USHORT)popts->mbm_reg_buffer_size;
    UCHAR* pucParBuffer = (UCHAR*)popts->mbm_reg_buffer_ptr; // Get instance address
    eMBErrorCode eStatus = MB_ENOERR;
    if (pucParBuffer && !usAddress && (usBytes >= 1) && (((usRegLen << 1) >= usBytes))){
            memmove(pucParBuffer, pucData, MIN((usRegLen << 1), usBytes));
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
 * Helper function to get current transaction info
 */
esp_err_t mbc_master_get_transaction_info(mb_trans_info_t *ptinfo)
{
    MB_MASTER_CHECK((ptinfo),
                    ESP_ERR_INVALID_ARG,
                    "Wrong argument.");
    MB_MASTER_CHECK(xMBMasterGetLastTransactionInfo(&ptinfo->trans_id, &ptinfo->dest_addr,
                                                    &ptinfo->func_code, &ptinfo->exception,
                                                    &ptinfo->err_type),
                    ESP_ERR_INVALID_STATE,
                    "Master can not get transaction info.");
    return ESP_OK;
}

// Helper function to set parameter buffer according to its type
esp_err_t mbc_master_set_param_data(void* dest, void* src, mb_descr_type_t param_type, size_t param_size)
{
    esp_err_t err = ESP_OK;
    MB_RETURN_ON_FALSE((src), ESP_ERR_INVALID_STATE, TAG,"incorrect data pointer.");
    MB_RETURN_ON_FALSE((dest), ESP_ERR_INVALID_STATE, TAG,"incorrect data pointer.");
    void *pdest = dest;
    void *psrc = src;

    // Transfer parameter data into value of characteristic
    switch(param_type)
    {
        case PARAM_TYPE_U8:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U8) {
                *((uint8_t*)pdest) = *((uint8_t*)psrc);
            }
            break;

        case PARAM_TYPE_U16:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U16) {
                *((uint16_t*)pdest) = *((uint16_t*)psrc);
            }
            break;

        case PARAM_TYPE_U32:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U32) {
                *((uint32_t*)pdest) = *((uint32_t*)psrc);
            }
            break;

        case PARAM_TYPE_FLOAT:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_FLOAT) {
                *((float*)pdest) = *(float*)psrc;
            }
            break;

        case PARAM_TYPE_ASCII:
        case PARAM_TYPE_BIN:
            memcpy((void*)dest, (void*)src, (size_t)param_size);
            break;

#if CONFIG_FMB_EXT_TYPE_SUPPORT

        case PARAM_TYPE_I8_A:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U8_REG) {
                mb_set_int8_a((val_16_arr *)pdest, (*(int8_t*)psrc));
                ESP_LOGV(TAG, "Convert uint8 B[%d] 0x%04" PRIx16 " = 0x%04" PRIx16, i, *(uint16_t *)psrc, *(uint16_t *)pdest);
            }
            break;

        case PARAM_TYPE_I8_B:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U8_REG) {
                mb_set_int8_b((val_16_arr *)pdest, (int8_t)((*(uint16_t*)psrc) >> 8));
                ESP_LOGV(TAG, "Convert int8 A[%d] 0x%02" PRIx16 " = 0x%02" PRIx16, i, *(uint16_t *)psrc, *(uint16_t *)pdest);
            }
            break;

        case PARAM_TYPE_U8_A:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U8_REG) {
                mb_set_uint8_a((val_16_arr *)pdest, (*(uint8_t*)psrc));
                ESP_LOGV(TAG, "Convert uint8 A[%d] 0x%02" PRIx16 " = %02" PRIx16, i, *(uint16_t *)psrc, *(uint16_t *)pdest);
            }
            break;

        case PARAM_TYPE_U8_B:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U8_REG) {
                uint8_t data = (uint8_t)((*(uint16_t*)psrc) >> 8);
                mb_set_uint8_b((val_16_arr *)pdest, data);
                ESP_LOGV(TAG, "Convert uint8 B[%d] 0x%02" PRIx16 " = 0x%02" PRIx16, i, *(uint16_t *)psrc, *(uint16_t *)pdest);
            }
            break;

        case PARAM_TYPE_I16_AB:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I16) {
                mb_set_int16_ab((val_16_arr *)pdest, *(int16_t*)psrc);
                ESP_LOGV(TAG, "Convert int16 AB[%d] 0x%04" PRIx16 " = 0x%04" PRIx16, i, *(uint16_t *)psrc, *(uint16_t *)pdest);
            }
            break;

        case PARAM_TYPE_I16_BA:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I16) {
                mb_set_int16_ba((val_16_arr *)pdest, *(int16_t*)psrc);
                ESP_LOGV(TAG, "Convert int16 BA[%d] 0x%04" PRIx16 " = 0x%04" PRIx16, i, *(uint16_t *)psrc, *(uint16_t *)pdest);
            }
            break;

        case PARAM_TYPE_U16_AB:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U16) {
                mb_set_uint16_ab((val_16_arr *)pdest, *(uint16_t*)psrc);
                ESP_LOGV(TAG, "Convert uint16 AB[%d] 0x%02" PRIx16 " = 0x%02" PRIx16, i, *(uint16_t *)psrc, *(uint16_t *)pdest);
            }
            break;

        case PARAM_TYPE_U16_BA:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U16) {
                mb_set_uint16_ba((val_16_arr *)pdest, *(uint16_t*)psrc);
                ESP_LOGV(TAG, "Convert uint16 BA[%d] 0x%02" PRIx16 " = 0x%02" PRIx16, i, *(uint16_t *)psrc, *(uint16_t *)pdest);
            }
            break;

        case PARAM_TYPE_I32_ABCD:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I32) {
                mb_set_int32_abcd((val_32_arr *)pdest, *(int32_t *)psrc);
                ESP_LOGV(TAG, "Convert int32 ABCD[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_U32_ABCD:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U32) {
                mb_set_uint32_abcd((val_32_arr *)pdest, *(uint32_t *)psrc);
                ESP_LOGV(TAG, "Convert uint32 ABCD[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_FLOAT_ABCD:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_FLOAT) {
                mb_set_float_abcd((val_32_arr *)pdest, *(float *)psrc);
                ESP_LOGV(TAG, "Convert float ABCD[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_I32_CDAB:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I32) {
                mb_set_int32_cdab((val_32_arr *)pdest, *(int32_t *)psrc);
                ESP_LOGV(TAG, "Convert int32 CDAB[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_U32_CDAB:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U32) {
                mb_set_uint32_cdab((val_32_arr *)pdest, *(uint32_t *)psrc);
                ESP_LOGV(TAG, "Convert uint32 CDAB[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_FLOAT_CDAB:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_FLOAT) {
                mb_set_float_cdab((val_32_arr *)pdest, *(float *)psrc);
                ESP_LOGV(TAG, "Convert float CDAB[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_I32_BADC:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I32) {
                mb_set_int32_badc((val_32_arr *)pdest, *(int32_t *)psrc);
                ESP_LOGV(TAG, "Convert int32 BADC[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_U32_BADC:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U32) {
                mb_set_uint32_badc((val_32_arr *)pdest, *(uint32_t *)psrc);
                ESP_LOGV(TAG, "Convert uint32 BADC[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_FLOAT_BADC:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_FLOAT) {
                mb_set_float_badc((val_32_arr *)pdest, *(float *)psrc);
                ESP_LOGV(TAG, "Convert float BADC[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_I32_DCBA:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I32) {
                mb_set_int32_dcba((val_32_arr *)pdest, *(int32_t *)psrc);
                ESP_LOGV(TAG, "Convert int32 DCBA[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_U32_DCBA:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U32) {
                mb_set_uint32_dcba((val_32_arr *)pdest, *(uint32_t *)psrc);
                ESP_LOGV(TAG, "Convert uint32 DCBA[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_FLOAT_DCBA:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_FLOAT) {
                mb_set_float_dcba((val_32_arr *)pdest, *(float *)psrc);
                ESP_LOGV(TAG, "Convert float DCBA[%d] 0x%04" PRIx32 " = 0x%04" PRIx32, i, *(uint32_t *)psrc, *(uint32_t *)pdest);
            }
            break;

        case PARAM_TYPE_I64_ABCDEFGH:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I64) {
                mb_set_int64_abcdefgh((val_64_arr *)pdest, *(int64_t *)psrc);
                ESP_LOGV(TAG, "Convert int64 ABCDEFGH[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_U64_ABCDEFGH:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U64) {
                mb_set_uint64_abcdefgh((val_64_arr *)pdest, *(uint64_t *)psrc);
                ESP_LOGV(TAG, "Convert double ABCDEFGH[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_DOUBLE_ABCDEFGH:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_DOUBLE) {
                mb_set_double_abcdefgh((val_64_arr *)pdest, *(double *)psrc);
                ESP_LOGV(TAG, "Convert double ABCDEFGH[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_I64_HGFEDCBA:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I64) {
                mb_set_int64_hgfedcba((val_64_arr *)pdest, *(int64_t *)psrc);
                ESP_LOGV(TAG, "Convert int64 HGFEDCBA[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_U64_HGFEDCBA:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U64) {
                mb_set_uint64_hgfedcba((val_64_arr *)pdest, *(uint64_t *)psrc);
                ESP_LOGV(TAG, "Convert double HGFEDCBA[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_DOUBLE_HGFEDCBA:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_DOUBLE) {
                mb_set_double_hgfedcba((val_64_arr *)pdest, *(double *)psrc);
                ESP_LOGV(TAG, "Convert double HGFEDCBA[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_I64_GHEFCDAB:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I64) {
                mb_set_int64_ghefcdab((val_64_arr *)pdest, *(int64_t *)psrc);
                ESP_LOGV(TAG, "Convert int64 GHEFCDAB[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_U64_GHEFCDAB:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U64) {
                mb_set_uint64_ghefcdab((val_64_arr *)pdest, *(uint64_t *)psrc);
                ESP_LOGV(TAG, "Convert uint64 GHEFCDAB[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_DOUBLE_GHEFCDAB:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_DOUBLE) {
                mb_set_double_ghefcdab((val_64_arr *)pdest, *(double *)psrc);
                ESP_LOGV(TAG, "Convert double GHEFCDAB[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_I64_BADCFEHG:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_I64) {
                mb_set_int64_badcfehg((val_64_arr *)pdest, *(int64_t *)psrc);
                ESP_LOGV(TAG, "Convert int64 BADCFEHG[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_U64_BADCFEHG:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_U64) {
                mb_set_uint64_badcfehg((val_64_arr *)pdest, *(uint64_t *)psrc);
                ESP_LOGV(TAG, "Convert uint64 BADCFEHG[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

        case PARAM_TYPE_DOUBLE_BADCFEHG:
            for MB_EACH_ELEM(psrc, pdest, param_size, PARAM_SIZE_DOUBLE) {
                mb_set_double_badcfehg((val_64_arr *)pdest, *(double *)psrc);
                ESP_LOGV(TAG, "Convert double BADCFEHG[%d] 0x%" PRIx64 " = 0x%" PRIx64, i, *(uint64_t *)psrc, *(uint64_t *)pdest);
            }
            break;

#endif
        default:
            ESP_LOGE(TAG, "%s: Incorrect param type (%u).",
                        __FUNCTION__, (unsigned)param_type);
            err = ESP_ERR_NOT_SUPPORTED;
            break;
    }
    return err;
}
