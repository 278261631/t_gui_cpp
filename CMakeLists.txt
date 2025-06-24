cmake_minimum_required(VERSION 3.16)
project(t_gui_cpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL)

# Set Qt specific settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
set(CORE_SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/MainWindow.cpp
    src/core/LayerManager.cpp
    src/core/EventSystem.cpp
    src/core/SimpleLayer.cpp
)

set(PLUGIN_SOURCES
    src/plugins/PluginManager.cpp
    src/plugins/BasePlugin.cpp
)

set(UI_SOURCES
    src/ui/LayerWidget.cpp
    src/ui/ViewerWidget.cpp
    src/ui/ToolBar.cpp
)

set(UTILS_SOURCES
    src/utils/Logger.cpp
    src/utils/Config.cpp
)

# Header files
set(CORE_HEADERS
    src/core/Application.h
    src/core/MainWindow.h
    src/core/LayerManager.h
    src/core/EventSystem.h
    src/core/SimpleLayer.h
)

set(PLUGIN_HEADERS
    src/plugins/PluginManager.h
    src/plugins/PluginInterface.h
    src/plugins/BasePlugin.h
)

set(UI_HEADERS
    src/ui/LayerWidget.h
    src/ui/ViewerWidget.h
    src/ui/ToolBar.h
)

set(UTILS_HEADERS
    src/utils/Logger.h
    src/utils/Config.h
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${PLUGIN_SOURCES}
    ${UI_SOURCES}
    ${UTILS_SOURCES}
)

set(ALL_HEADERS
    ${CORE_HEADERS}
    ${PLUGIN_HEADERS}
    ${UI_HEADERS}
    ${UTILS_HEADERS}
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_SOURCES} ${ALL_HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets
    Qt5::OpenGL
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Enable plugin support
set_target_properties(${PROJECT_NAME} PROPERTIES
    ENABLE_EXPORTS ON
)

# Create directories
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/core)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/plugins)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ui)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/utils)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/resources)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Plugin library template
function(add_plugin PLUGIN_NAME)
    file(GLOB PLUGIN_SOURCES "plugins/${PLUGIN_NAME}/*.cpp")
    file(GLOB PLUGIN_HEADERS "plugins/${PLUGIN_NAME}/*.h")
    
    add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SOURCES} ${PLUGIN_HEADERS})
    target_link_libraries(${PLUGIN_NAME} Qt6::Core Qt6::Widgets)
    
    set_target_properties(${PLUGIN_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    )
endfunction()

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Copy plugins to build directory
install(DIRECTORY ${CMAKE_BINARY_DIR}/plugins/
    DESTINATION plugins
    FILES_MATCHING PATTERN "*.so" PATTERN "*.dll" PATTERN "*.dylib"
)
