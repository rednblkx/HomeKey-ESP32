# SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# APIs for interpreting and creating protobuf packets for Wi-Fi provisioning

import proto
from utils import hex_str_to_bytes, str_to_bytes


def print_verbose(security_ctx, data):
    if (security_ctx.verbose):
        print(f'\x1b[32;20m++++ {data} ++++\x1b[0m')


def config_get_status_request(network_type, security_ctx):
    # Form protobuf request packet for GetStatus command
    cfg1 = proto.network_config_pb2.NetworkConfigPayload()
    if network_type == 'wifi':
        cfg1.msg = proto.network_config_pb2.TypeCmdGetWifiStatus
        cmd_get_wifi_status = proto.network_config_pb2.CmdGetWifiStatus()
        cfg1.cmd_get_wifi_status.MergeFrom(cmd_get_wifi_status)
    elif network_type == 'thread':
        cfg1.msg = proto.network_config_pb2.TypeCmdGetThreadStatus
        cmd_get_thread_status = proto.network_config_pb2.CmdGetThreadStatus()
        cfg1.cmd_get_thread_status.MergeFrom(cmd_get_thread_status)
    else:
        raise RuntimeError
    encrypted_cfg = security_ctx.encrypt_data(cfg1.SerializeToString())
    print_verbose(security_ctx, f'Client -> Device (Encrypted CmdGetStatus): 0x{encrypted_cfg.hex()}')
    return encrypted_cfg.decode('latin-1')


def config_get_status_response(security_ctx, response_data):
    # Interpret protobuf response packet from GetStatus command
    decrypted_message = security_ctx.decrypt_data(str_to_bytes(response_data))
    cmd_resp1 = proto.network_config_pb2.NetworkConfigPayload()
    cmd_resp1.ParseFromString(decrypted_message)
    print_verbose(security_ctx, f'CmdGetStatus type: {str(cmd_resp1.msg)}')

    if cmd_resp1.msg == proto.network_config_pb2.TypeRespGetWifiStatus:
        print_verbose(security_ctx, f'CmdGetStatus status: {str(cmd_resp1.resp_get_wifi_status.status)}')
        if cmd_resp1.resp_get_wifi_status.wifi_sta_state == 0:
            print('==== WiFi state: Connected ====')
            return 'connected'
        elif cmd_resp1.resp_get_wifi_status.wifi_sta_state == 1:
            print('++++ WiFi state: Connecting... ++++')
            return 'connecting'
        elif cmd_resp1.resp_get_wifi_status.wifi_sta_state == 2:
            print('---- WiFi state: Disconnected ----')
            return 'disconnected'
        elif cmd_resp1.resp_get_wifi_status.wifi_sta_state == 3:
            print('---- WiFi state: Connection Failed ----')
            if cmd_resp1.resp_get_wifi_status.wifi_fail_reason == 0:
                print('---- Failure reason: Incorrect Password ----')
            elif cmd_resp1.resp_get_wifi_status.wifi_fail_reason == 1:
                print('---- Failure reason: Incorrect SSID ----')
            return 'failed'
    elif cmd_resp1.msg == proto.network_config_pb2.TypeRespGetThreadStatus:
        print_verbose(security_ctx, f'CmdGetStatus status: {str(cmd_resp1.resp_get_thread_status.status)}')
        if cmd_resp1.resp_get_thread_status.thread_state == 0:
            print('==== Thread state: Attached ====')
            return 'attached'
        elif cmd_resp1.resp_get_thread_status.thread_state == 1:
            print('==== Thread state: Attaching ====')
            return 'attaching'
        elif cmd_resp1.resp_get_thread_status.thread_state == 2:
            print('==== Thread state: Detached ====')
            return 'detached'
        elif cmd_resp1.resp_get_thread_status.thread_state == 3:
            print('==== Thread state: Attaching Failed ====')
            if cmd_resp1.resp_get_thread_status.thread_fail_reason == 0:
                print('---- Failure reason: Invalid Dataset ----')
            elif cmd_resp1.resp_get_thread_status.thread_fail_reason == 1:
                print('---- Failure reason: Network Not Found ----')
            return 'failed'

    return 'unknown'


def config_set_config_request(network_type, security_ctx, ssid_or_dataset_tlvs, passphrase=''):
    # Form protobuf request packet for SetConfig command
    cmd = proto.network_config_pb2.NetworkConfigPayload()
    if network_type == 'wifi':
        cmd.msg = proto.network_config_pb2.TypeCmdSetWifiConfig
        cmd.cmd_set_wifi_config.ssid = str_to_bytes(ssid_or_dataset_tlvs)
        cmd.cmd_set_wifi_config.passphrase = str_to_bytes(passphrase)
    elif network_type == 'thread':
        cmd.msg = proto.network_config_pb2.TypeCmdSetThreadConfig
        cmd.cmd_set_thread_config.dataset = hex_str_to_bytes(ssid_or_dataset_tlvs)
    else:
        raise RuntimeError
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString())
    print_verbose(security_ctx, f'Client -> Device (SetConfig cmd): 0x{enc_cmd.hex()}')
    return enc_cmd.decode('latin-1')


def config_set_config_response(security_ctx, response_data):
    # Interpret protobuf response packet from SetConfig command
    decrypt = security_ctx.decrypt_data(str_to_bytes(response_data))
    cmd_resp4 = proto.network_config_pb2.NetworkConfigPayload()
    cmd_resp4.ParseFromString(decrypt)
    if cmd_resp4.msg == proto.network_config_pb2.TypeRespSetWifiConfig:
        print_verbose(security_ctx, f'SetConfig status: 0x{str(cmd_resp4.resp_set_wifi_config.status)}')
        return cmd_resp4.resp_set_wifi_config.status
    elif cmd_resp4.msg == proto.network_config_pb2.TypeRespSetThreadConfig:
        print_verbose(security_ctx, f'SetConfig status: 0x{str(cmd_resp4.resp_set_thread_config.status)}')
        return cmd_resp4.resp_set_thread_config.status


def config_apply_config_request(network_type, security_ctx):
    # Form protobuf request packet for ApplyConfig command
    cmd = proto.network_config_pb2.NetworkConfigPayload()
    if network_type == 'wifi':
        cmd.msg = proto.network_config_pb2.TypeCmdApplyWifiConfig
    elif network_type == 'thread':
        cmd.msg = proto.network_config_pb2.TypeCmdApplyThreadConfig
    else:
        raise RuntimeError
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString())
    print_verbose(security_ctx, f'Client -> Device (ApplyConfig cmd): 0x{enc_cmd.hex()}')
    return enc_cmd.decode('latin-1')


def config_apply_config_response(security_ctx, response_data):
    # Interpret protobuf response packet from ApplyConfig command
    decrypt = security_ctx.decrypt_data(str_to_bytes(response_data))
    cmd_resp5 = proto.network_config_pb2.NetworkConfigPayload()
    cmd_resp5.ParseFromString(decrypt)
    if cmd_resp5.msg == proto.network_config_pb2.TypeRespApplyWifiConfig:
        print_verbose(security_ctx, f'ApplyConfig status: 0x{str(cmd_resp5.resp_apply_wifi_config.status)}')
        return cmd_resp5.resp_apply_wifi_config.status
    elif cmd_resp5.msg == proto.network_config_pb2.TypeRespApplyThreadConfig:
        print_verbose(security_ctx, f'ApplyConfig status: 0x{str(cmd_resp5.resp_apply_thread_config.status)}')
        return cmd_resp5.resp_apply_thread_config.status
