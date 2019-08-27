# EFM32 EM_USB CMake file
# Configures the EM_USB and adds it to the build

##### Files #####

# Headers
include_directories(${BASE_LOCATION}/drivers/usb/inc)

# Source files
FILE(GLOB EFM_USB_SOURCES ${BASE_LOCATION}/drivers/usb/src/*.c)

##### Outputs #####

# USB library cannot be linked as a library or weak ISR symbols will not be resolved
# As such, it is reccomended to include ${EFM_USB_SOURCES} in your target compilation

# Create emlib library
add_library(emusb ${EFM_USB_SOURCES})

# TODO I don't think it matters, but we should be careful about these timers.
# target_compile_definitions(emusb
#     PRIVATE -DUSB_TIMER USB_TIMER1
# )

target_link_libraries(emusb emlib)

# Add library to build
set(LIBS ${LIBS} emusb)
