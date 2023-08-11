cmake_minimum_required(VERSION 3.0)

if(UNITTEST AND EXISTS ../test)
	if(NOT TEST_OUTPUT_PATH)
		set(TEST_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/binaries/test)
	endif()

	file(GLOB SRC ../test/*.cpp ../test/*.h)

	add_executable(${PROJECT_NAME}_t ${SRC})
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)


	target_link_libraries(${PROJECT_NAME}_t ${PROJECT_NAME} gtest )
        set_target_properties(${PROJECT_NAME}_t PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
        add_test(NAME ${PROJECT_NAME}_t COMMAND ${PROJECT_NAME}_t WORKING_DIRECTORY ${TEST_OUTPUT_PATH})

	if(EXISTS ../test/config/conf.cmake)
		include(../test/config/conf.cmake)

	endif()
endif()
