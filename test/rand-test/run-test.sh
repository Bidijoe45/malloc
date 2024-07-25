#!/bin/bash

if [ "$#" -ne 6 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

python3 rand-generator.py $1 $2 $3 $4 $5 $6
./rand_test $1 $2
exit $?