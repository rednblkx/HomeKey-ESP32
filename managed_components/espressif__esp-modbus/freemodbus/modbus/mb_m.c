/*
 * SPDX-FileCopyrightText: 2013 Armink
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2016-2021 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbrtu_m.c,v 1.60 2013/08/20 11:18:10 Armink Add Master Functions $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"

#include "mbport.h"
#if MB_MASTER_RTU_ENABLED
#include "mbrtu.h"
#endif
#if MB_MASTER_ASCII_ENABLED
#include "mbascii.h"
#endif
#if MB_MASTER_TCP_ENABLED
#include "mbtcp.h"
#include "mbtcp_m.h"
#endif

#if MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED || MB_MASTER_TCP_ENABLED

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 1
#endif

/*------------------------ Shared variables ---------------------------------*/

_lock_t xMBMLock; // base modbus object lock
volatile UCHAR ucMasterSndBuf[MB_SERIAL_BUF_SIZE];
volatile UCHAR ucMasterRcvBuf[MB_SERIAL_BUF_SIZE];

static _Atomic USHORT usMasterSendPDULength = 0;
static _Atomic eMBMasterErrorEventType eMBMasterCurErrorType = EV_ERROR_INIT;
static _Atomic BOOL xMBRunInMasterMode = FALSE;
static _Atomic UCHAR ucMBMasterDestAddress = 0;
static _Atomic BOOL xFrameIsBroadcast = FALSE;

static _Atomic eMBMasterTimerMode eMasterCurTimerMode;

/* ----------------------- Static variables ---------------------------------*/
static uint64_t xCurTransactionId = 0;
static UCHAR *pucMBSendFrame = NULL;
static UCHAR *pucMBRecvFrame = NULL;
static UCHAR ucRecvAddress = 0;
static eMBMode eMBMasterCurrentMode;

/* The transaction information structure which keep last processing state */
static TransactionInfo_t xTransactionInfo = {0};

static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 * Using for Modbus Master,Add by Armink 20130813
 */
static peMBFrameSend peMBMasterFrameSendCur;
static pvMBFrameStart pvMBMasterFrameStartCur;
static pvMBFrameStop pvMBMasterFrameStopCur;
static peMBFrameReceive peMBMasterFrameReceiveCur;
static pvMBFrameClose pvMBMasterFrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 * Using for Modbus Master,Add by Armink 20130813
 */
BOOL( *pxMBMasterFrameCBByteReceived ) ( void );

BOOL( *pxMBMasterFrameCBTransmitterEmpty ) ( void );

BOOL( *pxMBMasterPortCBTimerExpired ) ( void );

BOOL( *pxMBMasterFrameCBReceiveFSMCur ) ( void );

BOOL( *pxMBMasterFrameCBTransmitFSMCur ) ( void );

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xMasterFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBMasterFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBMasterFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBMasterFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBMasterFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBMasterFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBMasterFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBMasterFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBMasterFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBMasterFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBMasterFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/
#if MB_MASTER_TCP_ENABLED
eMBErrorCode
eMBMasterTCPInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( ( eStatus = eMBMasterTCPDoInit( ucTCPPort ) ) != MB_ENOERR ) {
        eMBState = STATE_DISABLED;
    }
    else if( !xMBMasterPortEventInit(  ) ) {
        /* Port dependent event module initialization failed. */
        eStatus = MB_EPORTERR;
    } else {
        pvMBMasterFrameStartCur = eMBMasterTCPStart;
        pvMBMasterFrameStopCur = eMBMasterTCPStop;
        peMBMasterFrameReceiveCur = eMBMasterTCPReceive;
        peMBMasterFrameSendCur = eMBMasterTCPSend;
        pxMBMasterPortCBTimerExpired = xMBMasterTCPTimerExpired;
        pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterTCPPortClose : NULL;
        eMBMasterCurrentMode = MB_TCP;
        eMBState = STATE_DISABLED;
        ucRecvAddress = MB_TCP_PSEUDO_ADDRESS;
        xCurTransactionId = 0;
        xTransactionInfo.xTransId = 0;
        xTransactionInfo.ucDestAddr = 0;
        xTransactionInfo.ucFuncCode = 0;
        xTransactionInfo.eException = MB_EX_NONE;
        xTransactionInfo.ucFrameError = 0;
        
        /* initialize the state values. */
        atomic_init(&usMasterSendPDULength, 0);
        atomic_init(&eMBMasterCurErrorType, EV_ERROR_INIT);
        atomic_init(&xMBRunInMasterMode, FALSE);
        atomic_init(&ucMBMasterDestAddress, MB_TCP_PSEUDO_ADDRESS);

        // initialize the OS resource for modbus master.
        vMBMasterOsResInit();
        if (xMBMasterPortTimersInit(MB_MASTER_TIMEOUT_MS_RESPOND * MB_TIMER_TICS_PER_MS) != TRUE)
        {
            eStatus = MB_EPORTERR;
        }

    }
    return eStatus;
}
#endif

