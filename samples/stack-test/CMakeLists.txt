###### Project Environment #####

# Set minimum CMake version
cmake_minimum_required(VERSION 2.8.4)

# Set the location of the kernel header files the user links to.
set(LIBK_LOCATION ../../include/libk)

# Set efm32-base project location
set(EFM32_BASE_LOCATION ../../efm32-base)

# Set the compiler (must be prior to project setup)
include(${EFM32_BASE_LOCATION}/toolchain/arm-gcc.cmake)

##### Project Setup #####

# Configure project and languages
project(stack-test C CXX ASM)

# Set device
if(NOT DEVICE)
    set(DEVICE EFM32HG322F64)
endif()

# Set build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

##### Modules #####

# Libraries can be added to the LIBS variable
# or manually included here.

# Add base libs (emlib, CMSIS, device files)
include(${EFM32_BASE_LOCATION}/toolchain/efm32-base.cmake)

add_subdirectory(${EFM32_BASE_LOCATION}/cmsis ../${EFM32_BASE_LOCATION}/cmsis)
add_subdirectory(${EFM32_BASE_LOCATION}/device ../${EFM32_BASE_LOCATION}/device)

include("${CMAKE_CURRENT_LIST_DIR}/../device-compile-definitions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../patch-linker-script.cmake")

# This is normally set in efm32-base.cmake, but libraries may modify it so set
# it after libraries/subdirectories have been added
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_DEFINITIONS} -Xlinker -T${LINKER_SCRIPT} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections -Wl,-v")

##### Files #####

# Add project headers
include_directories(${LIBK_LOCATION}
    ../../targets/Stack/include
    include)

# Add project sources
add_executable(${PROJECT_NAME}
    src/main.c
    ../../targets/Stack/source/svc_device.c)

efm32_configure_linker_addresses(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} ${LIBS} cmsis device)

##### Post build #####

# Add post build commands
include(${EFM32_BASE_LOCATION}/toolchain/post-build.cmake)

##### CMake debug prints #####
if (CMAKE_VERBOSE_MAKEFILE)
    print_debug_info()
endif()
