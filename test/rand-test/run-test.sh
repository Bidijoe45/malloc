#!/bin/bash

if [ "$#" -ne 6 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

CURRENT_DIR=$(dirname $0)

python3 "$CURRENT_DIR/rand-generator.py" $1 $2 $3 $4 $5 $6
"$CURRENT_DIR/rand_test" $1
exit $?