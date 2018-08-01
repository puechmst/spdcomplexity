cmake_minimum_required(VERSION 3.9)
project(spdcomplexity)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)

# find boost
find_package(Boost COMPONENTS iostreams)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Find the QtWidgets library
set(CMAKE_PREFIX_PATH /home/stephane/Qt/5.11.0/gcc_64)
find_package(Qt5Widgets CONFIG REQUIRED)

set(UI_SRC
        ui/mainwindow.cpp
        ui/mainwindow.h
        ui/mainwindow.ui)
set(DATA_SRC
        data/Parser.cpp
        data/Parser.h)

add_executable(spdcomplexity
        ${UI_SRC}
        ${DATA_SRC}
        ui/main.cpp)
# Use the Widgets module from Qt 5
target_link_libraries(spdcomplexity Qt5::Widgets ${Boost_LIBRARIES} stdc++fs)