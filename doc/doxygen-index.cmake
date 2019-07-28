# Find all the public headers.
set(GECKOS_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
file(GLOB_RECURSE GECKOS_PUBLIC_HEADERS ${GECKOS_PUBLIC_HEADER_DIR}/*.h)

set(DOXYGEN_INPUT_DIR ${GECKOS_PUBLIC_HEADER_DIR})
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) # Doxygen won't create this for us.

# Only regenerate Doxygen when the Doxyfile or public headers change.
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
    DEPENDS ${GECKOS_PUBLIC_HEADERS} ${DOXYFILE_OUT}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    COMMENT "Generating doxygen docs"
    VERBATIM)

add_custom_target(doxygen DEPENDS ${DOXYGEN_INDEX_FILE})
