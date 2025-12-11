cmake_minimum_required(VERSION 3.16)
project(gridview)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable automatic MOC, UIC, and RCC processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(include)

# Add executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/grid_view.cpp
    include/grid_view.h
    include/i_element.h
    include/i_element_manager.h
    include/i_route_builder.h
    include/i_scene.h
    include/point.h
    src/point.cpp
    include/obstacle.h
    src/obstacle.cpp
    include/route.h
    src/route.cpp
    include/element_manager.h
    src/element_manager.cpp
    include/route_builder.h
    src/route_builder.cpp
    include/scene.h
    src/scene.cpp
    include/scene_factory.h
    src/scene_factory.cpp
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets)