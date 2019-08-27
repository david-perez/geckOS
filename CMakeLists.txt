###### Project Environment #####

# Set minimum CMake version.
cmake_minimum_required(VERSION 2.8.4)

# Set efm32-base project location.
set(BASE_LOCATION ${CMAKE_CURRENT_LIST_DIR}/efm32-base)

# Set the compiler (must be prior to project setup).
include(${BASE_LOCATION}/toolchain/arm-gcc.cmake)

##### Project Setup #####

# Set our output target
set(TARGET geckOS)

# Configure project and languages
project(${TARGET} C CXX ASM)

# Set MCU
if(NOT DEVICE)
    set(DEVICE EFM32HG322F64)
    # set(DEVICE EFM32WG380F256)
endif()

# Set build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

# Set board
if(NOT TARGET_DEVICE)
    # set(TARGET_DEVICE "efm32hg_slstk3400a")
    # set(TARGET_DEVICE "AudioMoth")
    set(TARGET_DEVICE "Stack")
endif()

##### Modules #####

# Libraries can be added to the LIBS variable
# or manually included here.

# Add base libs (emlib, CMSIS, device files)
include(${BASE_LOCATION}/toolchain/efm32-base.cmake)

add_subdirectory(${BASE_LOCATION}/cmsis)
add_subdirectory(${BASE_LOCATION}/emlib)
add_subdirectory(${BASE_LOCATION}/device)
add_subdirectory(${BASE_LOCATION}/protocol)
add_subdirectory(${BASE_LOCATION}/hardware)

add_subdirectory(${BASE_LOCATION}/emdrv/gpiointerrupt)

include(usb.cmake)
include(ustimer.cmake)

# This is normally set in efm32-base.cmake, but libraries may modify it so set
# it after libraries/subdirectories have been added.
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_DEFINITIONS} -Xlinker -T${LINKER_SCRIPT} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections -Wl,-v")

##### Files #####

# Add project headers.
include_directories(include include/libk)

# Add device-specific headers.
include_directories(targets/${TARGET_DEVICE}/include)

# Add device-specific sources.
# We need to add different sources depending on the target.
set(DEVICE_SPECIFIC_SOURCES
    # Everything except svc_device.c should be put here.
    targets/${TARGET_DEVICE}/source/svc_dispatcher_device.c
    targets/${TARGET_DEVICE}/source/svc_handlers_device.c
    targets/${TARGET_DEVICE}/source/device_init.c)

include(${CMAKE_CURRENT_LIST_DIR}/targets/${TARGET_DEVICE}/device-specific-sources.cmake)

# Add geckOS device-independent sources.
set(SOURCES
    source/main.c
    source/msddmedia.c
    source/msdd.c
    source/descriptors.c
    source/fat.c
    source/svc_dispatcher.c
    source/svc_handlers.c
    source/jump_to_address.s
    source/kernel.c
    source/bootloader.c
    source/timer.c
    source/timeout.c
    ${DEVICE_SPECIFIC_SOURCES})

##### Outputs #####

# Generate executable and link
add_executable(${TARGET} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PUBLIC VOLUME_LABEL="${TARGET_DEVICE}")

efm32_configure_linker_addresses(${PROJECT_NAME})

# TODO We should maybe not link everything here. This depends on the device.
# Although I guess that everything that is not needed will be stripped out in
# the release version, so it doesn't hurt.
target_link_libraries(${PROJECT_NAME} ${LIBS} emlib cmsis device gpiointerrupt ustimer emusb)

# Link optional libraries if available.
if (${HAS_HARDWARE})
    target_link_libraries(${PROJECT_NAME} hardware)
endif ()

if (${HAS_PROTOCOL})
    target_link_libraries(${PROJECT_NAME} protocol)
endif ()

##### Post build #####

# Add post build commands.
include(${BASE_LOCATION}/toolchain/post-build.cmake)

# Add JLink commands.
include(${BASE_LOCATION}/toolchain/jlink.cmake)

##### CMake debug prints #####
if (CMAKE_VERBOSE_MAKEFILE)
    print_debug_info()
endif()
