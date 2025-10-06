# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(COMPONENTS main)
set(EXTRA_COMPONENT_DIRS /home/sonic/Desktop/computer/gits/esp-idf-lib/components
                         ${CMAKE_CURRENT_SOURCE_DIR}/components)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(espvco)