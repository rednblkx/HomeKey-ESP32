#!/usr/bin/env python
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import sys


def get_install_requires():
    with open('requirements.txt') as f:
        required = f.read().splitlines()
        return required


try:
    from setuptools import find_packages, setup
except ImportError:
    print(
        "Package setuptools is missing from your Python installation. "
        "Please see the installation section in the esp-secure-cert-tool "
        "documentation for instructions on how to install it."
    )
    exit(1)

VERSION = "2.1.0"

long_description = """
====================
esp-secure-cert-tool
====================
The python utility helps to configure and provision the device with
PKI credentials to generate the esp_secure_cert partition.
The utility also configures the DS peripheral on the SoC if available.

The `esp-secure-cert-tool` utility is `hosted on
github <https://github.com/espressif/esp_secure_cert_mgr/
blob/main/tools/configure_esp_secure_cert.py>`_.

Documentation
-------------
Visit online `esp-secure-cert-tool
documentation <https://github.com/espressif/
esp_secure_cert_mgr/blob/main/tools/README.md/>`_ \
or run ``configure_esp_secure_cert.py -h``.

License
-------
The License for the project can be found
`here <https://github.com/espressif/esp_secure_cert_mgr/
blob/main/tools/LICENSE>`_
"""

setup(
    name="esp-secure-cert-tool",
    version=VERSION,
    description="A python utility which helps to configure and provision"
                "the ESP platform with PKI credentials "
                "into the esp_secure_cert partition",
    long_description=long_description,
    long_description_content_type='text/x-rst',
    url="https://github.com/espressif/"
        "esp_secure_cert_mgr/blob/main/tools",
    project_urls={
        "Documentation": "https://github.com/espressif/"
                         "esp_secure_cert_mgr/blob/main/tools/README.md",
        "Source": "https://github.com/espressif/esp_secure_cert_mgr/"
                  "blob/main/tools/configure_esp_secure_cert.py",
    },
    author="Espressif Systems",
    author_email="",
    license="Apache-2.0",
    classifiers=[
        "License :: OSI Approved :: Apache Software License",
        "Intended Audience :: Developers",
        "Natural Language :: English",
        "Operating System :: POSIX",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: MacOS :: MacOS X",
        "Topic :: Software Development :: Embedded Systems",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
    ],
    python_requires=">=3.7",
    setup_requires=(["wheel"] if "bdist_wheel" in sys.argv else []),
    install_requires=get_install_requires(),
    include_package_data=True,
    packages=find_packages(),
    scripts=["configure_esp_secure_cert.py"],
)
