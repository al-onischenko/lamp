cmake_minimum_required(VERSION 3.11)

project("lamp_all"
	VERSION 1.0
	DESCRIPTION "test project: client and server lamp"
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(
	Boost 1.69 REQUIRED
	COMPONENTS program_options
)
include_directories(${Boost_INCLUDE_DIR})

add_compile_options(-Wall -Wextra -Werror)
#add_compile_options(-O2)

add_subdirectory (src/lamp)
add_subdirectory (src/lamp_server)
