set(TEST_SOURCES
    malloc_test.h
    malloc_test.c
    pool_strategy_test.c)

add_executable(malloc_test ${TEST_SOURCES})

target_link_libraries(malloc_test PRIVATE ${PROJECT_NAME})

target_include_directories(malloc_test PRIVATE "${PROJECT_SOURCE_DIR}/src")

add_test(NAME malloc_test COMMAND malloc_test)