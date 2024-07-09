cmake_minimum_required(VERSION 3.11)

set(project_name lamp)

project(${project_name}
	VERSION 1.0
	DESCRIPTION "test project: lamp client"
	LANGUAGES CXX)

aux_source_directory(. sources)
add_executable(${project_name} ${sources})
target_link_libraries(${project_name} PUBLIC ${Boost_LIBRARIES})

install(TARGETS ${project_name} DESTINATION ${CMAKE_HOME_DIRECTORY})
