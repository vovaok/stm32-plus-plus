cmake_minimum_required(VERSION 3.20)
project(stm32-plus-plus CXX ASM)

set(CMAKE_CXX_STANDARD 17)

set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/stm32_mw_lwip)

# Turnaround to remove bridgeif_fdb.c from sources
# It is not used but does break a build
file(RENAME ${LWIP_DIR}/src/netif/bridgeif_fdb.c ${LWIP_DIR}/src/netif/__bridgeif_fdb.c)
file(TOUCH  ${LWIP_DIR}/src/netif/bridgeif_fdb.c)

set (LWIP_INCLUDE_DIRS
    "${LWIP_DIR}/src/include"
    "${LWIP_DIR}/system"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ethernet"
)
add_subdirectory(lib/stm32_mw_lwip)

target_include_directories(lwipcore PUBLIC ${LWIP_INCLUDE_DIRS})

include(sources.cmake)

add_library(stm32pp-core ${STM32PP_CORE_SOURCES} ${STM32PP_COMMON_SOURCES})
target_include_directories(stm32pp-core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_library(stm32pp-audio ${STM32PP_AUDIO_SOURCES})
target_link_libraries(stm32pp-audio PUBLIC stm32pp-core)

add_library(stm32pp-canopen ${STM32PP_CANOPEN_SOURCES})
target_link_libraries(stm32pp-canopen PUBLIC stm32pp-core)

add_library(stm32pp-device ${STM32PP_DEVICE_SOURCES})
target_link_libraries(stm32pp-device PUBLIC stm32pp-core)

add_library(stm32pp-ethernet ${STM32PP_ETHERNET_SOURCES})
target_link_libraries(stm32pp-ethernet PUBLIC stm32pp-core lwipcore)

add_library(stm32pp-gfx ${STM32PP_GFX_SOURCES})
target_link_libraries(stm32pp-gfx PUBLIC stm32pp-core)

add_library(stm32pp-modbus ${STM32PP_MODBUS_SOURCES})
target_link_libraries(stm32pp-modbus PUBLIC stm32pp-core lwipcore)

add_library(stm32pp-objnet ${STM32PP_OBJNET_SOURCES})
target_link_libraries(stm32pp-objnet PUBLIC stm32pp-core stm32pp-ethernet)

add_library(stm32pp-radio ${STM32PP_RADIO_SOURCES})
target_link_libraries(stm32pp-radio PUBLIC stm32pp-core)

add_library(stm32pp-usb ${STM32PP_USB_SOURCES})
target_link_libraries(stm32pp-usb PUBLIC stm32pp-core)