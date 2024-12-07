/*
 * SPDX-FileCopyrightText: 2013 Armink
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2016-2021 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: ESP32 Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: portevent.c v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include <stdatomic.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

#include "mb_m.h"
#include "mbport.h"
#include "mbconfig.h"

#include "port.h"
#include "mbport.h"
#include "freertos/semphr.h"

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED || MB_MASTER_TCP_ENABLED
/* ----------------------- Defines ------------------------------------------*/

// Event bit mask for eMBMasterWaitRequestFinish()
#define MB_EVENT_REQ_MASK   (EventBits_t)( EV_MASTER_PROCESS_SUCCESS | \
                                            EV_MASTER_ERROR_RESPOND_TIMEOUT | \
                                            EV_MASTER_ERROR_RECEIVE_DATA | \
                                            EV_MASTER_ERROR_EXECUTE_FUNCTION )

/* ----------------------- Variables ----------------------------------------*/
static SemaphoreHandle_t xResourceMasterHdl;
static EventGroupHandle_t xEventGroupMasterHdl;
static EventGroupHandle_t xEventGroupMasterConfirmHdl;
static QueueHandle_t xQueueMasterHdl;

static _Atomic uint64_t xTransactionID = 0;

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBMasterPortEventInit( void )
{
    xEventGroupMasterHdl = xEventGroupCreate();
    xEventGroupMasterConfirmHdl = xEventGroupCreate();
    MB_PORT_CHECK((xEventGroupMasterHdl != NULL) && (xEventGroupMasterConfirmHdl != NULL),
                    FALSE, "mb stack event group creation error.");
    xQueueMasterHdl = xQueueCreate(MB_EVENT_QUEUE_SIZE, sizeof(xMBMasterEventType));
    MB_PORT_CHECK(xQueueMasterHdl, FALSE, "mb stack event group creation error.");
    vQueueAddToRegistry(xQueueMasterHdl, "MbMasterPortEventQueue");
    atomic_init(&xTransactionID, 0);
    return TRUE;
}

BOOL MB_PORT_ISR_ATTR
xMBMasterPortEventPost( eMBMasterEventEnum eEvent)
{
    BaseType_t xStatus, xHigherPriorityTaskWoken = pdFALSE;
    assert(xQueueMasterHdl != NULL);
    xMBMasterEventType xEvent;
    xEvent.xPostTimestamp = esp_timer_get_time();
    
    if (eEvent & EV_MASTER_TRANS_START) {
        atomic_store(&(xTransactionID), xEvent.xPostTimestamp);
    }
    xEvent.eEvent = (eEvent & ~EV_MASTER_TRANS_START);

    if( (BOOL)xPortInIsrContext() == TRUE ) {
        xStatus = xQueueSendFromISR(xQueueMasterHdl, (const void*)&xEvent, &xHigherPriorityTaskWoken);
        if ( xHigherPriorityTaskWoken ) {
            portYIELD_FROM_ISR();
        }
        if (xStatus != pdTRUE) {
            ESP_EARLY_LOGV(MB_PORT_TAG, "%s: Post message failure = %d.", __func__, xStatus);
            return FALSE;
        }
    } else {
        xStatus = xQueueSend(xQueueMasterHdl, (const void*)&xEvent, MB_EVENT_QUEUE_TIMEOUT);
        MB_PORT_CHECK((xStatus == pdTRUE), FALSE, "%s: Post message failure.", __func__);
    }
    return TRUE;
}

BOOL
xMBMasterPortEventGet(xMBMasterEventType *peEvent)
{
    assert(xQueueMasterHdl != NULL);
    BOOL xEventHappened = FALSE;

    if (xQueueReceive(xQueueMasterHdl, peEvent, portMAX_DELAY) == pdTRUE) {
        peEvent->xTransactionId = atomic_load(&xTransactionID);
        // Set event bits in confirmation group (for synchronization with port task)
        xEventGroupSetBits(xEventGroupMasterConfirmHdl, peEvent->eEvent);
        peEvent->xGetTimestamp = esp_timer_get_time();
        xEventHappened = TRUE;
    }
    return xEventHappened;
}

eMBMasterEventEnum
xMBMasterPortFsmWaitConfirmation( eMBMasterEventEnum eEventMask, ULONG ulTimeout)
{
    EventBits_t uxBits;
    uxBits = xEventGroupWaitBits( xEventGroupMasterConfirmHdl,  // The event group being tested.
                                    eEventMask,                 // The bits within the event group to wait for.
                                    pdFALSE,                    // Keep masked bits.
                                    pdFALSE,                    // Don't wait for both bits, either bit will do.
                                    ulTimeout);                 // Wait timeout for either bit to be set.
    if (ulTimeout && uxBits) {
        // Clear confirmation events that where set in the mask
        xEventGroupClearBits( xEventGroupMasterConfirmHdl, (uxBits & eEventMask) );
    }
    return (eMBMasterEventEnum)(uxBits & eEventMask);
}

