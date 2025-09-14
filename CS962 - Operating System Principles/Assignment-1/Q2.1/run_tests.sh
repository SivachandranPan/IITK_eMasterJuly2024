#!/bin/bash 

if [[ ! -f pipes.c ]];
then
     echo "Failed. The file pipes.c does not exist"	
     exit -1
fi

rm -f pipes temp.log

NA=`grep "system" pipes.c`
if [[ "$NA" != "" ]];
then
     echo "Your solution uses some constructs that is not allowed culprit --> $NA"	
     exit -1

fi

gcc pipes.c -o pipes

if [[ ! -f pipes ]];
then
     echo "Failed. Compilation error"	
     exit -1
fi

strace -f -o temp.log sh -c './pipes "echo | tr a-z A-Z | tr A-Z a-z | tr a-z A-Z | tr A-Z a-z"'
NUM_CLONES=$(grep -c 'clone' temp.log)
NUM_EXEC=$(grep -c 'exec' temp.log)
NUM_PIPE=$(grep -c 'pipe2' temp.log)

if [ $NUM_CLONES -lt 5 ] || [ $NUM_EXEC -lt 5 ] || [ $NUM_PIPE -lt 4 ];
then
    echo "Your solution does not implement the problem in pipelined manner :("
    rm -f pipes temp.log
    exit -1
fi

test_case() {
  NUM="$1"
  COMMAND="$2"
  EXPECTED="$3"
  
  OUTPUT=$(./pipes "$COMMAND" | tr -d ' \n')
  if [[ "$OUTPUT" == "$EXPECTED" ]]; 
  then
      echo "Test case $NUM passed"
  else
      echo "Test case $NUM failed. Your output: $OUTPUT"
  fi
}

# Test cases.
test_case 1 "echo hellowworld | wc -l" "1"
test_case 2 "echo OperatingSystemPrinciplesAssignment1 | wc -l" "1"
test_case 3 "cat testcases/testcase1.txt | sort | uniq | head -n 1" "Apple"
test_case 4 "cat testcases/testcase1.txt | sort | uniq | tail -n 1" "Orange"
test_case 5 "cat testcases/testcase1.txt | grep Apple | sort -r | grep pp | wc -l" "3"
test_case 6 "cat testcases/testcase2.txt | grep 500 | sort -r | grep C5 | wc -l" "4"
test_case 7 "cat testcases/testcase2.txt | grep 600 | sort -r | grep d6 | wc -l" "4"
test_case 8 "cat testcases/testcase3.txt | grep cs330 | sort -r | grep cs | wc -l" "1"
test_case 9 "cat testcases/testcase3.txt | grep cs | sort -r | grep c | wc -l" "4"
test_case 10 "cat testcases/testcase3.txt | grep system | sort -r | grep sys | wc -l" "1"
test_case 11 "cat testcases/testcase3.txt | grep cs330 | wc -l" "1"
test_case 12 "cat testcases/testcase3.txt | grep 12 | sort -r | grep 1 | wc -l" "2"

# Clean
rm -f pipes
rm -f temp.log
