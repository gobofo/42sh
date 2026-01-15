#!/bin/bash

#----------------- COLOR -----------------#
# 0 - No style | 1 - Bold
RED="\e[0;31m"
GRN="\e[0;32m"
BBLU="\e[1;34m"
WHT="\e[0m"
BYEL="\e[1;33m"

TIMEOUT=5

TOTAL=0
SUCCESS=0

if [ -z "$BIN_PATH" ]; then
    echo "Error: BIN_PATH not set"
    exit 0
fi

test_cmd()
{
	TOTAL=$((TOTAL + 1))
    
    local expected=$2
	#$(timeout $TIMEOUT bash --posix -c "$1" 2>&1)
    local actual=$(timeout $TIMEOUT "$BIN_PATH" -c "$1" 2>&1)

	if [ "$expected" = "$actual" ]; then
		echo -e "${GRN}$3${WHT}"
		SUCCESS=$((SUCCES + 1))
	else
		echo -e "${RED}$3${WHT}"
		echo -e "${RED}Expected:${WHT} $expected"
		echo -e "${RED}Actual:${WHT} $actual"
	fi
}

if [ "$COVERAGE" = "yes" ]; then

	echo "Unit Test"

    gcc -I../src unit/lexer_tests.c ../src/lexer/*.c -o unit_tester

    if ./unit_tester; then
        SUCCESS=$((SUCCESS + 1))
    fi

    TOTAL=$((TOTAL + 1))

fi

#test fonctionels

test_cmd \
	"echo a;" \
	"a" \
	"Test echo super simple"

test_cmd \
	"echo a; echo b" \
	"a
b" \
	"Test echo double"

PERCENT=$(($SUCCESS * 100 / $TOTAL))
echo -e "${BBLU}Succeed:${GRN} $SUCCESS${WHT}"
echo -e "${BBLU}Failed:${RED} $(($TOTAL - $SUCCESS))${WHT}"
echo -e "${BBLU}Results:${BYEL} $PERCENT%${WHT}"

if [ -n "$OUTPUT_FILE" ]; then
    echo "$PERCENT" > "$OUTPUT_FILE"
fi

exit 0
