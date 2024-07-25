set(RAND_TEST_SOURCES
    rand-tester.c)

add_executable(rand_test ${RAND_TEST_SOURCES})
target_link_libraries(rand_test PRIVATE ${PROJECT_NAME})

function(create_test test_name n_allocations n_reallocations lower_bound upper_bound initial_offset)
    set(commands_file "${test_name}_commands")

    add_test(
        NAME ${test_name}
        COMMAND ${CMAKE_BINARY_DIR}/test/rand-test/run-test.sh ${commands_file} ${n_allocations} ${n_reallocations} ${lower_bound} ${upper_bound} ${initial_offset}
    )
endfunction()

configure_file(${CMAKE_SOURCE_DIR}/test/rand-test/rand-generator.py ${CMAKE_BINARY_DIR}/test/rand-test/rand-generator.py COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/rand-test/run-test.sh ${CMAKE_BINARY_DIR}/test/rand-test/run-test.sh COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/rand-test/run-test.sh ${CMAKE_BINARY_DIR}/test/rand-test/run-test.sh COPYONLY)


create_test("tiny_test_1" 200 30 100 1000 30)
#create_test("tiny_test_2" 200 30 100 1000 30)
#create_test("tiny_test_3" 200 30 100 1000 30)
#create_test("tiny_test_4" 200 30 100 1000 30)
