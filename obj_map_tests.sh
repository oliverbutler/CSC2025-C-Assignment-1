#!/bin/sh
# This script runs each test of the object map in turn in a separate process.
# This means that the failure of one test will not prevent others from running.
# It is assumed that the test_obj_map binary that the script runs is
# in the "bin" subdirectory of the current directory.
# Usage:
#       ./obj_map_tests.sh

cd ./bin

if [ $? != 0 ]; then exit $?; fi

for i in 0 1 2 3 4 5 6
do
    ./test_obj_map $i $1
done

cd ..