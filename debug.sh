#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Missing arguments"
    exit 1
fi

debugger=""
test_type=""

if [ "$OSTYPE" == "linux-gnu" ]; then
    debugger="gdb --args"
elif [[ "$OSTYPE" =~ "darwin" ]]; then
    debugger="lldb --"
fi

echo 

if [ "$1" == "s" ]; then
    $debugger "$(dirname "$0")/build/test/strategy-test/${2}"
elif [ "$1" == "r" ]; then
    $debugger "$(dirname $0)/build/test/rand-test/rand_test" "$(dirname $0)/build/test/rand-test/${2}_commands"
fi
