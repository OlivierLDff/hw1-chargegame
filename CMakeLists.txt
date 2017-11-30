cmake_minimum_required(VERSION 3.5)

set(PNAME ChargeGame)
project(${PNAME})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if ( MSVC )
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(SDL2_DIR "./SDL2-2.0.6")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2image REQUIRED)
find_package(SDL2ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

add_executable(${PNAME} Main.c)
set(MATHLIB m)
if ( MSVC )
	set(MATHLIB "")
endif()
target_link_libraries(${PNAME} ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2TTF_LIBRARY} ${MATHLIB})