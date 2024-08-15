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

#pool rand strategy tests
create_test("rand_ps_test_1" 100 0 1 120 30)
create_test("rand_ps_test_2" 200 0 1 120 50)
create_test("rand_ps_test_3" 400 0 1 120 70)
create_test("rand_ps_test_4" 800 0 1 120 100)
create_test("rand_ps_test_5" 1600 0 1 120 300)
create_test("rand_ps_test_6" 1600 300 1 120 300)
create_test("rand_ps_test_8" 2000 800 1 120 400)

#free rand list strategy tests
create_test("rand_fls_test_1" 50 0 130 60000 20)
create_test("rand_fls_test_2" 100 0 130 60000 40)
create_test("rand_fls_test_3" 200 0 130 60000 50)
create_test("rand_fls_test_4" 200 0 130 60000 100)
create_test("rand_fls_test_5" 600 0 130 60000 100)
create_test("rand_fls_test_6" 600 0 130 60000 200)
create_test("rand_fls_test_7" 1000 0 130 60000 400)
create_test("rand_fls_test_8" 10000 0 130 60000 1000)

#large rand strategy test
create_test("rand_lgs_test_1" 50 0 66000 100000 20)
create_test("rand_lgs_test_2" 100 0 66000 100000 40)
create_test("rand_lgs_test_3" 200 0 66000 100000 50)
create_test("rand_lgs_test_4" 200 0 66000 100000 100)
create_test("rand_lgs_test_5" 600 0 66000 100000 100)
create_test("rand_lgs_test_6" 600 0 66000 100000 200)
create_test("rand_lgs_test_7" 1000 0 66000 100000 400)
create_test("rand_lgs_test_8" 10000 0 66000 100000 1000)
create_test("rand_lgs_test_9" 10000 0 66000 1000000 1000)

#mix rand strategy test
create_test("rand_mix_test_1" 50 0 0 100000 20)
create_test("rand_mix_test_2" 100 0 0 100000 40)
create_test("rand_mix_test_3" 200 0 0 100000 50)
create_test("rand_mix_test_4" 200 0 0 100000 100)
create_test("rand_mix_test_5" 600 0 0 100000 100)
create_test("rand_mix_test_6" 600 0 0 100000 200)
create_test("rand_mix_test_7" 1000 0 0 150000 400)
create_test("rand_mix_test_8" 10000 0 0 150000 1000)
create_test("rand_mix_test_9" 10000 0 0 1000000 1000)

#realloc test
create_test("rand_realloc_test_1" 1000 900 0 150000 5)
create_test("rand_realloc_test_2" 15000 2000 0 1000000 1000)
create_test("rand_realloc_test_3" 15000 3000 0 150000 1000)
create_test("rand_realloc_test_4" 15000 4000 0 1000000 1000)
create_test("rand_realloc_test_5" 15000 5000 0 150000 1000)
create_test("rand_realloc_test_6" 30000 6000 0 1000000 1000)
create_test("rand_realloc_test_7" 30000 7000 0 150000 1000)
create_test("rand_realloc_test_8" 30000 12000 0 1000000 1000)
create_test("rand_realloc_test_9" 30000 20000 0 1000000 1000)