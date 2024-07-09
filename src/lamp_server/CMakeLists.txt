cmake_minimum_required(VERSION 3.1)

set(project_name lamp_server)

project(${project_name}
	VERSION 1.0
	DESCRIPTION "test project: lamp server"
	LANGUAGES CXX)

aux_source_directory(. sources)
add_executable(${project_name} ${sources})
target_link_libraries(${project_name} PUBLIC Threads::Threads ${Boost_LIBRARIES})

install(TARGETS ${project_name} DESTINATION ${CMAKE_HOME_DIRECTORY})
