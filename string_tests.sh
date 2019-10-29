#!/bin/sh
# This script runs each test of string in turn in a separate process. This
# means that the failure of one test will not prevent others from running.
# It is assumed that the test_string binary that the script runs is
# in the "bin" subdirectory of the current directory.
# Usage:
#       ./string_tests.sh [-o]
#
# If invoked with the -o flag, the object store is used during tests
# otherwise tests are run without the object store.

cd ./bin

if [ $? != 0 ]; then exit $?; fi

for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
    ./test_string $i $1
done

cd ..