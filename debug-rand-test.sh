#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Missing test name"
    exit 1
fi

TEST_DIR=$(dirname $0})/build/test/rand-test

lldb -- "${TEST_DIR}/rand_test" "${TEST_DIR}/${1}_commands"