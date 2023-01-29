cmake_minimum_required(VERSION 3.12)

set(FAMILY rp2040)
#set(BOARD pico_sdk)
set(PICO_BOARD adafruit_feather_rp2040)

# Change directory to the board you are compiling for
set(PORT_DIR "ports/${FAMILY}/")
set(PICO_BOARD_CONFIG_DIR "ports/RP2040/boards/${PICO_BOARD}/")

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(MotorDrive C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

#include(example_auto_set_url.cmake)

file(GLOB motordrive_src CONFIGURE_DEPENDS "motordrive/*.cpp")
#file(GLOB motordrive_src CONFIGURE_DEPENDS "motordrive/*.c")

add_executable(MotorDrive ${motordrive_src})

target_include_directories(MotorDrive PUBLIC motordrive)
target_include_directories(MotorDrive PUBLIC lib)
target_include_directories(MotorDrive PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(MotorDrive PUBLIC pico_stdlib hardware_flash hardware_sync hardware_i2c hardware_adc hardware_pwm)
target_link_libraries(MotorDrive PUBLIC fatfs)
target_link_libraries(MotorDrive PUBLIC storage)
target_link_libraries(MotorDrive PUBLIC interface)
target_link_libraries(MotorDrive PUBLIC components)
target_link_libraries(MotorDrive PUBLIC console)
target_link_libraries(MotorDrive PUBLIC shell)
target_link_libraries(MotorDrive PUBLIC register)

pico_enable_stdio_usb(MotorDrive 1)
pico_enable_stdio_uart(MotorDrive 0)
pico_add_extra_outputs(MotorDrive)

add_subdirectory(lib/fatfs)
add_subdirectory(lib/storage)
add_subdirectory(lib/interface)
add_subdirectory(lib/components)
add_subdirectory(lib/console)
add_subdirectory(lib/shell)
add_subdirectory(lib/register)

#-mno-unaligned-access
add_compile_options(-Wall
                    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
                    -Wno-unused-function # we have some for the docs that aren't called
                    -Wno-maybe-uninitialized
                    -mno-unaligned-access
                    )
