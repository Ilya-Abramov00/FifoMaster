set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/config)
set(DST_DIR ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/binaries/test)
message("src dir bpl test =" ${SRC_DIR})
message("dist dir bpl test =" ${DST_DIR})

add_custom_target(${PROJECT_NAME}_tconf

        COMMAND ${CMAKE_COMMAND} -E make_directory  ${DST_DIR}/string_test

        COMMAND ${CMAKE_COMMAND} -E copy ${SRC_DIR}/string_test/file_1.txt ${DST_DIR}/string_test/
        COMMAND ${CMAKE_COMMAND} -E copy ${SRC_DIR}/string_test/file_2.txt ${DST_DIR}/string_test/
        COMMAND ${CMAKE_COMMAND} -E copy ${SRC_DIR}/string_test/file_4.txt ${DST_DIR}/string_test/
        )
add_dependencies(${PROJECT_NAME}_t ${PROJECT_NAME}_tconf)