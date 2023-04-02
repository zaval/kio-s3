#!/bin/bash

# Exit if something fails
set -e

mkdir -p build
cd build

cmake \
    -DKDE_INSTALL_PLUGINDIR=`kf5-config --qt-plugins` \
    -DKDE_INSTALL_KSERVICESDIR=`kf5-config --install services` \
    -DKDE_INSTALL_LOGGINGCATEGORIESDIR=`kf5-config --install data`qlogging-categories5 \
    -DCMAKE_BUILD_TYPE=Debug  .. \
    -DAWSSDK_DIR=/home/zaval/projects/aws-sdk-cpp/lib/cmake/AWSSDK \
    -DCMAKE_PREFIX_PATH=/home/zaval/projects/aws-sdk-cpp
make -j$(nproc)
sudo make install

# start dolphin to browse the new IO system
#dolphin s3:///
