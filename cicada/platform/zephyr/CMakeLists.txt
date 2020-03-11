# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(../../..)

target_sources(app PRIVATE zephyrserial.cpp)
target_sources(app PRIVATE zephyrtick.cpp)
target_sources(app PRIVATE zephyrirq.cpp)

target_sources(app PRIVATE ../../bufferedserial.cpp)
target_sources(app PRIVATE ../../mqttcountdown.cpp)

target_sources(app PRIVATE ../../commdevices/ipcommdevice.cpp)
target_sources(app PRIVATE ../../commdevices/simcommdevice.cpp)
#target_sources(app PRIVATE ../../commdevices/sim7x00.cpp)
target_sources(app PRIVATE ../../commdevices/sim800.cpp)
target_sources(app PRIVATE ../../commdevices/blockingcommdev.cpp)