eMBErrorCode
eMBMasterSerialInit( eMBMode eMode, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    switch (eMode)
    {
#if MB_MASTER_RTU_ENABLED > 0
    case MB_RTU:
        pvMBMasterFrameStartCur = eMBMasterRTUStart;
        pvMBMasterFrameStopCur = eMBMasterRTUStop;
        peMBMasterFrameSendCur = eMBMasterRTUSend;
        peMBMasterFrameReceiveCur = eMBMasterRTUReceive;
        pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;
        pxMBMasterFrameCBByteReceived = xMBMasterRTUReceiveFSM;
        pxMBMasterFrameCBTransmitterEmpty = xMBMasterRTUTransmitFSM;
        pxMBMasterPortCBTimerExpired = xMBMasterRTUTimerExpired;
        eMBMasterCurrentMode = eMode;

        eStatus = eMBMasterRTUInit(ucPort, ulBaudRate, eParity);
        break;
#endif
#if MB_MASTER_ASCII_ENABLED > 0
    case MB_ASCII:
        pvMBMasterFrameStartCur = eMBMasterASCIIStart;
        pvMBMasterFrameStopCur = eMBMasterASCIIStop;
        peMBMasterFrameSendCur = eMBMasterASCIISend;
        peMBMasterFrameReceiveCur = eMBMasterASCIIReceive;
        pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;
        pxMBMasterFrameCBByteReceived = xMBMasterASCIIReceiveFSM;
        pxMBMasterFrameCBTransmitterEmpty = xMBMasterASCIITransmitFSM;
        pxMBMasterPortCBTimerExpired = xMBMasterASCIITimerT1SExpired;
        eMBMasterCurrentMode = eMode;

        eStatus = eMBMasterASCIIInit(ucPort, ulBaudRate, eParity );
        break;
#endif
    default:
        eStatus = MB_EINVAL;
        break;
    }

    if (eStatus == MB_ENOERR)
    {
        if (!xMBMasterPortEventInit())
        {
            /* port dependent event module initalization failed. */
            eStatus = MB_EPORTERR;
        }
        else
        {
            eMBState = STATE_DISABLED;
            ucRecvAddress = 0;
            xCurTransactionId = 0;
            xTransactionInfo.xTransId = 0;
            xTransactionInfo.ucDestAddr = 0;
            xTransactionInfo.ucFuncCode = 0;
            xTransactionInfo.eException = MB_EX_NONE;
            xTransactionInfo.ucFrameError = 0;
            
            /* initialize the state values. */
            atomic_init(&usMasterSendPDULength, 0);
            atomic_init(&eMBMasterCurErrorType, EV_ERROR_INIT);
            atomic_init(&xMBRunInMasterMode, FALSE);
            atomic_init(&ucMBMasterDestAddress, 0);
        }
        /* initialize the OS resource for modbus master. */
        vMBMasterOsResInit();
    }
    return eStatus;
}

