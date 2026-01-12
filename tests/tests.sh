#!/bin/sh

#----------------- COLOR -----------------#
# 0 - No style | 1 - Bold
RED="\e[0;31m"
BRED="\e[1;31m"
GRN="\e[0;32m"
BGRN="\e[1;32m"
YEL="\e[0;33m"
BYEL="\e[1;33m"
BLU="\e[0;34m"
BBLU="\e[1;34m"
PUR="\e[0;35m"
BPUR="\e[1;35m"
CYA="\e[0;36m"
BCYA="\e[1;36m"
WHI="\e[0;37m"
BWHI="\e[1;37m"
GRE="\e[2;37m"

#----------------- TESTS -----------------#

#----------------- WITH GOOD GRAMMAR -----------------#

#test_basic

test_null() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
EOF

	"$SHELL42" -c "" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if [ $return_val -eq 0 ] && diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test null passed --\e[0m"
	else
		echo -e "${RED}-- Test null failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

test_echo_simple() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
aaa
EOF

	"$SHELL42" -c "echo aaa;" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if [ $return_val -eq 0 ] && diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test echo simple passed --\e[0m"
	else
		echo -e "${RED}-- Test echo simple failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

#test_multiple_echo

test_echo_multiple() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
aaa
bbb
ccc
EOF

	"$SHELL42" -c "echo aaa; echo bbb; echo ccc" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if [ $return_val -eq 0 ] && diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test echo multiple passed --\e[0m"
	else
		echo -e "${RED}-- Test echo multiple failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

#if basics

test_if_simple() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
aaa
EOF

	"$SHELL42" -c "if false; then echo bbb; else echo aaa; fi" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if [ $return_val -eq 0 ] && diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test if simple passed --\e[0m"
	else
		echo -e "${RED}-- Test if simple failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

#if imbriquer simple

test_simple_nested_if() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
check
EOF

	"$SHELL42" -c "if true; then if true; then echo check; fi; fi" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if [ $return_val -eq 0 ] && diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test simple nested if passed --\e[0m"
	else
		echo -e "${RED}-- Test simple nested if failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

#if imbriquer expert

test_expert_nested_if() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
c
EOF

	"$SHELL42" -c "if false; then echo a; else if true; then if false; then echo b; elif true; then echo c; else echo d; fi; fi; fi" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if [ $return_val -eq 0 ] && diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test expert nested if passed --\e[0m"
	else
		echo -e "${RED}-- Test expert nested if failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

#----------------- WITH BAD GRAMMAR -----------------#

test_missing_separator() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
Error: Parsing
EOF

	"$SHELL42" -c "echo aaa if true; then echo bbb; fi" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test missing separator passed --\e[0m"
	else
		echo -e "${RED}-- Test missing separator failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

test_missing_fi() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
Error: Parsing
EOF

	"$SHELL42" -c "if true; then echo bbb;" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test missing fi passed --\e[0m"
	else
		echo -e "${RED}-- Test missing fi failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

test_multiple_else() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
Error: Parsing
EOF

	"$SHELL42" -c "if true; then echo bbb; else echo cccc; else echo dddd; fi" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test multiple else passed --\e[0m"
	else
		echo -e "${RED}-- Test multiple else failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

test_missing_then() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
Error: Parsing
EOF

	"$SHELL42" -c "if true; fi" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test missing then passed --\e[0m"
	else
		echo -e "${RED}-- Test missing then failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

test_too_much_separator() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
Error: Parsing
EOF

	"$SHELL42" -c "echo aaa ;; echo bb" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test too much separator passed --\e[0m"
	else
		echo -e "${RED}-- Test too much separator failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

test_start_with_separator() {
	local TMP=$(mktemp -d)
	local SHELL42="$(realpath src/42sh)"
	cd "$TMP"

	cat > expected.txt << 'EOF'
Error: Parsing
EOF

	"$SHELL42" -c "; echo bb" > actual.txt 2>&1
	local return_val=$?

	cd - > /dev/null

	if diff -u "$TMP/expected.txt" "$TMP/actual.txt" > /dev/null; then
		echo -e "${GRN}-- Test start with separator passed --\e[0m"
	else
		echo -e "${RED}-- Test start with separator failed --\e[0m"
		echo -e "${YEL}Resultat attendu:\e[0m"
		cat -e "$TMP/expected.txt"
		echo -e "${YEL}Resultat obtenu:\e[0m"
		cat -e "$TMP/actual.txt"
		echo -e "${YEL}Code de retour: $return_val\e[0m"
	fi

	rm -rf "$TMP"
	return 0
}

#execution tests

echo -e "$CYA=============== TESTS ===============\e[0m"
echo -e "$BLU=========== TESTS WITH GOOD GRAMMAR ===========\e[0m"
echo -e ""

test_null
test_echo_simple
test_echo_multiple
test_if_simple
test_simple_nested_if
test_expert_nested_if

echo -e ""
echo -e "$BLU=========== TESTS WITH BAD GRAMMAR ===========\e[0m"
echo -e ""

test_missing_separator
test_missing_fi
test_multiple_else
test_missing_then
test_too_much_separator
test_start_with_separator

echo -e ""
echo -e "$CYA========== FIN DES TESTS =============\e[0m"
