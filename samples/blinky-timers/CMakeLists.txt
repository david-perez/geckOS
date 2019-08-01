###### Project Environment #####

# Set minimum CMake version
cmake_minimum_required(VERSION 2.8.4)

# Set the location of the kernel header files the user links to.
set(LIBK_LOCATION ../../include/libk)

# Set efm32-base project location
set(EFM_32_BASE_LOCATION ../../efm32-base)

# Set the compiler (must be prior to project setup)
include(${EFM_32_BASE_LOCATION}/toolchain/arm-gcc.cmake)

##### Project Setup #####

# Set our output target
set(TARGET blinky-timers)

# Configure project and languages
project(${TARGET} C CXX ASM)

# Set device
if(NOT DEVICE)
set(DEVICE EFM32HG322F64)
endif()

# Set build
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE DEBUG)
endif()

##### Compile flags and device #####
include(../efm32-base.cmake)

##### Files #####

# Add project headers
include_directories(./include ${LIBK_LOCATION} ../../targets/efm32hg_slstk3400a/include)

# Add project sources
set(SOURCES
    src/main.c
    ../../targets/efm32hg_slstk3400a/source/svc_device.c)

##### Outputs #####

# Generate executable and link
add_executable(${TARGET} ${SOURCES})

if(FLASH_ORIGIN)
target_link_options(${TARGET}
    PRIVATE "LINKER:--defsym=flash_origin=${FLASH_ORIGIN}"
)
endif()

if(FLASH_LENGTH)
target_link_options(${TARGET}
    PRIVATE "LINKER:--defsym=flash_length=${FLASH_LENGTH}"
)
endif()

if(RAM_ORIGIN)
target_link_options(${TARGET}
    PRIVATE "LINKER:--defsym=ram_origin=${RAM_ORIGIN}"
)
endif()

if(RAM_LENGTH)
target_link_options(${TARGET}
    PRIVATE "LINKER:--defsym=ram_length=${RAM_LENGTH}"
)
endif()

target_link_libraries(${TARGET} ${LIBS})

##### Post build #####

# Add post build commands
include(${EFM_32_BASE_LOCATION}/toolchain/post-build.cmake)
