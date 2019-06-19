# EFM32 emdrv/gpiointerrupt CMake file
# Configures the emdrv/gpiointerrupt and adds it to the build

##### Files #####

# Headers
include_directories(${BASE_LOCATION}/emdrv/gpiointerrupt/inc)

# Source files
FILE(GLOB EFM_GPIOINTERRUPT_SOURCES ${BASE_LOCATION}/emdrv/gpiointerrupt/src/*.c)

##### Outputs #####

# Create emlib library
add_library(gpiointerrupt ${EFM_GPIOINTERRUPT_SOURCES})

# Add library to build
set(LIBS ${LIBS} gpiointerrupt)
