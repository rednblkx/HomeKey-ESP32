#!/bin/bash

idf_version=$1
component=$2

lwip=lwip-2.1.2
lwip_uri=http://download.savannah.nongnu.org/releases/lwip
lwip_contrib=contrib-2.1.0

wget --no-verbose ${lwip_uri}/${lwip}.zip
unzip -oq ${lwip}.zip
wget --no-verbose ${lwip_uri}/${lwip_contrib}.zip
unzip -oq ${lwip_contrib}.zip

export LWIP_PATH=`pwd`/$lwip
export LWIP_CONTRIB_PATH=`pwd`/$lwip_contrib
