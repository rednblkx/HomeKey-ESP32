/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_MB_MASTER_INTERFACE_H
#define _ESP_MB_MASTER_INTERFACE_H

#include <stdint.h>                 // for standard int types definition
#include <stddef.h>                 // for NULL and std defines
#include "soc/soc.h"                // for BITN definitions
#include "esp_modbus_common.h"      // for common types

#ifdef __cplusplus
extern "C" {
#endif

#define MB_MASTER_CHECK(a, err_code, format, ...) MB_RETURN_ON_FALSE(a, err_code, TAG, format __VA_OPT__(,) __VA_ARGS__)

#define MB_MASTER_ASSERT(con) do { \
        if (!(con)) { ESP_LOGE(TAG, "assert errno:%u, errno_str: !(%s)", (unsigned)errno, strerror(errno)); assert(0 && #con); } \
    } while (0)

/*!
 * \brief The macro to access arrays of elements for type conversion.
 */
#define MB_EACH_ELEM(psrc, pdest, arr_size, elem_size) \
(int i = 0; (i < (arr_size / elem_size)); i++, pdest += elem_size, psrc += elem_size)

/*!
 * \brief Modbus descriptor table parameter type defines.
 */
typedef enum {
    PARAM_TYPE_U8 = 0x00,                   /*!< Unsigned 8 */
    PARAM_TYPE_U16 = 0x01,                  /*!< Unsigned 16 */
    PARAM_TYPE_U32 = 0x02,                  /*!< Unsigned 32 */
    PARAM_TYPE_FLOAT = 0x03,                /*!< Float type */
    PARAM_TYPE_ASCII = 0x04,                /*!< ASCII type */
    PARAM_TYPE_BIN = 0x07,                  /*!< BIN type */
    PARAM_TYPE_I8_A = 0x0A,                 /*!< I8 signed integer in high byte of register */
    PARAM_TYPE_I8_B = 0x0B,                 /*!< I8 signed integer in low byte of register */
    PARAM_TYPE_U8_A = 0x0C,                 /*!< U8 unsigned integer written to hi byte of register */
    PARAM_TYPE_U8_B = 0x0D,                 /*!< U8 unsigned integer written to low byte of register */
    PARAM_TYPE_I16_AB = 0x0E,               /*!< I16 signed integer, big endian */
    PARAM_TYPE_I16_BA = 0x0F,               /*!< I16 signed integer, little endian */
    PARAM_TYPE_U16_AB = 0x10,               /*!< U16 unsigned integer, big endian*/
    PARAM_TYPE_U16_BA = 0x11,               /*!< U16 unsigned integer, little endian */
    PARAM_TYPE_I32_ABCD = 0x12,             /*!< I32 ABCD signed integer, big endian */
    PARAM_TYPE_I32_CDAB = 0x13,             /*!< I32 CDAB signed integer, big endian, reversed register order */
    PARAM_TYPE_I32_BADC = 0x14,             /*!< I32 BADC signed integer, little endian, reversed register order */
    PARAM_TYPE_I32_DCBA = 0x15,             /*!< I32 DCBA signed integer, little endian */
    PARAM_TYPE_U32_ABCD = 0x16,             /*!< U32 ABCD unsigned integer, big endian */
    PARAM_TYPE_U32_CDAB = 0x17,             /*!< U32 CDAB unsigned integer, big endian, reversed register order */
    PARAM_TYPE_U32_BADC = 0x18,             /*!< U32 BADC unsigned integer, little endian, reversed register order */
    PARAM_TYPE_U32_DCBA = 0x19,             /*!< U32 DCBA unsigned integer, little endian */
    PARAM_TYPE_FLOAT_ABCD = 0x1A,           /*!< Float ABCD floating point, big endian */
    PARAM_TYPE_FLOAT_CDAB = 0x1B,           /*!< Float CDAB floating point big endian, reversed register order */
    PARAM_TYPE_FLOAT_BADC = 0x1C,           /*!< Float BADC floating point, little endian, reversed register order */
    PARAM_TYPE_FLOAT_DCBA = 0x1D,           /*!< Float DCBA floating point, little endian */
    PARAM_TYPE_I64_ABCDEFGH = 0x1E,         /*!< I64, ABCDEFGH signed integer, big endian */
    PARAM_TYPE_I64_HGFEDCBA = 0x1F,         /*!< I64, HGFEDCBA signed integer, little endian */
    PARAM_TYPE_I64_GHEFCDAB = 0x20,         /*!< I64, GHEFCDAB signed integer, big endian, reversed register order */
    PARAM_TYPE_I64_BADCFEHG = 0x21,         /*!< I64, BADCFEHG signed integer, little endian, reversed register order */
    PARAM_TYPE_U64_ABCDEFGH = 0x22,         /*!< U64, ABCDEFGH unsigned integer, big endian */
    PARAM_TYPE_U64_HGFEDCBA = 0x23,         /*!< U64, HGFEDCBA unsigned integer, little endian */
    PARAM_TYPE_U64_GHEFCDAB = 0x24,         /*!< U64, GHEFCDAB unsigned integer, big endian, reversed register order */
    PARAM_TYPE_U64_BADCFEHG = 0x25,         /*!< U64, BADCFEHG unsigned integer, little endian, reversed register order */
    PARAM_TYPE_DOUBLE_ABCDEFGH = 0x26,      /*!< Double ABCDEFGH floating point, big endian*/
    PARAM_TYPE_DOUBLE_HGFEDCBA = 0x27,      /*!< Double HGFEDCBA floating point, little endian*/
    PARAM_TYPE_DOUBLE_GHEFCDAB = 0x28,      /*!< Double GHEFCDAB floating point, big endian, reversed register order */
    PARAM_TYPE_DOUBLE_BADCFEHG = 0x29       /*!< Double BADCFEHG floating point, little endian, reversed register order */
} mb_descr_type_t;

/*!
 * \brief Modbus descriptor table parameter size in bytes.
 */
typedef enum {
    PARAM_SIZE_U8 = 0x01,                   /*!< Unsigned 8 */
    PARAM_SIZE_U8_REG = 0x02,               /*!< Unsigned 8, register value */
    PARAM_SIZE_I8_REG = 0x02,               /*!< Signed 8, register value */
    PARAM_SIZE_I16 = 0x02,                  /*!< Unsigned 16 */
    PARAM_SIZE_U16 = 0x02,                  /*!< Unsigned 16 */
    PARAM_SIZE_I32 = 0x04,                  /*!< Signed 32 */
    PARAM_SIZE_U32 = 0x04,                  /*!< Unsigned 32 */
    PARAM_SIZE_FLOAT = 0x04,                /*!< Float 32 size */
    PARAM_SIZE_ASCII = 0x08,                /*!< ASCII size default*/
    PARAM_SIZE_ASCII24 = 0x18,              /*!< ASCII24 size */
    PARAM_SIZE_I64 = 0x08,                  /*!< Signed integer 64 size */
    PARAM_SIZE_U64 = 0x08,                  /*!< Unsigned integer 64 size */
    PARAM_SIZE_DOUBLE = 0x08,               /*!< Double 64 size */
    PARAM_MAX_SIZE
} mb_descr_size_t;

/*!
 * \brief Modbus parameter options for description table
 */
typedef union {
    struct {
        int opt1;                         /*!< Parameter option1 */
        int opt2;                         /*!< Parameter option2 */
        int opt3;                         /*!< Parameter option3 */
    }; /*!< Parameter options version 1 */
    struct {
        int min;                          /*!< Parameter minimum value */
        int max;                          /*!< Parameter maximum value */
        int step;                         /*!< Step of parameter change tracking */
    }; /*!< Parameter options version 2 */
} mb_parameter_opt_t;

/**
 * @brief Permissions for the characteristics
 */
typedef enum {
    PAR_PERMS_READ               = 1 << BIT0,                                   /**< the characteristic of the device are readable */
    PAR_PERMS_WRITE              = 1 << BIT1,                                   /**< the characteristic of the device are writable*/
    PAR_PERMS_TRIGGER            = 1 << BIT2,                                   /**< the characteristic of the device are triggerable */
    PAR_PERMS_READ_WRITE         = PAR_PERMS_READ | PAR_PERMS_WRITE,            /**< the characteristic of the device are readable & writable */
    PAR_PERMS_READ_TRIGGER       = PAR_PERMS_READ | PAR_PERMS_TRIGGER,          /**< the characteristic of the device are readable & triggerable */
    PAR_PERMS_WRITE_TRIGGER      = PAR_PERMS_WRITE | PAR_PERMS_TRIGGER,         /**< the characteristic of the device are writable & triggerable */
    PAR_PERMS_READ_WRITE_TRIGGER = PAR_PERMS_READ_WRITE | PAR_PERMS_TRIGGER,    /**< the characteristic of the device are readable & writable & triggerable */
} mb_param_perms_t;

/**
 * @brief Characteristics descriptor type is used to describe characteristic and
 * link it with Modbus parameters that reflect its data.
 */
typedef struct {
    uint16_t            cid;                /*!< Characteristic cid */
    const char*         param_key;          /*!< The key (name) of the parameter */
    const char*         param_units;        /*!< The physical units of the parameter */
    uint8_t             mb_slave_addr;      /*!< Slave address of device in the Modbus segment */
    mb_param_type_t     mb_param_type;      /*!< Type of modbus parameter */
    uint16_t            mb_reg_start;       /*!< This is the Modbus register address. This is the 0 based value. */
    uint16_t            mb_size;            /*!< Size of mb parameter in registers */
    uint16_t            param_offset;       /*!< Parameter name (OFFSET in the parameter structure) */
    mb_descr_type_t     param_type;         /*!< Float, U8, U16, U32, ASCII, etc. */
    mb_descr_size_t     param_size;         /*!< Number of bytes in the parameter. */
    mb_parameter_opt_t  param_opts;         /*!< Parameter options used to check limits and etc. */
    mb_param_perms_t    access;             /*!< Access permissions based on mode */
} mb_parameter_descriptor_t;

/**
 * @brief Modbus register request type structure
 */
typedef struct {
    uint8_t slave_addr;             /*!< Modbus slave address */
    uint8_t command;                /*!< Modbus command to send */
    uint16_t reg_start;             /*!< Modbus start register */
    uint16_t reg_size;              /*!< Modbus number of registers */
} mb_param_request_t;

/**
 * @brief Modbus transacion info structure
 */
typedef struct {
    uint64_t trans_id;              /*!< Modbus unique transaction identificator */
    uint16_t err_type;              /*!< Modbus last transaction error type */
    uint8_t dest_addr;              /*!< Modbus destination short address (or UID) */
    uint8_t func_code;              /*!< Modbus last transaction function code */
    uint8_t exception;              /*!< Modbus last transaction exception code returned by slave */
} mb_trans_info_t;

/**
 * @brief Initialize Modbus controller and stack for TCP port
 *
 * @param[out] handler handler(pointer) to master data structure
 * @return
 *     - ESP_OK                 Success
 *     - ESP_ERR_NO_MEM         Parameter error
 *     - ESP_ERR_NOT_SUPPORTED  Port type not supported
 *     - ESP_ERR_INVALID_STATE  Initialization failure
 */
esp_err_t mbc_master_init_tcp(void** handler);

/**
 * @brief Initialize Modbus Master controller and stack for Serial port
 *
 * @param[out] handler handler(pointer) to master data structure
 * @param[in] port_type type of stack
 * @return
 *     - ESP_OK                 Success
 *     - ESP_ERR_NO_MEM         Parameter error
 *     - ESP_ERR_NOT_SUPPORTED  Port type not supported
 *     - ESP_ERR_INVALID_STATE  Initialization failure
 */
esp_err_t mbc_master_init(mb_port_type_t port_type, void** handler);

/**
 * @brief Initialize Modbus Master controller interface handle
 *
 * @param[in] handler - pointer to master data structure
 */
void mbc_master_init_iface(void* handler);

/**
 * @brief Destroy Modbus controller and stack
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_INVALID_STATE Parameter error
 */
esp_err_t mbc_master_destroy(void);

/**
 * @brief Start Modbus communication stack
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_INVALID_ARG Modbus stack start error
 */
esp_err_t mbc_master_start(void);

/**
 * @brief Set Modbus communication parameters for the controller
 *
 * @param comm_info Communication parameters structure.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Incorrect parameter data
 */
esp_err_t mbc_master_setup(void* comm_info);

/***************************** Specific interface functions ********************************************
 * Interface functions below provide basic methods to read/write access to slave devices in Modbus
 * segment as well as API to read specific supported characteristics linked to Modbus parameters
 * of devices in Modbus network.
*******************************************************************************************************/

/**
 * @brief Assign parameter description table for Modbus controller interface.
 *
 * @param[in] descriptor pointer to parameter description table
 * @param num_elements number of elements in the table
 *
 * @return
 *     - esp_err_t ESP_OK - set descriptor successfully
 *     - esp_err_t ESP_ERR_INVALID_ARG - invalid argument in function call
 */
esp_err_t mbc_master_set_descriptor(const mb_parameter_descriptor_t* descriptor, const uint16_t num_elements);

/**
 * @brief Send data request as defined in parameter request, waits response
 *        from slave and returns status of command execution. This function provides standard way
 *        for read/write access to Modbus devices in the network.
 *
 * @param[in] request pointer to request structure of type mb_param_request_t
 * @param[in] data_ptr pointer to data buffer to send or received data (dependent of command field in request)
 *
 * @return
 *     - esp_err_t ESP_OK - request was successful
 *     - esp_err_t ESP_ERR_INVALID_ARG - invalid argument of function
 *     - esp_err_t ESP_ERR_INVALID_RESPONSE - an invalid response from slave
 *     - esp_err_t ESP_ERR_TIMEOUT - operation timeout or no response from slave
 *     - esp_err_t ESP_ERR_NOT_SUPPORTED - the request command is not supported by slave
 *     - esp_err_t ESP_FAIL - slave returned an exception or other failure
 */
esp_err_t mbc_master_send_request(mb_param_request_t* request, void* data_ptr);

/**
 * @brief Get information about supported characteristic defined as cid. Uses parameter description table to get
 *        this information. The function will check if characteristic defined as a cid parameter is supported
 *        and returns its description in param_info. Returns ESP_ERR_NOT_FOUND if characteristic is not supported.
 *
 * @param[in] cid characteristic id
 * @param param_info pointer to pointer of characteristic data.
 *
 * @return
 *     - esp_err_t ESP_OK - request was successful and buffer contains the supported characteristic name
 *     - esp_err_t ESP_ERR_INVALID_ARG - invalid argument of function
 *     - esp_err_t ESP_ERR_NOT_FOUND - the characteristic (cid) not found
 *     - esp_err_t ESP_FAIL - unknown error during lookup table processing
*/
esp_err_t mbc_master_get_cid_info(uint16_t cid, const mb_parameter_descriptor_t** param_info);

/**
 * @brief Read parameter from modbus slave device whose name is defined by name and has cid.
 *        The additional data for request is taken from parameter description (lookup) table.
 *
 * @param[in] cid id of the characteristic for parameter
 * @param[in] name pointer into string name (key) of parameter (null terminated)
 * @param[out] value pointer to data buffer of parameter
 * @param[out] type parameter type associated with the name returned from parameter description table.
 *
 * @return
 *     - esp_err_t ESP_OK - request was successful and value buffer contains
 *                          representation of actual parameter data from slave
 *     - esp_err_t ESP_ERR_INVALID_ARG - invalid argument of function or parameter descriptor
 *     - esp_err_t ESP_ERR_INVALID_RESPONSE - an invalid response from slave
 *     - esp_err_t ESP_ERR_INVALID_STATE - invalid state during data processing or allocation failure
 *     - esp_err_t ESP_ERR_TIMEOUT - operation timed out and no response from slave
 *     - esp_err_t ESP_ERR_NOT_SUPPORTED - the request command is not supported by slave
 *     - esp_err_t ESP_ERR_NOT_FOUND - the parameter is not found in the parameter description table
 *     - esp_err_t ESP_FAIL - slave returned an exception or other failure
*/
esp_err_t mbc_master_get_parameter(uint16_t cid, char* name, uint8_t* value, uint8_t *type);

/**
 * @brief Set characteristic's value defined as a name and cid parameter.
 *        The additional data for cid parameter request is taken from master parameter lookup table.
 *
 * @param[in] cid id of the characteristic for parameter
 * @param[in] name pointer into string name (key) of parameter (null terminated)
 * @param[out] value pointer to data buffer of parameter (actual representation of json value field in binary form)
 * @param[out] type pointer to parameter type associated with the name returned from parameter lookup table.
 *
 * @return
 *     - esp_err_t ESP_OK - request was successful and value was saved in the slave device registers
 *     - esp_err_t ESP_ERR_INVALID_ARG - invalid argument of function or parameter descriptor
 *     - esp_err_t ESP_ERR_INVALID_RESPONSE - an invalid response from slave during processing of parameter
 *     - esp_err_t ESP_ERR_INVALID_STATE - invalid state during data processing or allocation failure
 *     - esp_err_t ESP_ERR_TIMEOUT - operation timed out and no response from slave
 *     - esp_err_t ESP_ERR_NOT_SUPPORTED - the request command is not supported by slave
 *     - esp_err_t ESP_FAIL - slave returned an exception or other failure
*/
esp_err_t mbc_master_set_parameter(uint16_t cid, char* name, uint8_t* value, uint8_t *type);

/**
 * @brief The helper function to set data of parameters according to its type
 *
 * @param[in] dest the destination address of the parameter
 * @param[in] src the source address of the parameter
 * @param[out] param_type type of parameter from data dictionary
 * @param[out] param_size the storage size of the characteristic (in bytes).
 *             Describes the size of data to keep into data instance during mapping.
 *
 * @return
 *     - esp_err_t ESP_OK - request was successful and value was saved in the slave device registers
 *     - esp_err_t ESP_ERR_INVALID_ARG - invalid argument of function or parameter descriptor
 *     - esp_err_t ESP_ERR_NOT_SUPPORTED - the request command is not supported by slave
*/
esp_err_t mbc_master_set_param_data(void* dest, void* src, mb_descr_type_t param_type, size_t param_size);

/**
 * @brief The helper function to expose transaction info from modbus layer
 *
 * @param[in] ptinfo the pointer to transaction info structure
 *
 * @return
 *     - esp_err_t ESP_OK - the transaction info is saved in the appropriate parameter structure
 *     - esp_err_t ESP_ERR_INVALID_ARG - invalid argument of function or parameter descriptor
 *     - esp_err_t ESP_ERR_INVALID_STATE - invalid state during data processing or allocation failure
*/
esp_err_t mbc_master_get_transaction_info(mb_trans_info_t *ptinfo);

#ifdef __cplusplus
}
#endif

#endif // _ESP_MB_MASTER_INTERFACE_H
