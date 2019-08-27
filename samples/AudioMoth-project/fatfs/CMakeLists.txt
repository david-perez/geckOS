# Set efm32-base project location
set(BASE_LOCATION ../../../efm32-base)

# fatfs CMake file
project(fatfs)

# find . -name '*.c' | sort -u
add_library(${PROJECT_NAME}
    src/audioMoth_ff.c
    src/diskio.c)

target_include_directories(${PROJECT_NAME} PUBLIC
    inc
    ${BASE_LOCATION}/hardware/kit/common/drivers
    ../include)

target_link_libraries(${PROJECT_NAME} emlib)
