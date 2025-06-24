cmake_minimum_required(VERSION 3.16)
project(ExamplePlugin)

# Find Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Set Qt specific settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../../src)

# Source files
set(PLUGIN_SOURCES
    ExamplePlugin.cpp
)

set(PLUGIN_HEADERS
    ExamplePlugin.h
)

# Create plugin library
add_library(ExamplePlugin SHARED ${PLUGIN_SOURCES} ${PLUGIN_HEADERS})

# Link Qt libraries
target_link_libraries(ExamplePlugin 
    Qt6::Core 
    Qt6::Widgets
)

# Set plugin properties
set_target_properties(ExamplePlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    PREFIX ""  # Remove lib prefix on Unix
)

# Copy plugin metadata
configure_file(plugin.json ${CMAKE_BINARY_DIR}/plugins/plugin.json COPYONLY)