eMBErrorCode
eMBMasterClose( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        if( pvMBMasterFrameCloseCur != NULL )
        {
            pvMBMasterFrameCloseCur(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterEnable( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBMasterFrameStartCur(  );
        /* Release the resource, because it created in busy state */
        //vMBMasterRunResRelease( );
        eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterDisable( void )
{
    eMBErrorCode    eStatus;

    if( eMBState == STATE_ENABLED )
    {
        pvMBMasterFrameStopCur(  );
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterPoll( void )
{
    int                     i;
    int                     j;
    eMBErrorCode            eStatus = MB_ENOERR;
    xMBMasterEventType      xEvent;
    eMBMasterErrorEventType errorType = EV_ERROR_INIT;
    static eMBException     eException = MB_EX_NONE;
    static UCHAR            ucFunctionCode = 0;
    static USHORT           usRecvLength = 0;

    /* Check if the protocol stack is ready. */
    if( eMBState != STATE_ENABLED ) {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if ( xMBMasterPortEventGet( &xEvent ) == TRUE ) {
        switch( xEvent.eEvent ) {
            // In some cases it is possible that more than one event set
            // together (even from one subset mask) than process them consistently
            case EV_MASTER_READY:
                ESP_LOGD(MB_PORT_TAG, "%" PRIu64 ":EV_MASTER_READY", xEvent.xTransactionId);
                vMBMasterSetErrorType( EV_ERROR_INIT );
                vMBMasterRunResRelease( );
                break;
            case EV_MASTER_FRAME_TRANSMIT:
                ESP_LOGD(MB_PORT_TAG, "%" PRIu64 ":EV_MASTER_FRAME_TRANSMIT", xEvent.xTransactionId);
                /* Master is busy now. */
                vMBMasterGetPDUSndBuf( &pucMBSendFrame );
                ESP_LOG_BUFFER_HEX_LEVEL("POLL transmit buffer", (void*)pucMBSendFrame, usMBMasterGetPDUSndLength(), ESP_LOG_DEBUG);
                eStatus = peMBMasterFrameSendCur( ucMBMasterGetDestAddress(), pucMBSendFrame, usMBMasterGetPDUSndLength() );
                if (eStatus != MB_ENOERR) {
                    vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
                    ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                    ESP_LOGE( MB_PORT_TAG, "%" PRIu64 ":Frame send error = %d", xEvent.xTransactionId, (unsigned)eStatus );
                }
                xCurTransactionId = xEvent.xTransactionId;
                break;
            case EV_MASTER_FRAME_SENT:
                if (xCurTransactionId == xEvent.xTransactionId) {
                    ESP_LOGD( MB_PORT_TAG, "%" PRIu64 ":EV_MASTER_FRAME_SENT", xEvent.xTransactionId );
                    ESP_LOG_BUFFER_HEX_LEVEL("POLL sent buffer", (void*)pucMBSendFrame, usMBMasterGetPDUSndLength(), ESP_LOG_DEBUG);
                }
                break;
            case EV_MASTER_FRAME_RECEIVED:
                ESP_LOGD( MB_PORT_TAG, "%" PRIu64 ":EV_MASTER_FRAME_RECEIVED", xEvent.xTransactionId );
                eStatus = peMBMasterFrameReceiveCur( &ucRecvAddress, &pucMBRecvFrame, &usRecvLength);
                if (xCurTransactionId == xEvent.xTransactionId) {
                    MB_PORT_CHECK(pucMBSendFrame, MB_EILLSTATE, "Send buffer initialization fail.");
                    // Check if the frame is for us. If not ,send an error process event.
                    if ( ( eStatus == MB_ENOERR ) && ( ( ucRecvAddress == ucMBMasterGetDestAddress() )
                                                    || ( ucRecvAddress == MB_TCP_PSEUDO_ADDRESS) ) ) {
                        if ( ( pucMBRecvFrame[MB_PDU_FUNC_OFF]  & ~MB_FUNC_ERROR ) == ( pucMBSendFrame[MB_PDU_FUNC_OFF] ) ) {
                            ESP_LOGD(MB_PORT_TAG, "%" PRIu64 ": Packet data received successfully (%u).", xEvent.xTransactionId, (unsigned)eStatus);
                            ESP_LOG_BUFFER_HEX_LEVEL("POLL receive buffer", (void*)pucMBRecvFrame, (uint16_t)usRecvLength, ESP_LOG_DEBUG);
                            ( void ) xMBMasterPortEventPost( EV_MASTER_EXECUTE );
                        } else {
                            ESP_LOGE( MB_PORT_TAG, "Drop incorrect frame, receive_func(%u) != send_func(%u)",
                                            pucMBRecvFrame[MB_PDU_FUNC_OFF], pucMBSendFrame[MB_PDU_FUNC_OFF]);
                            vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
                            ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                        }
                    } else {
                        vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
                        ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                        ESP_LOGD( MB_PORT_TAG, "%" PRIu64 ": Packet data receive failed (addr=%u)(%u).",
                                               xEvent.xTransactionId, (unsigned)ucRecvAddress, (unsigned)eStatus);
                    }
                } else {
                    // Ignore the `EV_MASTER_FRAME_RECEIVED` event because the respond timeout occurred
                    // and this is likely respond to previous transaction
                    ESP_LOGE( MB_PORT_TAG, "Drop data received outside of transaction (%" PRIu64 ")", xEvent.xTransactionId );
                }
                break;
            case EV_MASTER_EXECUTE:
                if (xCurTransactionId == xEvent.xTransactionId) {
                    if ( xMBMasterRequestIsBroadcast() 
                         && (( ucMBMasterGetCommMode() == MB_RTU ) || ( ucMBMasterGetCommMode() == MB_ASCII ) ) ) {
                        pucMBRecvFrame = pucMBSendFrame;
                    }
                    MB_PORT_CHECK(pucMBRecvFrame, MB_EILLSTATE, "receive buffer initialization fail.");
                    ESP_LOGD(MB_PORT_TAG, "%" PRIu64 ":EV_MASTER_EXECUTE", xEvent.xTransactionId);
                    ucFunctionCode = pucMBRecvFrame[MB_PDU_FUNC_OFF];
                    eException = MB_EX_ILLEGAL_FUNCTION;
                    /* If receive frame has exception. The receive function code highest bit is 1.*/
                    if (ucFunctionCode & MB_FUNC_ERROR) {
                        eException = (eMBException)pucMBRecvFrame[MB_PDU_DATA_OFF];
                    } else {
                        for ( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
                        {
                            /* No more function handlers registered. Abort. */
                            if (xMasterFuncHandlers[i].ucFunctionCode == 0) {
                                break;
                            }
                            if (xMasterFuncHandlers[i].ucFunctionCode == ucFunctionCode) {
                                vMBMasterSetCBRunInMasterMode(TRUE);
                                /* If master request is broadcast,
                                * the master need execute function for all slave.
                                */
                                if ( xMBMasterRequestIsBroadcast() ) {
                                    USHORT usLength = usMBMasterGetPDUSndLength();
                                    for(j = 1; j <= MB_MASTER_TOTAL_SLAVE_NUM; j++)
                                    {
                                        vMBMasterSetDestAddress(j);
                                        eException = xMasterFuncHandlers[i].pxHandler(pucMBRecvFrame, &usLength);
                                    }
                                } else {
                                    eException = xMasterFuncHandlers[i].pxHandler(pucMBRecvFrame, &usRecvLength);
                                }
                                vMBMasterSetCBRunInMasterMode( FALSE );
                                break;
                            }
                        }
                    }
                    /* If master has exception, will send error process event. Otherwise the master is idle.*/
                    if ( eException != MB_EX_NONE ) {
                        vMBMasterSetErrorType( EV_ERROR_EXECUTE_FUNCTION );
                        ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                    } else {
                        if ( eMBMasterGetErrorType( ) == EV_ERROR_INIT ) {
                            vMBMasterSetErrorType(EV_ERROR_OK);
                            ESP_LOGD( MB_PORT_TAG, "%" PRIu64 ":set event EV_ERROR_OK", xEvent.xTransactionId );
                            ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                        }
                    }
                } else {
                    ESP_LOGD( MB_PORT_TAG, "%" PRIu64 ":EV_MASTER_EXECUTE is expired", xEvent.xTransactionId );
                }
                break;
            case EV_MASTER_ERROR_PROCESS:
                if (xCurTransactionId == xEvent.xTransactionId) {
                    ESP_LOGD( MB_PORT_TAG, "%" PRIu64 ":EV_MASTER_ERROR_PROCESS", xEvent.xTransactionId);
                    /* Execute specified error process callback function. */
                    errorType = eMBMasterGetErrorType( );
                    vMBMasterGetPDUSndBuf( &pucMBSendFrame );
                    switch ( errorType )
                    {
                        case EV_ERROR_RESPOND_TIMEOUT:
                            vMBMasterErrorCBRespondTimeout( xEvent.xTransactionId,
                                                            ucMBMasterGetDestAddress( ),
                                                            pucMBSendFrame, usMBMasterGetPDUSndLength( ) );
                            break;
                        case EV_ERROR_RECEIVE_DATA:
                            vMBMasterErrorCBReceiveData( xEvent.xTransactionId,
                                                            ucMBMasterGetDestAddress( ),
                                                            pucMBRecvFrame, usRecvLength,
                                                            pucMBSendFrame, usMBMasterGetPDUSndLength( ) );
                            break;
                        case EV_ERROR_EXECUTE_FUNCTION:
                            vMBMasterErrorCBExecuteFunction( xEvent.xTransactionId,
                                                            ucMBMasterGetDestAddress( ),
                                                            pucMBRecvFrame, usRecvLength,
                                                            pucMBSendFrame, usMBMasterGetPDUSndLength( ) );
                            break;
                        case EV_ERROR_OK:
                            vMBMasterCBRequestSuccess( xEvent.xTransactionId,
                                                        ucMBMasterGetDestAddress( ),
                                                        pucMBRecvFrame, usRecvLength,
                                                        pucMBSendFrame, usMBMasterGetPDUSndLength( ) );
                            break;
                        default:
                            ESP_LOGE( MB_PORT_TAG, "%" PRIu64 ":incorrect error type = %d.", xEvent.xTransactionId, (int)errorType);
                            break;
                    }
                }
                vMBMasterPortTimersDisable( );
                uint64_t xProcTime = xCurTransactionId ? ( xEvent.xPostTimestamp - xCurTransactionId ) : 0;
                ESP_LOGD( MB_PORT_TAG, "Transaction (%" PRIu64 "), processing time(us) = %" PRId64, xCurTransactionId, xProcTime );
                MB_ATOMIC_SECTION {
                    xTransactionInfo.xTransId = xCurTransactionId;
                    xTransactionInfo.ucDestAddr = ucMBMasterGetDestAddress();
                    xTransactionInfo.ucFuncCode = ucFunctionCode;
                    xTransactionInfo.eException = eException;
                    xTransactionInfo.ucFrameError = errorType;
                }
                xCurTransactionId = 0;
                vMBMasterSetErrorType( EV_ERROR_INIT );
                vMBMasterRunResRelease( );
                break;
            default:
                ESP_LOGE( MB_PORT_TAG, "%" PRIu64 ":Unexpected event triggered 0x%02x.", xEvent.xTransactionId, (int)xEvent.eEvent );
                break;
        }
    } else {
        // Something went wrong and task unblocked but there are no any correct events set
        ESP_LOGE( MB_PORT_TAG, "%" PRIu64 ": Unexpected event triggered 0x%02x.", xEvent.xTransactionId, (int)xEvent.eEvent );
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

// Get whether the Modbus Master is run in master mode.
BOOL xMBMasterGetCBRunInMasterMode( void )
{
    return atomic_load(&xMBRunInMasterMode);
}

// Set whether the Modbus Master is run in master mode.
void vMBMasterSetCBRunInMasterMode( BOOL IsMasterMode )
{
    atomic_store(&xMBRunInMasterMode, IsMasterMode);
}

// Get Modbus Master send destination address.
UCHAR ucMBMasterGetDestAddress( void )
{
    return atomic_load(&ucMBMasterDestAddress);
}

// Set Modbus Master send destination address.
void vMBMasterSetDestAddress( UCHAR Address )
{
    atomic_store(&ucMBMasterDestAddress, Address);
}

// Get Modbus Master current error event type.
eMBMasterErrorEventType inline eMBMasterGetErrorType( void )
{
    return atomic_load(&eMBMasterCurErrorType);
}

// Set Modbus Master current error event type.
void IRAM_ATTR vMBMasterSetErrorType( eMBMasterErrorEventType errorType )
{
    atomic_store(&eMBMasterCurErrorType, errorType);
}

/* Get Modbus Master send PDU's buffer address pointer.*/
void vMBMasterGetPDUSndBuf( UCHAR ** pucFrame )
{
    *pucFrame = ( UCHAR * ) &ucMasterSndBuf[MB_SEND_BUF_PDU_OFF];
}

/* Set Modbus Master send PDU's buffer length.*/
void vMBMasterSetPDUSndLength( USHORT SendPDULength )
{
    atomic_store(&usMasterSendPDULength, SendPDULength);
}

/* Get Modbus Master send PDU's buffer length.*/
USHORT usMBMasterGetPDUSndLength( void )
{
    return atomic_load(&usMasterSendPDULength);
}

/* Set Modbus Master current timer mode.*/
void vMBMasterSetCurTimerMode( eMBMasterTimerMode eMBTimerMode )
{
    atomic_store(&eMasterCurTimerMode, eMBTimerMode);
}

/* Get Modbus Master current timer mode.*/
eMBMasterTimerMode MB_PORT_ISR_ATTR xMBMasterGetCurTimerMode( void )
{
    return atomic_load(&eMasterCurTimerMode);
}

/* The master request is broadcast? */
BOOL MB_PORT_ISR_ATTR xMBMasterRequestIsBroadcast( void )
{
    return atomic_load(&xFrameIsBroadcast);
}

/* The master request is broadcast? */
void vMBMasterRequestSetType( BOOL xIsBroadcast )
{
    atomic_store(&xFrameIsBroadcast, xIsBroadcast);
}

// Get Modbus Master communication mode.
eMBMode ucMBMasterGetCommMode(void)
{
    return eMBMasterCurrentMode;
}

/* Get current transaction information */
BOOL xMBMasterGetLastTransactionInfo( uint64_t *pxTransId, UCHAR *pucDestAddress,
                                        UCHAR *pucFunctionCode, UCHAR *pucException,
                                        USHORT *pusErrorType )
{
    BOOL xState = (eMBState == STATE_ENABLED);
    if (xState && pxTransId && pucDestAddress && pucFunctionCode
            && pucException && pusErrorType) {
        MB_ATOMIC_SECTION {
            *pxTransId = xTransactionInfo.xTransId;
            *pucDestAddress = xTransactionInfo.ucDestAddr;
            *pucFunctionCode = xTransactionInfo.ucFuncCode;
            *pucException =  xTransactionInfo.eException;
            *pusErrorType = xTransactionInfo.ucFrameError;
        }
    }
    return xState;
}

#endif // MB_MASTER_RTU_ENABLED || MB_MASTER_ASCII_ENABLED || MB_MASTER_TCP_ENABLED
