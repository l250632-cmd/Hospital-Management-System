cmake_minimum_required(VERSION 3.16)
project(MediCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML 2.6.2 - adjust SFML_DIR if needed
# Example: cmake -DSFML_DIR=/path/to/SFML-2.6.2/lib/cmake/SFML ..
find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system)

# All source files
set(SOURCES
    main.cpp
)

add_executable(MediCore ${SOURCES})

target_include_directories(MediCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(MediCore
    sfml-graphics
    sfml-window
    sfml-system
)

# Copy any needed data files to build directory
# (The .txt data files should be placed next to the executable)