uint64_t xMBMasterPortGetTransactionId( )
{
    return atomic_load(&xTransactionID);
}

// This function is initialize the OS resource for modbus master.
void vMBMasterOsResInit( void )
{
    xResourceMasterHdl = xSemaphoreCreateBinary();
    MB_PORT_CHECK((xResourceMasterHdl != NULL), ; , "%s: Resource create error.", __func__);
}

/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource take result
 */
BOOL xMBMasterRunResTake( LONG lTimeOut )
{
    BaseType_t xStatus = pdTRUE;
    xStatus = xSemaphoreTake( xResourceMasterHdl, lTimeOut );
    MB_PORT_CHECK((xStatus == pdTRUE), FALSE , "%s: Resource take failure.", __func__);
    ESP_LOGD(MB_PORT_TAG,"%s:Take MB resource (%lu ticks).", __func__, lTimeOut);
    return TRUE;
}

/**
 * This function is release Modbus Master running resource.
 * Note:The resource is define by Operating System. If you not use OS this function can be empty.
 */
void vMBMasterRunResRelease( void )
{
    BaseType_t xStatus = pdFALSE;
    xStatus = xSemaphoreGive( xResourceMasterHdl );
    if (xStatus != pdTRUE) {
        ESP_LOGD(MB_PORT_TAG,"%s: Release resource fail.", __func__);
    }
}

/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 *
 * @param xTransId - the identification of the trasaction
 * @param ucDestAddress destination salve address
 * @param pucRecvData current receive data pointer
 * @param ucRecvLength current length of receive buffer
 * @param pucSendData Send buffer data
 * @param ucSendLength Send buffer length
 *
 */
void vMBMasterErrorCBRespondTimeout(uint64_t xTransId, UCHAR ucDestAddress, const UCHAR* pucSendData, USHORT ucSendLength)
{
    (void)xEventGroupSetBits( xEventGroupMasterHdl, EV_MASTER_ERROR_RESPOND_TIMEOUT );
    ESP_LOGD(MB_PORT_TAG,"%s:Callback respond timeout.", __func__);
    if (vMBMasterErrorCBUserHandler) {
        vMBMasterErrorCBUserHandler( xTransId,
                                        (USHORT)EV_ERROR_RESPOND_TIMEOUT, ucDestAddress,
                                        NULL, 0,
                                        pucSendData, ucSendLength );
    }
}

/**
 * This is modbus master receive data error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 *
 * @param xTransId - the identification of the trasaction
 * @param ucDestAddress destination salve address
 * @param pucRecvData current receive data pointer
 * @param ucRecvLength current length of receive buffer
 * @param pucSendData Send buffer data
 * @param ucSendLength Send buffer length
 */
void vMBMasterErrorCBReceiveData(uint64_t xTransId, UCHAR ucDestAddress, 
                                    const UCHAR* pucRecvData, USHORT ucRecvLength, 
                                    const UCHAR* pucSendData, USHORT ucSendLength)
{
    (void)xEventGroupSetBits( xEventGroupMasterHdl, EV_MASTER_ERROR_RECEIVE_DATA );
    ESP_LOGD(MB_PORT_TAG,"%s:Callback receive data failure.", __func__);
    if (vMBMasterErrorCBUserHandler) {
        vMBMasterErrorCBUserHandler( xTransId,
                                        (USHORT)EV_ERROR_RECEIVE_DATA, ucDestAddress,
                                        pucRecvData, ucRecvLength,
                                        pucSendData, ucSendLength );
    }
}

/**
 * This is modbus master execute function error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param xTransId - the identification of the trasaction
 * @param ucDestAddress destination salve address
 * @param pucRecvData current receive data pointer
 * @param ucRecvLength current length of receive buffer
 * @param pucSendData Send buffer data
 * @param ucSendLength Send buffer length
 *
 */
void vMBMasterErrorCBExecuteFunction(uint64_t xTransId, UCHAR ucDestAddress,
                                        const UCHAR* pucRecvData, USHORT ucRecvLength,
                                        const UCHAR* pucSendData, USHORT ucSendLength)
{
    xEventGroupSetBits( xEventGroupMasterHdl, EV_MASTER_ERROR_EXECUTE_FUNCTION );
    ESP_LOGD(MB_PORT_TAG,"%s:Callback execute data handler failure.", __func__);
    if (vMBMasterErrorCBUserHandler) {
        vMBMasterErrorCBUserHandler( xTransId,
                                        (USHORT)EV_ERROR_EXECUTE_FUNCTION, ucDestAddress,
                                        pucRecvData, ucRecvLength,
                                        pucSendData, ucSendLength );
    }
}

/**
 * This is modbus master request process success callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system. Do not execute too much waiting process.
 * 
 * @param xTransId - the identification of the trasaction
 * @param ucDestAddress destination salve address
 * @param pucRecvData current receive data pointer
 * @param ucRecvLength current length of receive buffer
 * @param pucSendData Send buffer data
 * @param ucSendLength Send buffer length
 */
void vMBMasterCBRequestSuccess(uint64_t xTransId, UCHAR ucDestAddress,
                                        const UCHAR* pucRecvData, USHORT ucRecvLength,
                                        const UCHAR* pucSendData, USHORT ucSendLength)
{
    (void)xEventGroupSetBits( xEventGroupMasterHdl, EV_MASTER_PROCESS_SUCCESS );
    ESP_LOGD(MB_PORT_TAG,"%s: Callback request success.", __func__);
    if (vMBMasterErrorCBUserHandler) {
        vMBMasterErrorCBUserHandler( xTransId,
                                        (USHORT)EV_ERROR_OK, ucDestAddress,
                                        pucRecvData, ucRecvLength,
                                        pucSendData, ucSendLength );
    }
}

/**
 * This function is wait for modbus master request finish and return result.
 * Waiting result include request process success, request respond timeout,
 * receive data error and execute function error.You can use the above callback function.
 * @note If you are use OS, you can use OS's event mechanism. Otherwise you have to run
 * much user custom delay for waiting.
 *
 * @return request error code
 */
eMBMasterReqErrCode eMBMasterWaitRequestFinish( void ) {
    eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
    eMBMasterEventEnum xRecvedEvent;

    EventBits_t uxBits = xEventGroupWaitBits( xEventGroupMasterHdl, // The event group being tested.
                                                MB_EVENT_REQ_MASK,  // The bits within the event group to wait for.
                                                pdTRUE,             // Masked bits should be cleared before returning.
                                                pdFALSE,            // Don't wait for both bits, either bit will do.
                                                portMAX_DELAY );    // Wait forever for either bit to be set.
    xRecvedEvent = (eMBMasterEventEnum)(uxBits);
    if (xRecvedEvent) {
        ESP_LOGD(MB_PORT_TAG,"%s: returned event = 0x%x", __func__, (int)xRecvedEvent);
        if (!(xRecvedEvent & MB_EVENT_REQ_MASK)) {
            // if we wait for certain event bits but get from poll subset
            ESP_LOGE(MB_PORT_TAG,"%s: incorrect event set = 0x%x", __func__, (int)xRecvedEvent);
        }
        xEventGroupSetBits( xEventGroupMasterConfirmHdl, (xRecvedEvent & MB_EVENT_REQ_MASK) );
        if (MB_PORT_CHECK_EVENT(xRecvedEvent, EV_MASTER_PROCESS_SUCCESS)) {
            eErrStatus = MB_MRE_NO_ERR;
        } else if (MB_PORT_CHECK_EVENT(xRecvedEvent, EV_MASTER_ERROR_RESPOND_TIMEOUT)) {
            eErrStatus = MB_MRE_TIMEDOUT;
        } else if (MB_PORT_CHECK_EVENT(xRecvedEvent, EV_MASTER_ERROR_RECEIVE_DATA)) {
            eErrStatus = MB_MRE_REV_DATA;
        } else if (MB_PORT_CHECK_EVENT(xRecvedEvent, EV_MASTER_ERROR_EXECUTE_FUNCTION)) {
            eErrStatus = MB_MRE_EXE_FUN;
        }
    } else {
        ESP_LOGE(MB_PORT_TAG,"%s: Incorrect event or timeout xRecvedEvent = 0x%x", __func__, (int)uxBits);
        // https://github.com/espressif/esp-idf/issues/5275
        // if a no event is received, that means vMBMasterPortEventClose()
        // has been closed, so event group has been deleted by FreeRTOS, which
        // triggers the send of 0 value to the event group to unlock this task
        // waiting on it. For this patch, handles it as a time out without assert.
        eErrStatus = MB_MRE_TIMEDOUT;
    }
    return eErrStatus;
}

void vMBMasterPortEventClose(void)
{
    if (xEventGroupMasterHdl) {
        vEventGroupDelete(xEventGroupMasterHdl);
        xEventGroupMasterHdl = NULL;
    }
    if (xQueueMasterHdl) {
        vQueueDelete(xQueueMasterHdl);
        xQueueMasterHdl = NULL;
    }
    if (xEventGroupMasterConfirmHdl) {
        vEventGroupDelete(xEventGroupMasterConfirmHdl);
        xEventGroupMasterConfirmHdl = NULL;
    }
    if (xResourceMasterHdl) {
        vSemaphoreDelete(xResourceMasterHdl);
        xResourceMasterHdl = NULL;
    }
}

#endif
