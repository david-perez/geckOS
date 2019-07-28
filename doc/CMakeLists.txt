cmake_minimum_required(VERSION 2.8.4)

# Explicitly specify empty LANGUAGES so that cmake does not look for C and CXX compilers.
project(geckOS-doc LANGUAGES)

# Find some programs we will need.
find_package(Doxygen REQUIRED)
find_program(
    SPHINXBUILD
    NAMES sphinx-build-3 sphinx-build
    )
if(${SPHINXBUILD} STREQUAL SPHINX-NOTFOUND)
    message(FATAL_ERROR "The 'sphinx-build' command was not found. Make sure you have Sphinx installed.")
endif()
find_package(PythonInterp 3.4)

# This takes care of defining the doxygen build.
include(${PROJECT_SOURCE_DIR}/doxygen-index.cmake)

#
# Content section.
#
set(RST_OUT ${CMAKE_CURRENT_BINARY_DIR}/rst)

# This command is used to copy all documentation source files from the geckOS
# project root level directory into the build directory,
#
# We need to make copies because Sphinx requires a single documentation root
# directory, but geckOS's documentation is scattered around the tree in
# samples/ and doc/. Putting them into a single rooted tree in the build
# directory is a workaround for this limitation.
set(EXTRACT_CONTENT_COMMAND
    ${CMAKE_COMMAND} -E env
    ${PYTHON_EXECUTABLE} extract_content.py
    # Ignore any files in the output directory.
    --ignore ${CMAKE_CURRENT_BINARY_DIR}
    # Tell the script where the geckOS project root level directory is
    ${PROJECT_SOURCE_DIR}/../
    # Copy *all* files in doc to the rst folder.
    "*:doc:${RST_OUT}"
    # We want to copy the .rst files in samples/ to the rst folder.
    "*.rst:samples:${RST_OUT}/doc")

add_custom_target(content
    COMMAND ${EXTRACT_CONTENT_COMMAND}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Copying files to ${RST_OUT}")

#
# HTML section.
#
set(SPHINX_LOG ${CMAKE_CURRENT_BINARY_DIR}/sphinx.log)
set(SPHINX_OUTPUT_DIR_HTML ${CMAKE_CURRENT_BINARY_DIR}/html)

set(SPHINX_BUILD_HTML_COMMAND
    ${SPHINXBUILD} -w ${SPHINX_LOG} ${RST_OUT}/doc ${SPHINX_OUTPUT_DIR_HTML}
    -Dbreathe_projects.geckOS=${DOXYGEN_OUTPUT_DIR}/xml)

# The sphinx-html target is provided as a convenience for incremental re-builds
# of content files without regenerating the entire docs pipeline. It can be
# significantly faster than re-running the full HTML build, but it has no idea
# if Doxygen needs to be regenerated. Use with caution.
add_custom_target(sphinx-html
    COMMAND ${SPHINX_BUILD_HTML_COMMAND}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMENT "Just re-generating HTML (USE WITH CAUTION)")
add_dependencies(sphinx-html content)

add_custom_target(html
    ALL
    COMMAND ${SPHINX_BUILD_HTML_COMMAND}
    DEPENDS ${PROJECT_SOURCE_DIR}/conf.py # Depend on the sphinx config file.
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMENT "Generating HTML documentation with ${SPHINX_BUILD_HTML_COMMAND}")
add_dependencies(html doxygen content)
