cmake_minimum_required(VERSION 3.12)

set(FAMILY rp2040)
#set(BOARD pico_sdk)
set(PICO_BOARD adafruit_feather_rp2040)

# Change directory to the board you are compiling for
set(PORT_DIR "ports/${FAMILY}/")
set(PICO_BOARD_CONFIG_DIR "ports/RP2040/boards/${PICO_BOARD}/")

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(PicoLink C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICOLINK_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

#include(example_auto_set_url.cmake)

file(GLOB picolink_src CONFIGURE_DEPENDS "picolink/*.c")

add_executable(PicoLink ${picolink_src})

target_include_directories(PicoLink PUBLIC picolink)
target_include_directories(PicoLink PUBLIC lib)
target_include_directories(PicoLink PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(PicoLink PUBLIC ${PORT_DIR})
target_include_directories(PicoLink PUBLIC ${PICO_BOARD_CONFIG_DIR})

target_link_libraries(PicoLink PUBLIC pico_stdlib hardware_flash hardware_sync)
target_link_libraries(PicoLink PUBLIC fatfs)
target_link_libraries(PicoLink PUBLIC storage)
target_link_libraries(PicoLink PUBLIC ymodem)
#target_link_libraries(PicoLink PUBLIC shell)
#target_link_libraries(PicoLink PUBLIC utils)
target_link_libraries(PicoLink PUBLIC port)

pico_enable_stdio_usb(PicoLink 1)
pico_enable_stdio_uart(PicoLink 0)
pico_add_extra_outputs(PicoLink)

#add_subdirectory(PicoLink)
add_subdirectory(lib/fatfs)
add_subdirectory(lib/storage)
add_subdirectory(lib/ymodem)
#add_subdirectory(lib/shell)
#add_subdirectory(lib/utils)
add_subdirectory(${PORT_DIR})

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )
