cmake_minimum_required(VERSION 2.6)

project(gingerbread)

set(BOOST_VERSION 1.37.0)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

set(BOOST_PACKAGES filesystem date_time program_options serialization signals thread system regex)

find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS ${BOOST_PACKAGES} REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)

include_directories(${Boost_INCLUDE_DIR} ${SDL_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories("include")

if(MSVC)
    add_definitions(
		/Wall /WX /Y- /D "MSVC" /wd4820 /wd4121 /wd4710 /wd4625 /wd4626 /wd4571 /wd4512 /wd4266 /wd4711
		/wd4514 /wd4548 /wd4244 /wd4242 /wd4365 /wd4350 /wd4510 /wd4610
	)
else(MSVC)
	add_definitions(
		-pipe -fsigned-char -march=native -pedantic -pedantic-errors -Wall -Wno-long-long
	)

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

#enable_testing()

add_subdirectory(main)
#add_subdirectory(gtest)
#add_subdirectory(main_test)
