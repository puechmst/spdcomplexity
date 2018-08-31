cmake_minimum_required(VERSION 3.9)
set(CMAKE_C_COMPILER "/usr/bin/gcc-8")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-8")
project(spdcomplexity)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)


set(CMAKE_CXX_STANDARD 17)

# find boost
find_package(Boost COMPONENTS iostreams date_time)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

# Find the QtWidgets library
set(CMAKE_PREFIX_PATH /home/stephane/Qt/5.11.1/gcc_64)
find_package(Qt5Widgets CONFIG REQUIRED)

# find libproj
find_library(PROJ names proj HINTS /usr/lib)
if (PROJ_NOTFOUND)
    message(STATUS "libproj not found")
endif ()
message(STATUS ${PROJ})

# find mongodb
find_package (libmongoc-1.0 1.12 REQUIRED)
message ("--   mongoc found version \"${MONGOC_VERSION}\"")
message ("--   mongoc include path \"${MONGOC_INCLUDE_DIRS}\"")
message ("--   mongoc libraries \"${MONGOC_LIBRARIES}\"")
include_directories(${MONGOC_INCLUDE_DIRS})
add_definitions(${MONGO_DEFINITIONS})

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
target_link_libraries(spdcomplexity Qt5::Widgets ${Boost_LIBRARIES} ${MONGOC_LIBRARIES} ${PROJ} stdc++fs)
