#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Missing test name"
    exit 1
fi

TEST_DIR=$(dirname $0})/build/test/strategy-test

lldb "${TEST_DIR}/${1}"