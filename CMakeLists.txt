cmake_minimum_required(VERSION 3.7)

set(PNAME ChargeGame)
project(${PNAME})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if ( MSVC )
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(SDL2_DIR "./SDL2-2.0.6")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

add_executable(${PNAME} Main.c)
target_link_libraries(${PNAME} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})