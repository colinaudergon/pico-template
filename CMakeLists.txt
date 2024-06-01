cmake_minimum_required(VERSION 3.16)

include(pico-sdk/pico_sdk_init.cmake)

# Set the project name as a variable
set(PROJECT_NAME PicoSDcard)

project(PicoSDcard)

pico_sdk_init()
#Add sub dir lib/no-os... output compiled to build dir
add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)


include_directories(include) 

add_library(wavReader src/wavReader.cpp) 
target_link_libraries(wavReader pico_stdlib FatFs_SPI)

add_library(audioDriver src/audioDriver.cpp)
# Link libraries
target_link_libraries(audioDriver 
pico_stdlib 
hardware_pwm 
hardware_irq 
hardware_dma 
hardware_clocks 
hardware_structs)

add_library(sevenSegController src/sevenSegController.cpp)
target_link_libraries(sevenSegController pico_stdlib )


# Add digital_user_inputs to project
add_library(digitalUserInputs src/digitalUserInputs.cpp)
target_link_libraries(digitalUserInputs pico_stdlib)


# Add analogue_user_inputs to project
add_library(analogueUserInputs src/analogueUserInputs.cpp)
target_link_libraries(analogueUserInputs pico_stdlib hardware_adc)

add_library(userInterface src/userInterface.cpp)
target_link_libraries(userInterface  pico_stdlib digitalUserInputs analogueUserInputs)

add_library(utils include/utils.hpp)
target_link_libraries(utils pico_stdlib)

add_library(indexLedsManager  src/indexLedsManager.cpp) 
target_link_libraries(indexLedsManager pico_stdlib  )

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_adc wavReader audioDriver sevenSegController userInterface indexLedsManager) 

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Enable USB serial
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)