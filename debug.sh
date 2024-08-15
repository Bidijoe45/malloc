#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Missing arguments"
    exit 1
fi

debugger=""
test_type=""

if [ "$OSTYPE" == "linux-gnu" ]; then
    debugger="gdb"
elif [ "$OSTYPE" == "darwin" ]; then
    debugger="lldb"
fi

if [ "$1" == "s" ]; then
    test_type="strategy-test"
elif [ "$1" == "r" ]; then
    test_type="rand-test"
fi

TEST_DIR=$(dirname "$0")/build/test/$test_type

$debugger "${TEST_DIR}/${2}"
