set(RAND_TEST_SOURCES
    rand-tester.c)

add_executable(rand_test ${RAND_TEST_SOURCES})
target_link_libraries(rand_test PRIVATE ${PROJECT_NAME})
target_include_directories(rand_test PRIVATE "${PROJECT_SOURCE_DIR}/src")

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

set(tiny_lower_bound 1)
set(tiny_upper_bound 110)

#pool rand strategy tests
create_test("rand_ps_test_1" 100 0 ${tiny_lower_bound} ${tiny_upper_bound} 30)
create_test("rand_ps_test_2" 200 0 ${tiny_lower_bound} ${tiny_upper_bound} 50)
create_test("rand_ps_test_3" 400 0 ${tiny_lower_bound} ${tiny_upper_bound} 70)
create_test("rand_ps_test_4" 800 0 ${tiny_lower_bound} ${tiny_upper_bound} 100)
create_test("rand_ps_test_5" 1600 0 ${tiny_lower_bound} ${tiny_upper_bound} 300)
create_test("rand_ps_test_6" 1600 300 ${tiny_lower_bound} ${tiny_upper_bound} 300)
create_test("rand_ps_test_8" 2000 800 ${tiny_lower_bound} ${tiny_upper_bound} 400)

set(small_lower_bound 130)
set(small_upper_bound 102300)

#free rand list strategy tests
create_test("rand_fls_test_1" 50 0 ${small_lower_bound} ${small_upper_bound} 20)
create_test("rand_fls_test_2" 100 0 ${small_lower_bound} ${small_upper_bound} 40)
create_test("rand_fls_test_3" 200 0 ${small_lower_bound} ${small_upper_bound} 50)
create_test("rand_fls_test_4" 200 0 ${small_lower_bound} ${small_upper_bound} 100)
create_test("rand_fls_test_5" 600 0 ${small_lower_bound} ${small_upper_bound} 100)
create_test("rand_fls_test_6" 600 0 ${small_lower_bound} ${small_upper_bound} 200)
create_test("rand_fls_test_7" 1000 0 ${small_lower_bound} ${small_upper_bound} 400)
create_test("rand_fls_test_8" 10000 0 ${small_lower_bound} ${small_upper_bound} 1000)

set(large_lower_bound 103000)
set(large_upper_bound 250000)

#large rand strategy test
create_test("rand_lgs_test_1" 50 0 ${large_lower_bound} ${large_upper_bound} 20)
create_test("rand_lgs_test_2" 100 0 ${large_lower_bound} ${large_upper_bound} 40)
create_test("rand_lgs_test_3" 200 0 ${large_lower_bound} ${large_upper_bound} 50)
create_test("rand_lgs_test_4" 200 0 ${large_lower_bound} ${large_upper_bound} 100)
create_test("rand_lgs_test_5" 600 0 ${large_lower_bound} ${large_upper_bound} 100)
create_test("rand_lgs_test_6" 600 0 ${large_lower_bound} ${large_upper_bound} 200)
create_test("rand_lgs_test_7" 1000 0 ${large_lower_bound} ${large_upper_bound} 400)
create_test("rand_lgs_test_8" 3000 0 ${large_lower_bound} ${large_upper_bound} 1000)
#create_test("rand_lgs_test_9" 5000 0 ${large_lower_bound} ${large_upper_bound} 3000)

#mix rand strategy test
create_test("rand_mix_test_1" 50 0 ${tiny_lower_bound} ${large_upper_bound} 20)
create_test("rand_mix_test_2" 100 0 ${tiny_lower_bound} ${large_upper_bound} 40)
create_test("rand_mix_test_3" 200 0 ${tiny_lower_bound} ${large_upper_bound} 50)
create_test("rand_mix_test_4" 200 0 ${tiny_lower_bound} ${large_upper_bound} 100)
create_test("rand_mix_test_5" 600 0 ${tiny_lower_bound} ${large_upper_bound} 100)
create_test("rand_mix_test_6" 600 0 ${tiny_lower_bound} ${large_upper_bound} 200)
create_test("rand_mix_test_7" 1000 0 ${tiny_lower_bound} ${large_upper_bound} 400)
create_test("rand_mix_test_8" 3000 0 ${tiny_lower_bound} ${large_upper_bound} 1000)
create_test("rand_mix_test_9" 5000 0 ${tiny_lower_bound} ${large_upper_bound} 3000)

#realloc test
create_test("rand_realloc_test_1" 50 40 ${tiny_lower_bound} ${large_upper_bound} 20)
create_test("rand_realloc_test_2" 100 90 ${tiny_lower_bound} ${large_upper_bound} 40)
create_test("rand_realloc_test_3" 200 190 ${tiny_lower_bound} ${large_upper_bound} 50)
create_test("rand_realloc_test_4" 200 190 ${tiny_lower_bound} ${large_upper_bound} 100)
create_test("rand_realloc_test_5" 600 500 ${tiny_lower_bound} ${large_upper_bound} 100)
create_test("rand_realloc_test_6" 600 500 ${tiny_lower_bound} ${large_upper_bound} 200)
create_test("rand_realloc_test_7" 2000 1900 ${tiny_lower_bound} ${large_upper_bound} 400)
create_test("rand_realloc_test_8" 3000 2000 ${tiny_lower_bound} ${large_upper_bound} 1000)
create_test("rand_realloc_test_9" 5000 3000 ${tiny_lower_bound} ${large_upper_bound} 3000)