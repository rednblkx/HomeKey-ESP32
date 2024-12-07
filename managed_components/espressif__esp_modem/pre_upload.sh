#!/bin/bash

apt-get update
apt-get -y install doxygen clang python3-pip
python -m pip install breathe recommonmark
pushd components/esp_modem/docs
./generate_docs
popd
