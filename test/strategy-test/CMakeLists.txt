set(UNIT_TEST_SOURCES
    malloc_test.c
    )

function (create_test test_dir test_name)
    add_executable(${test_name} ${UNIT_TEST_SOURCES} "${test_dir}/${test_name}.c")
    target_link_libraries(${test_name} PRIVATE ${PROJECT_NAME})
    target_include_directories(${test_name} PRIVATE "${PROJECT_SOURCE_DIR}/src")
    target_include_directories(${test_name} PRIVATE "${PROJECT_SOURCE_DIR}/test/strategy-test")
    add_test(
        NAME ${test_name}
        COMMAND ${CMAKE_BINARY_DIR}/test/strategy-test/${test_name}
    )
endfunction()

#pool strategy tests
create_test(pool_strategy pool_strategy_test_1)
create_test(pool_strategy pool_strategy_test_2)
create_test(pool_strategy pool_strategy_test_3)
create_test(pool_strategy pool_strategy_test_4)
create_test(pool_strategy pool_strategy_test_5)
create_test(pool_strategy pool_strategy_test_6)
create_test(pool_strategy pool_strategy_test_7)
create_test(pool_strategy pool_strategy_test_8)
create_test(pool_strategy pool_strategy_test_9)
create_test(pool_strategy pool_strategy_test_10)
create_test(pool_strategy pool_strategy_chunk_test_1)
create_test(pool_strategy pool_strategy_chunk_test_2)
create_test(pool_strategy pool_strategy_init_test)
create_test(pool_strategy pool_strategy_zone_test_1)

#free list strategy tests
create_test(free_list_strategy free_list_strategy_test_1)
create_test(free_list_strategy free_list_strategy_test_2)
create_test(free_list_strategy free_list_strategy_test_3)
create_test(free_list_strategy free_list_strategy_test_4)
create_test(free_list_strategy free_list_strategy_test_5)

#large strategy tests