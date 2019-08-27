# Ensure the library is compiled with no stack and heap so that we don't get a
# RAM overflow in the linking phase. In reality applications will use the
# kernel's stack pointer.
# Also, it is very important to define -D__NO_SYSTEM_INIT so that VTOR is not
# reset to 0x0 in SystemInit(), which gets called first thing in the
# Reset_Handler.
target_compile_definitions(device
    PRIVATE -D__NO_SYSTEM_INIT -D__STACK_SIZE=0x0 -D__HEAP_SIZE=0x0
)
