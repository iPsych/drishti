#!/bin/bash

. ${DRISHTISDK}/bin/build-dev.sh

TOOLCHAIN=ios-10-1-arm64-dep-8-0-hid-sections

if [ -z "${DRISHTISDK}" ]; then
    echo 2>&1 "Must have DRISHTISDK set"
fi

if [ -z "${DRISHTISDK_IOS_IDENTITY}" ]; then
    echo 2>&1 "Must have DRISHTISDK_IOS_IDENTITY set"
fi

EXTRA_ARGS=""
if [ $# -ge 1 ]; then
    EXTRA_ARGS="--reconfig --clear"
fi

DRISHTI_BUILD_C_INTERFACE=OFF
DRISHTI_BUILD_QT=OFF
DRISHTI_BUILD_OGLES_GPGPU=ON
DRISHTI_BUILD_TESTS=ON
DRISHTI_COTIRE=OFF

rename_tab drishti $TOOLCHAIN

COMMAND=(
    "--verbose --fwd "
    "${DRISHTI_BUILD_ARGS[*]} "
    "${DRISHTI_BUILD_HIDE[*]} "
    "CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET=9.0 "
    "DRISHTI_BUILD_QT=${DRISHTI_BUILD_QT} "
    "DRISHTI_BUILD_OGLES_GPGPU=${DRISHTI_BUILD_OGLES_GPGPU} "
    "DRISHTI_BUILD_TESTS=${DRISHTI_BUILD_TESTS} "
    "DRISHTI_COTIRE=${DRISHTI_COTIRE} "
    "DRISHTI_BUILD_C_INTERFACE=${DRISHTI_BUILD_C_INTERFACE} "
    "${DRISHTI_POLLY_ARGS[*]} "    
    "--framework-device "
    "--install "
    "--jobs 8 "
    "--open "
    "--plist \"${DRISHTISDK}/cmake/framework/Info.plist\" "
    "--identity \"${DRISHTISDK_IOS_IDENTITY}\" "    
    "${EXTRA_ARGS} "
    "--test "
)

eval build.py --toolchain ${TOOLCHAIN} ${COMMAND[*]}
