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

#test args: test_name n_allocations n_reallocations lower_bound upper_bound initial_offset

#pool strategy tests
create_test("ps_test_1" 100 0 1 120 30)
create_test("ps_test_2" 200 0 1 120 50)
create_test("ps_test_3" 400 0 1 120 70)
create_test("ps_test_4" 800 0 1 120 100)
create_test("ps_test_5" 1600 0 1 120 300)
create_test("ps_test_6" 1600 300 1 120 300)
create_test("ps_test_8" 2000 800 1 120 400)

#free list strategy tests
create_test("fls_test_1" 50 0 130 60000 20)
create_test("fls_test_2" 100 0 130 60000 40)
create_test("fls_test_3" 200 0 130 60000 50)
create_test("fls_test_4" 200 0 130 60000 100)

#large strategy test
#create_test("lgs_test_1" 50 30 130 60000 30)
