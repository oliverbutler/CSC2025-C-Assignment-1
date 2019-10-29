#!/bin/sh
# This script runs each test of the object store in turn in a separate process.
# This means that the failure of one test will not prevent others from running.
# It is assumed that the test_obj_store binary that the script runs is
# in the "bin" subdirectory of the current directory.
# Usage:
#       ./obj_store_tests.sh

cd ./bin

if [ $? != 0 ]; then exit $?; fi

for i in 0 1 2
do
    ./test_obj_store $i $1
done

cd ..