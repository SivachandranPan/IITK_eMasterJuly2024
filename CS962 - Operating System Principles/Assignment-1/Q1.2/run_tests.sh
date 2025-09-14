#!/bin/bash

if [[ ! -f level_up.c ]];
then
     echo "Failed. The file level_up.c does not exist"	
     exit -1
fi

rm -f level_up

NA=`grep "system" level_up.c`
if [[ "$NA" != "" ]];
then
     echo "Your solution uses some constructs that is not allowed culprit --> $NA"	
     exit -1

fi

NA=`grep "grep" level_up.c`
if [[ "$NA" != "" ]];
then
     echo "Your solution uses some constructs that is not allowed culprit --> $NA"	
     exit -1

fi

NA=`grep "fork" level_up.c`
if [[ "$NA" != "" ]];
then
     echo "Your solution uses some constructs that is not allowed culprit --> $NA"	
     exit -1

fi

NA=`grep "exec" level_up.c`
if [[ "$NA" != "" ]];
then
     echo "Your solution uses some constructs that is not allowed culprit --> $NA"	
     exit -1

fi

gcc level_up.c -o level_up

if [[ ! -f level_up ]];
then
     echo "Failed. Compilation error"	
     exit -1
fi


# Test case function
test_case() {

    STRING="$1"
    FILE="$2"
    START_OFFSET="$3"
    END_OFFSET="$4"
    ORDER="$5"
    EXPECTED="$6"
    # NUM="$6"
    NUM="$7"
   
   OUTPUT=$(./level_up "$STRING" "$FILE" "$START_OFFSET" "$END_OFFSET" "$ORDER")
   if [ "$OUTPUT" == "$EXPECTED" ];
    then
      echo "Test case $NUM passed"
    else
      echo "Test case $NUM failed. Your output: $OUTPUT"
    fi
}

# Test cases.
test_case "test" "testcases/testcase1.txt" 6 20 0 "FOUND" "1"
test_case "tac" "testcases/testcase2.txt" 0 30 1 "FOUND" "2"
test_case "cs330" "testcases/testcase3.txt" 2 50 0 "NOT FOUND" "3"
test_case "cs330" "testcases/testcase3.txt" 2 50 1 "NOT FOUND" "4"
test_case "cs330" "testcases/testcase3.txt" 0 50 0 "FOUND" "5"
test_case "cs330" "testcases/testcase3.txt" 0 50 1 "NOT FOUND" "6"
test_case "03" "testcases/testcase3.txt" 0 50 1 "FOUND" "7"
test_case "03" "testcases/testcase3.txt" 0 50 0 "NOT FOUND" "8"
test_case "216cs193" "testcases/testcase3.txt" 0 90 0 "FOUND" "9"
test_case "216cs193" "testcases/testcase3.txt" 0 90 1 "NOT FOUND" "10"
test_case "216cs193" "testcases/testcase3.txt" 80 90 0 "FOUND" "11"
test_case "216cs193" "testcases/testcase3.txt" 82 90 0 "NOT FOUND" "12"
# Clean
rm -f level_up
