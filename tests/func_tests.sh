#!/bin/bash

RED="\e[0;31m"
GRN="\e[0;32m"
BBLU="\e[1;34m"
WHT="\e[0m"
BYEL="\e[1;33m"

TIMEOUT=5

TOTAL=0
SUCES=0

test_cmd() {
    TOTAL=$((TOTAL + 1))
    
    local expected=$(timeout $TIMEOUT bash --posix -c "$1" 2>&1)
    local actual=$(timeout $TIMEOUT ./src/42sh -c "$1" 2>&1)

	if [ "$expected" = "$actual" ]; then
		echo -e "${GRN}$2${WHT}"
		SUCES=$((SUCES + 1))
	else
		echo -e "${RED}$2${WHT}"
		echo -e "${RED}Expected:${WHT} $expected"
		echo -e "${RED}Actual:${WHT} $actual"
	fi

}

test_cmd "echo a;" "Test echo super simple"

PERCENT=$(($SUCES * 100 / TOTAL))
echo -e "${BBLU}Succeed:${GRN} $SUCES${WHT}"
echo -e "${BBLU}Failed:${RED} $(($TOTAL - $SUCES))${WHT}"
echo -e "${BBLU}Results:${BYEL} $PERCENT%${WHT}"

if [ -n "$OUTPUT_FILE" ]; then
    echo "$PERCENT" > "$OUTPUT_FILE"
fi