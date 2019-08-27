message(${LINKER_SCRIPT})
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

set(LINKER_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${CPU_FAMILY_L}.ld)
