#!/usr/bin/env bash
git clone -b v4.3.1 --recursive https://github.com/espressif/esp-idf.git

IDF_TOOLS_PATH=$(PWD)/espsdk/
./esp-idf/install.sh
. ./esp-idf/export.sh
