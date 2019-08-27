###### Project Environment #####

# Set minimum CMake version
cmake_minimum_required(VERSION 2.8.4)

# Set the location of the kernel header files the user links to.
set(LIBK_LOCATION ../../include/libk)

# Set efm32-base project location
set(BASE_LOCATION ../../efm32-base)

# Set the compiler (must be prior to project setup)
include(${BASE_LOCATION}/toolchain/arm-gcc.cmake)

##### Project Setup #####

# Configure project and languages
project(AudioMoth-Project C CXX ASM)

# Set device
if(NOT DEVICE)
    set(DEVICE EFM32WG380F256)
endif()

# Set build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

##### Modules #####

# Libraries can be added to the LIBS variable
# or manually included here.

set(OPTIONAL_DEBUG_SYMBOLS "-DARM_MATH_CM4=1 -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Add base libs (emlib, CMSIS, device files)
include(${BASE_LOCATION}/toolchain/efm32-base.cmake)

add_subdirectory(${BASE_LOCATION}/cmsis ../${BASE_LOCATION}/cmsis)
add_subdirectory(${BASE_LOCATION}/emlib ../${BASE_LOCATION}/emlib)
add_subdirectory(${BASE_LOCATION}/device ../${BASE_LOCATION}/device)

configure_file(${LINKER_SCRIPT} ./)
file(READ ${CMAKE_CURRENT_BINARY_DIR}/${CPU_FAMILY_L}.ld linker_script_content)
set(replacement_string "
    /*
     * Very important to lay the Reset_Handler coming from the startup assembly code of the device
     * the first thing in Flash, so that we can jump to it from the bootloader.
     * Here we're just laying out the whole .text section; Reset_Handler just happens to be
     * the first symbol defined in the section so this approach is not very robust.
     * Plus, the default linker script has to be modified to include this line.
     *
     * A better approach would seem to be to compile the assembly code with something similar to
     * how -ffunction-sections, -fdata-sections works with C code and specify the section where
     * Reset_Handler ends up being in. However there doesn't seem to be such a thing, if it did
     * it would probably break how as directives .text, .section section_name work.
     */
    *startup_${CPU_FAMILY_L}.S.obj(.text)
")
string(REGEX REPLACE "KEEP\\(\\*\\(\\.vectors\\)\\).*(\\*\\(\\.text\\*\\))" "${replacement_string}    \\1" modified_file_content "${linker_script_content}")
# We need to quote the variable holding the output file, otherwise cmake will discard semicolons.
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${CPU_FAMILY_L}.ld "${modified_file_content}")
file(READ ${CMAKE_CURRENT_BINARY_DIR}/${CPU_FAMILY_L}.ld file_content1)

set(LINKER_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${CPU_FAMILY_L}.ld)

#  If it is a relative path it will be evaluated with respect to the current output directory, but it may also be an absolute path.
add_subdirectory(${BASE_LOCATION}/protocol ../${BASE_LOCATION}/protocol)
add_subdirectory(${BASE_LOCATION}/hardware ../${BASE_LOCATION}/hardware)

add_subdirectory(fatfs)

include(usb.cmake)

# This is normally set in efm32-base.cmake, but libraries may modify it so set
# it after libraries/subdirectories have been added
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_DEFINITIONS} -Xlinker -T${LINKER_SCRIPT} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections -Wl,-v")

##### Files #####

# Add project headers
include_directories(${LIBK_LOCATION}
    ../../targets/AudioMoth/include
    include)

add_executable(${PROJECT_NAME}
    src/audioMoth.c
    src/main.c
    ../../targets/AudioMoth/source/svc_device.c)

efm32_configure_linker_addresses(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} ${LIBS} emlib cmsis device fatfs)

# Link optional libraries if available
if (${HAS_HARDWARE})
    target_link_libraries(${PROJECT_NAME} hardware)
endif ()

if (${HAS_PROTOCOL})
    target_link_libraries(${PROJECT_NAME} protocol)
endif ()

##### Post build #####

# Add post build commands
include(${BASE_LOCATION}/toolchain/post-build.cmake)

##### CMake debug prints #####
if (CMAKE_VERBOSE_MAKEFILE)
    print_debug_info()
endif()
