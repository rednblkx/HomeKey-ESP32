import os
import sys

idf_path = os.getenv('IDF_PATH')
try:

    import nvs_partition_gen as nvs_gen
except ImportError:
    if not idf_path or not os.path.exists(idf_path):
        raise Exception('IDF_PATH not found')
    sys.path.insert(0, os.path.join(idf_path, 'components',
                    'nvs_flash', 'nvs_partition_generator'))
    import nvs_partition_gen as nvs_gen


# @info
#       Generate a custom csv file of encrypted private key parameters
#       when DS is enabled.
#       The csv file is required by the nvs_partition_generator
#       utility to create the nvs partition.
def generate_csv_file_ds(c, iv, hmac_key_id, key_size,
                         device_cert, ca_cert, csv_file):

    with open(csv_file, 'wt', encoding='utf8') as f:
        f.write('# This is a generated csv file containing '
                'required parameters for the Digital Signature operation\n')
        f.write('key,type,encoding,value\nesp_secure_cert,namespace,,\n')

        if ca_cert is not None:
            f.write('ca_cert,file,string,{}\n'.format(ca_cert))
        f.write('cipher_c,data,hex2bin,{}\n'.format(c.hex()))
        f.write('dev_cert,file,string,{}\n'.format(device_cert))
        f.write('rsa_len,data,u16,{}\n'.format(key_size))
        f.write('ds_key_id,data,u8,{}\n'.format(hmac_key_id))
        f.write('iv,data,hex2bin,{}\n'.format(iv.hex()))


# @info
#       Generate a custom csv file of encrypted private key parameters
#       when DS is disabled.
#       The csv file is required by the nvs_partition_generator utility
#       to create the nvs partition.
def generate_csv_file_no_ds(device_cert, ca_cert, priv_key,
                            priv_key_pass, csv_file):

    with open(csv_file, 'wt', encoding='utf8') as f:
        f.write('# This is a generated csv file containing required '
                'parameters for the Digital Signature operation\n')
        f.write('key,type,encoding,value\nesp_secure_cert,namespace,,\n')

        if ca_cert is not None:
            f.write('ca_cert,file,string,{}\n'.format(ca_cert))
        f.write('dev_cert,file,string,{}\n'.format(device_cert))

        if priv_key_pass is not None:
            print('Private key is going to be written'
                  'in password encrypted format.')
            print('If you want to write ptivate key in plaintext,'
                  ' Please remove the password')

        f.write('priv_key,file,string,{}\n'.format(priv_key))


class DefineArgs(object):
    def __init__(self, attributes):
        for key, value in attributes.items():
            self.__setattr__(key, value)


# @info
#       This function uses the nvs_partition_generater utility
#       to generate the nvs partition of the encrypted private key parameters.
def generate_partition(input_filename, output_filename):

    nvs_args = DefineArgs({
        'input': input_filename,
        'outdir': os.getcwd(),
        'output': output_filename,
        'size': hex(0x3000),
        'version': 2,
        'keyfile': None,
    })

    nvs_gen.generate(nvs_args, is_encr_enabled=False, encr_key=None)
