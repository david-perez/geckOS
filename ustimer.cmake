# EFM32 emdrv/ustimer CMake file
# Configures the emdrv/ustimer and adds it to the build

##### Files #####

# Headers
include_directories(${BASE_LOCATION}/emdrv/common/inc)
include_directories(${BASE_LOCATION}/emdrv/ustimer/inc)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/config)

# Source files
FILE(GLOB EFM_USTIMER_SOURCES ${BASE_LOCATION}/emdrv/ustimer/src/*.c)

##### Outputs #####

# Create emlib library
add_library(ustimer ${EFM_USTIMER_SOURCES})

# Add library to build
set(LIBS ${LIBS} ustimer)
