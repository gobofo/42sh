#!/bin/sh

#----------------- COLOR -----------------#
RED="\e[0;31m"
GRN="\e[0;32m"
BBLU="\e[1;34m"
WHT="\e[0m"
BYEL="\e[1;33m"

TIMEOUT=2
TOTAL=0
SUCCESS=0

if [ -z "$BIN_PATH" ]; then
  echo "Error: BIN_PATH not set"
  exit 0
fi

test_cmd() {
  TOTAL=$((TOTAL + 1))
  local expected=$(timeout $TIMEOUT env LC_ALL=C bash --posix -c "$1" 2>&1)
  local actual=$(timeout $TIMEOUT env LC_ALL=C "$BIN_PATH" -c "$1" 2>&1)

  if [ "$expected" = "$actual" ]; then
    SUCCESS=$((SUCCESS + 1))
  else
    echo -e "${RED}Test:${WHT} $2"
    echo -e "${RED}Command:${WHT} $1"
    echo -e "${RED}Expected:${WHT} $expected"
    echo -e "${RED}Actual:${WHT} $actual"
  fi
}

test_file() {
  TOTAL=$((TOTAL + 1))

  local expected=$(timeout $TIMEOUT bash --posix "$1" 2>&1)
  local actual=$(timeout $TIMEOUT "$BIN_PATH" "$1" 2>&1)

  if [ "$expected" = "$actual" ]; then
    SUCCESS=$((SUCCESS + 1))
  else
    echo -e "${RED}Test:${WHT} $2"
    echo -e "${RED}Command:${WHT} $1"
    echo -e "${RED}Expected:${WHT} $expected"
    echo -e "${RED}Actual:${WHT} $actual"
  fi
}

test_error() {
  TOTAL=$((TOTAL + 1))

  timeout $TIMEOUT "$BIN_PATH" -c "$1" >/dev/null 2>&1
  local exit_code=$?

  if [ $exit_code -ne 0 ]; then
    SUCCESS=$((SUCCESS + 1))
  else
    echo -e "${RED}Test:${WHT} $2"
    echo -e "${RED}Command:${WHT} $1"
    echo -e "${RED}Expected:${WHT} $expected"
    echo -e "${RED}Actual:${WHT} $actual"
  fi
}

test_stdin() {
  TOTAL=$((TOTAL + 1))
  local expected=$(echo "$1" | timeout $TIMEOUT bash --posix 2>&1)
  local actual=$(echo "$1" | timeout $TIMEOUT "$BIN_PATH" 2>&1)
  if [ "$expected" = "$actual" ]; then
    SUCCESS=$((SUCCESS + 1))
  else
    echo -e "${RED}Test:${WHT} $2"
    echo -e "${RED}Command:${WHT} $1"
    echo -e "${RED}Expected:${WHT} $expected"
    echo -e "${RED}Actual:${WHT} $actual"
  fi
}

if [ "$COVERAGE" = "yes" ]; then

  echo "###################################################"
  echo "RUNNING UNIT TESTS"
  echo "###################################################"

  if [ -f "unit/unit_tests" ]; then

    UNIT_OUT=$(./unit/unit_tests --verbose --color=never 2>&1)
    UNIT_EXIT=$?

    PASSED=$(echo "$UNIT_OUT" | grep "Synthesis:" | grep -oE 'Passing: *[0-9]+' | grep -oE '[0-9]+')
    TESTED=$(echo "$UNIT_OUT" | grep "Synthesis:" | grep -oE 'Tested: *[0-9]+' | grep -oE '[0-9]+')

    if [ -n "$PASSED" ] && [ -n "$TESTED" ]; then
      SUCCESS=$((SUCCESS + PASSED))
      TOTAL=$((TOTAL + TESTED))
      echo -e "${GRN}Unit tests completed: $PASSED/$TESTED${WHT}"
    else
      echo -e "${RED}Failed to parse Criterion output${WHT}"
      if [ $UNIT_EXIT -eq 0 ]; then
        SUCCESS=$((SUCCESS + 1))
      fi
      TOTAL=$((TOTAL + 1))
    fi

  else
    echo -e "${RED}Unit tester binary not found${WHT}"
  fi

fi

#----------------- STEP 1: BASIC FUNCTIONALITY -----------------#

echo "###################################################"
echo "STEP 1 - SIMPLE COMMANDS"
echo "###################################################"

test_cmd "echo hello" "echo simple"
test_cmd "echo hello world" "echo multiple args"
test_cmd "echo a b c d e" "echo many args"
test_cmd "echo" "echo no args"
test_cmd "/bin/echo test" "echo absolute path"
test_cmd "ls /tmp > /dev/null; echo ok" "ls with redirect"
test_cmd "true" "true builtin"
test_cmd "false" "false builtin"
test_cmd "/bin/true" "true absolute path"
test_cmd "/bin/false" "false absolute path"

echo "###################################################"
echo "STEP 1 - COMMAND LISTS"
echo "###################################################"

test_cmd "echo a; echo b" "two commands semicolon"
test_cmd "echo a; echo b; echo c" "three commands"
test_cmd "echo a;echo b;echo c;echo d" "four commands no space"
test_cmd "echo a; echo b;" "trailing semicolon"
test_error "echo a;; echo b" "syntax error double semicolon"
test_cmd "true; echo after" "true then echo"
test_cmd "false; echo after" "false then echo"
test_cmd "echo start; echo middle; echo end" "three echos"

echo "###################################################"
echo "STEP 1 - IF COMMANDS"
echo "###################################################"

test_cmd "if true; then echo yes; fi" "if true simple"
test_cmd "if false; then echo no; fi" "if false no output"
test_cmd "if true; then echo a; echo b; fi" "if true multiple commands"
test_cmd "if false; then echo no; else echo yes; fi" "if else false"
test_cmd "if true; then echo yes; else echo no; fi" "if else true"
test_cmd "if false; then echo a; elif true; then echo b; fi" "if elif true"
test_cmd "if false; then echo a; elif false; then echo b; fi" "if elif false"
test_cmd "if false; then echo a; elif false; then echo b; else echo c; fi" "if elif else"
test_cmd "if true; then if true; then echo nested; fi; fi" "nested if"
test_cmd "if true; then echo a; fi; echo b" "if followed by command"
test_cmd "if false; then echo a; elif false; then echo b; elif true; then echo c; fi" "multiple elif"
test_cmd "if true && true; then echo ok; fi" "if with AND condition"
test_cmd "if false || true; then echo ok; fi" "if with OR condition"
test_cmd "if ! false; then echo ok; fi" "if with negation"
test_cmd "if true; then true; fi" "if with true body"
test_cmd "if false; then false; else true; fi" "if else no echo"

echo "###################################################"
echo "STEP 1 - COMPOUND LISTS (NEWLINES)"
echo "###################################################"

test_cmd "if true
then
echo multiline
fi" "if multiline format"

test_cmd "if true; then
echo a
echo b
fi" "if with newlines in body"

test_cmd "echo a
echo b
echo c" "commands separated by newlines"

test_cmd "if false
true
then echo ok; fi" "if compound condition with newline"

echo "###################################################"
echo "STEP 1 - SINGLE QUOTES"
echo "###################################################"

test_cmd "echo 'hello'" "single quotes simple"
test_cmd "echo 'hello world'" "single quotes with space"
test_cmd "echo 'a b c d'" "single quotes multiple words"
test_cmd "echo ''" "empty single quotes"
test_cmd "echo '' '' ''" "multiple empty quotes"
test_cmd "echo 'tab	here'" "single quotes with tab"
test_cmd "echo 'line1\nline2'" "single quotes no escape"
test_cmd "echo a'b'c" "concatenation with quotes"
test_cmd "echo 'single quote text here'" "long single quoted text"

echo "###################################################"
echo "STEP 1 - ECHO BUILTIN OPTIONS"
echo "###################################################"

test_cmd "echo -n test" "echo -n basic"
test_cmd "echo -n hello world" "echo -n multiple words"
test_cmd "echo -n -n test" "echo -n repeated"
test_cmd "echo -n" "echo -n no args"
test_cmd "echo -e 'hello\nworld'" "echo -e newline"
test_cmd "echo -e 'tab\there'" "echo -e tab"
test_cmd "echo -e 'back\\\\slash'" "echo -e backslash"
test_cmd "echo -e 'a\nb\nc'" "echo -e multiple newlines"
test_cmd "echo -E 'no\nnewline'" "echo -E no interpretation"
test_cmd "echo -n -e 'test\n'" "echo -n -e combined"
test_cmd "echo -e -n 'test\n'" "echo -e -n reversed"
test_cmd "echo -e '\t\ttabs'" "echo -e double tabs"
test_cmd "echo -E" "echo -E no args"

echo "###################################################"
echo "STEP 1 - COMMENTS"
echo "###################################################"

test_cmd "echo test # comment here" "inline comment"
test_cmd "# full line comment
echo visible" "full line comment"
test_cmd "echo a; # comment
echo b" "comment after semicolon"
test_cmd "echo not#comment" "hash not at start"
test_cmd "echo \# escaped" "escaped hash"
test_cmd "echo '#' quoted" "quoted hash"

#----------------- STEP 2: ADVANCED FEATURES -----------------#

echo "###################################################"
echo "STEP 2 - REDIRECTIONS"
echo "###################################################"

test_cmd "echo test > /tmp/42sh_1; cat /tmp/42sh_1" "redirect output"
test_cmd "echo line1 > /tmp/42sh_2; echo line2 >> /tmp/42sh_2; cat /tmp/42sh_2" "redirect append"
test_cmd "echo hello > /tmp/42sh_3; cat < /tmp/42sh_3" "redirect input"
test_cmd "echo a > /tmp/42sh_4; echo b > /tmp/42sh_4; cat /tmp/42sh_4" "overwrite file"
test_cmd "echo -n test > /tmp/42sh_5; cat /tmp/42sh_5" "redirect with -n"
test_cmd "echo -e 'a\nb' > /tmp/42sh_6; cat /tmp/42sh_6" "redirect with -e"
test_cmd "cat < /dev/null" "redirect from /dev/null"
test_cmd "echo test > /dev/null" "redirect to /dev/null"
test_cmd "if true; then echo a > /tmp/42sh_7; fi; cat /tmp/42sh_7" "redirect in if"
test_cmd "cat /etc/hostname 2>/dev/null" "redirect stderr"
test_cmd "echo visible 2>/dev/null" "redirect stderr no effect"

echo "###################################################"
echo "STEP 2 - REDIR TRICKY TOKEN (NO SPACES)"
echo "###################################################"

test_cmd "echo a>out; cat out" "replace"
test_cmd 'rm -f empty_file; echo a>>empty_file; cat empty_file' "in"
test_cmd "echo a>|out; cat out" "replace"
test_cmd "echo input > in; cat<in" "input redir"
test_cmd "echo hello 1>&2" "dup output"
test_cmd "cat <&0" "dup input"
test_cmd "echo word>file" "word>word"
test_cmd "echo word>>file" "word>>word"
test_cmd "echo a|cat" "pipe"
test_cmd "echo a>out|cat out" "redir then pipe"

echo "###################################################"
echo "STEP 2 - REDIR RESTORATION"
echo "###################################################"

test_cmd "echo hello > /tmp/42sh_rest; echo world; cat /tmp/42sh_rest" "redir restoration"
test_cmd "ls /nonexistent_file 2>&1 > /dev/null" "add stderr in stdout"
test_cmd "echo multi > /tmp/42sh_m1 > /tmp/42sh_m2; cat /tmp/42sh_m1; cat /tmp/42sh_m2" "multiple output redir"
test_cmd "echo 'input' > /tmp/42sh_in; cat < /tmp/42sh_in > /tmp/42sh_out; cat /tmp/42sh_out" "double redir"

echo "###################################################"
echo "STEP 2 - PIPELINES"
echo "###################################################"

test_cmd "echo hello | cat" "simple pipe"
test_cmd "echo test | cat | cat" "double pipe"
test_cmd "echo a | cat | cat | cat" "triple pipe"
test_cmd "echo multiple words | cat" "pipe multiple words"
test_cmd "echo 'quoted' | cat" "pipe quoted"
test_cmd "echo a | cat | cat | cat | cat | cat" "long pipeline"
test_cmd "true | false" "pipe true false (exit 1)"
test_cmd "false | true" "pipe false true (exit 0)"
test_cmd "echo test | grep test > /dev/null" "pipe with grep"
test_cmd "cat /etc/hostname | cat" "cat pipe cat"

echo "###################################################"
echo "STEP 2 - PIPELINE STATUS"
echo "###################################################"

test_cmd "true | true | false; echo \$?" "pipe status last command"
test_cmd "false | false | true; echo \$?" "pipe status propagation"
test_cmd "! true | false; echo \$?" "neg pipestatus"
test_cmd "false && echo 'no' | cat" "pipe with and"
test_cmd "true || echo 'no' | cat" "pipe with or"

echo "###################################################"
echo "STEP 2 - NEGATION"
echo "###################################################"

test_cmd "! false" "negate false"
test_cmd "! true" "negate true"
test_cmd "! false && echo ok" "negation in AND"
test_cmd "! true || echo ok" "negation in OR"
test_cmd "! echo test | cat" "negation with pipe"
test_error "! ! true" "double negation"

echo "###################################################"
echo "STEP 2 - WHILE LOOPS"
echo "###################################################"

test_cmd "while false; do echo loop; done" "while false no exec"
test_cmd "while false; do echo a; echo b; done" "while false multiple"
test_cmd "while false; do echo never; done; echo after" "while then command"
test_cmd "while false && false; do echo no; done; echo after" "while AND condition"
test_cmd "while false || false; do echo no; done; echo after" "while OR condition"

echo "###################################################"
echo "STEP 2 - UNTIL LOOPS"
echo "###################################################"

test_cmd "until true; do echo loop; done" "until true no exec"
test_cmd "until true; do echo a; echo b; done" "until true multiple"
test_cmd "until true; do echo never; done; echo after" "until then command"
test_cmd "until true && true; do echo no; done; echo after" "until AND condition"
test_cmd "until true || false; do echo no; done; echo after" "until OR condition"

echo "###################################################"
echo "STEP 2 - AND/OR OPERATORS"
echo "###################################################"

test_cmd "true && echo success" "AND true exec"
test_cmd "false && echo fail" "AND false no exec"
test_cmd "false || echo fallback" "OR false exec"
test_cmd "true || echo not_shown" "OR true no exec"
test_cmd "true && true && echo ok" "multiple AND"
test_cmd "false || false || echo final" "multiple OR"
test_cmd "true && false || echo rescue" "AND then OR"
test_cmd "false && echo no || echo yes" "AND OR combo"
test_cmd "echo start && echo end" "AND with echos"
test_cmd "echo start || echo end" "OR with echo"
test_cmd "true && true && true && echo ok" "long AND chain"
test_cmd "false || false || false || echo ok" "long OR chain"
test_cmd "echo a && echo b || echo c" "mixed operators"
test_cmd "false && echo a || echo b && echo c" "complex chain"

echo "###################################################"
echo "STEP 2 - DOUBLE QUOTES"
echo "###################################################"

test_cmd 'echo "hello"' "double quotes simple"
test_cmd 'echo "hello world"' "double quotes with space"
test_cmd 'echo "a b c"' "double quotes multiple words"
test_cmd 'echo ""' "empty double quotes"
test_cmd 'echo a"b"c' "concatenation with double quotes"
test_cmd 'echo "double quote text"' "long double quoted text"

echo "###################################################"
echo "STEP 2 - ESCAPE CHARACTER"
echo "###################################################"

test_cmd "echo \\#escaped" "escape hash"
test_cmd 'echo hello \
world' "line continuation"
test_cmd "echo test\\ ing" "escape space"

echo "###################################################"
echo "STEP 2 - VARIABLES BASIC"
echo "###################################################"

test_cmd 'x=hello; echo "$x"' "assign and expand"
test_cmd 'x=hello; y=world; echo "$x" "$y"' "multiple variables"
test_cmd 'var=test; echo "$var"' "simple expansion"
test_cmd 'x=a; y=b; z=c; echo "$x$y$z"' "concatenate variables"
test_cmd 'name=value; echo prefix"$name"' "prefix concatenation"
test_cmd 'name=value; echo "${name}"' "braces expansion"
test_cmd 'x=hello; echo "$x" world' "variable with text after"
test_cmd 'x=; echo "$x"' "empty variable"
test_cmd 'x=test; echo "$x$x"' "same variable twice"
test_cmd 'x=test; x=new; echo "$x"' "reassign variable"
test_cmd 'x=a; echo "$x"; x=b; echo "$x"' "change value"
test_cmd 'a=1 b=2; echo $a $b' "multiple assignment"
test_cmd 'echo $1var' "digit starts word"

test_error "1var=value" "starts with digit"
test_error "var-name=value" "contains hyphen"
test_error "var.name=value" "contains dot"
test_error "var@=value" "contains special char"
test_error "=value" "empty variable name"
test_error 'echo ${1var}' "invalid name in braces (should be grammar error)"

echo "###################################################"
echo "STEP 2 - VARIABLES RECURSIVE"
echo "###################################################"

test_cmd 'a=1; b=$a; echo "$b"' "recursive simple"
test_cmd 'a=hello; b=$a; c=$b; echo "$c"' "triple recursive"
test_cmd 'a=1; b=$a"2"; c=$b"3"; echo "$c"' "recursive with concat"
test_cmd 'a=1; a=$a; echo "$a"' "self reference"
test_cmd 'a=1; b=2; c=$a$b; echo "$c"' "multi-variable concat"
test_cmd 'x=5; y=$x; x=10; echo "$y"' "snapshot test"
test_cmd 'a=hello; b=world; c=$a$b; echo "$c"' "concat two vars"
test_cmd 'a=1; b=$a; c=$b; d=$c; echo "$d"' "deep recursive"

echo "###################################################"
echo "STEP 2 - SPECIAL VARIABLES"
echo "###################################################"

test_cmd "true; echo \$?" "exit status true"
test_cmd "false; echo \$?" "exit status false"
test_cmd 'echo $#' "argc no args"
test_cmd 'echo $@' "all args empty"
test_cmd 'echo $*' "all args star empty"
test_cmd 'echo ${1}' "positional 1 empty"
test_cmd 'echo "$VAR_NOT_EXIST"' "nonexistent variable"

echo "###################################################"
echo "STEP 2 - VAR EXPANSION & QUOTING"
echo "###################################################"

test_cmd "VAR=WORLD; echo \"HELLO '\$VAR'\"" "expansion inside single-in-double quotes"
test_cmd "echo \"\\\$ \\\" \\\\\"" "escaped special chars in double quotes"
test_cmd "var=123; echo \${var}456" "braces expansion concatenation"
test_cmd "EMPTY_VAR=; echo \"begin\${EMPTY_VAR}end\"" "empty variable in quotes"

echo "###################################################"
echo "STEP 2 - SPECIAL PARAMETERS"
echo "###################################################"

test_cmd "echo \$UID | grep -E '^[0-9]+$' > /dev/null && echo 'is_numeric'" "UID"
test_cmd "! false; echo \$?" "exit status"

echo "###################################################"
echo "STEP 2 - IDENTIFIER VALIDATION"
echo "###################################################"

test_error 'echo ${1var}' "invalid name starting with digit"
test_error 'echo ${var-hyphen}' "invalid hyphen"
test_error 'echo ${}' "empty name"
test_error '1var=value' "invalid name"
test_error 'for 1i in a; do echo $1i; done' "invalid identifier"

echo "###################################################"
echo "STEP 2 - FOR LOOPS"
echo "###################################################"

test_cmd 'for i in a b c; do echo "$i"; done' "for simple"
test_cmd 'for i in 1 2 3; do echo "$i"; done' "for with numbers"
test_cmd 'for word in hello world test; do echo "$word"; done' "for with words"
test_cmd 'for i in a; do echo "$i"; done' "for single element"
test_cmd 'for i in; do echo "$i"; done' "for no elements"
test_cmd 'for i in a b c; do echo "$i"; echo next; done' "for multiple commands"
test_cmd 'for i in x y; do for j in 1 2; do echo "$i$j"; done; done' "for nested"
test_cmd 'for i in a b c; do echo "$i"; done; echo after' "for then command"
test_cmd 'for i in one two three; do echo -n "$i"; done' "for with -n"
test_cmd 'for var in aa bb cc; do echo test "$var"; done' "for with prefix"
test_cmd 'for i in a b; do true && echo "$i"; done' "for with AND"
test_cmd 'for i in a b; do echo "$i" | cat; done' "for with pipe"
test_cmd "for i in 'hello world' test; do echo \"\$i\"; done" "for with quoted arg"

test_error "for 1i in a b; do echo \$1i; done" "for: identifier starts with digit"
test_error "for i-2 in a b; do echo \$i-2; done" "for: identifier with hyphen"
test_error "for in a b; do echo ok; done" "for: missing identifier"

#----------------- STEP 3: ADVANCED BUILTINS & CONSTRUCTS -----------------#

echo "###################################################"
echo "STEP 3 - FOR LOOPS WITH POSITIONAL PARAMETERS"
echo "###################################################"

test_cmd 'f() { for i in $@; do echo "$i"; done; }; f a b c' "for in \$@ (unquoted)"
test_cmd 'f() { for i in "$@"; do echo "$i"; done; }; f "a b" c' "for in \"\$@\" (quoted, preserving spaces)"

test_cmd 'f() { for i in $*; do echo "$i"; done; }; f a b c' "for in \$* (unquoted)"
test_cmd 'f() { for i in "$*"; do echo "$i"; done; }; f a b c' "for in \"\$*\" (quoted, single string)"

test_cmd 'f() { for i; do echo "$i"; done; }; f x y z' "for default 'in \$@'"

test_cmd 'f() { for i in "$@"; do echo "[$i]"; done; }; f' "for in \"\$@\" with no args"
test_cmd 'f() { for i in "$@"; do echo "$i"; done; }; f "" " "' "for in \"\$@\" with empty/space args"

echo "###################################################"
echo "STEP 3 - EXIT BUILTIN"
echo "###################################################"

test_cmd "exit 0" "exit 0"
test_cmd "exit 1" "exit 1"
test_cmd "exit 42" "exit custom code"
test_cmd "exit 255" "exit max byte"
test_cmd "exit 256" "exit overflow"
test_cmd "exit -1" "exit negative"
test_cmd "exit 1000" "exit large number"
test_cmd "echo a; exit 3; echo b" "exit stops execution"
test_cmd "if true; then exit 5; fi; echo fail" "exit in if"
test_cmd "(exit 12); echo \$?" "exit in subshell"

echo "###################################################"
echo "STEP 3 - EXIT STATUS & PIPE"
echo "###################################################"

test_cmd "(exit 42); echo \$?" "42" "subshell exit status"
test_cmd "f() { return 10; }; f; echo \$?" "10" "function return status"

echo "###################################################"
echo "STEP 3 - CD BUILTIN"
echo "###################################################"

test_cmd "cd /tmp; pwd" "cd absolute path"
test_cmd "cd /; pwd" "cd root"
test_cmd "cd /tmp && cd .. && pwd" "cd relative .."
test_cmd "cd . && pwd" "cd dot"
test_cmd "mkdir -p /tmp/test_cd_42sh/a/b; cd /tmp/test_cd_42sh/a/b; pwd" "cd deep path"
test_cmd "cd /tmp; echo \$PWD" "cd updates PWD"
test_cmd "cd /; cd /tmp; echo \$OLDPWD" "cd updates OLDPWD"
test_cmd "cd /nonexistent_xyz 2>/dev/null || echo fail" "cd nonexistent"
test_cmd "touch /tmp/not_dir_42sh; cd /tmp/not_dir_42sh 2>/dev/null || echo fail" "cd on file"
test_cmd "cd /tmp; cd -; pwd" "cd dash"
test_cmd "cd /tmp; cd; pwd" "cd no args"
test_cmd "unset PWD; cd /tmp; pwd" "cd without PWD"

echo "###################################################"
echo "STEP 3 - CD & ENVIRONMENT SYNC"
echo "###################################################"

test_cmd "cd /tmp; FIRST=\$PWD; cd /; echo \"\$OLDPWD\" | grep -q \"\$FIRST\" && echo 'OLDPWD_OK'" "cd updates OLDPWD"
test_cmd "cd /tmp; cd /; cd - > /dev/null; pwd" "cd dash restoration"
test_cmd "mkdir -p /tmp/test_dir; ln -s /tmp/test_dir /tmp/link; cd /tmp/link; echo \"\$PWD\"" "path with symlink"

echo "###################################################"
echo "STEP 3 - EXPORT BUILTIN"
echo "###################################################"

test_cmd "export MYVAR=test; env | grep MYVAR" "export basic"
test_cmd "MYVAR=test; export MYVAR; env | grep MYVAR" "export existing var"
test_cmd "export VAL; VAL=changed; env | grep VAL" "export then modify"
test_cmd "export X=1; sh -c 'echo \$X'" "export to child"
test_cmd "X=1; sh -c 'echo \$X'" "no export no child"
test_cmd "export EMPTY=; env | grep EMPTY" "export empty"
test_cmd "export _VAR123=ok; echo \$_VAR123" "export special name"
test_cmd "export VAR='a b c'; sh -c 'echo \$VAR'" "export with spaces"

echo "###################################################"
echo "STEP 3 - EXPORT PERSISTENCE"
echo "###################################################"

test_cmd "export GLOBAL_VAR=42; (echo \$GLOBAL_VAR)" "export visible in subshell"
test_cmd "export ENV_VAR=hello; env | grep -q 'ENV_VAR=hello' && echo 'passed'" "export to external env"
test_cmd "export NEW_VAR=val; echo \$NEW_VAR" "export with assignment"

echo "###################################################"
echo "STEP 3 - UNSET BUILTIN"
echo "###################################################"

test_cmd 'x=1; unset x; echo "$x"' "unset variable"
test_cmd 'export x=1; unset x; echo "$x"' "unset exported"
test_cmd "unset NONEXISTENT" "unset nonexistent"
test_cmd 'x=1 y=2; unset x y; echo $x $y' "unset multiple"
test_cmd "f() { echo ok; }; unset -f f; f 2>/dev/null || echo deleted" "unset function"
test_cmd 'x=1; unset -v x; echo $x' "unset with -v"
test_cmd 'f() { echo ok; }; unset f; f 2>/dev/null || echo deleted' "unset function without flag"
test_cmd 'f() { echo ok; }; f=1; unset f; f' "unset var same name function"

echo "###################################################"
echo "STEP 3 - VARIABLE SCOPE (UNSET & LOCAL)"
echo "###################################################"

test_cmd "VAR=val; (unset VAR; echo \"sub:\$VAR\"); echo \"parent:\$VAR\"" "sub:
parent:val" "unset in subshell isolation"

echo "###################################################"
echo "STEP 3 - CONTINUE/BREAK"
echo "###################################################"

test_cmd "for i in 1 2 3; do continue; echo \$i; done" "continue in for"
test_cmd "for i in 1 2 3; do if true; then break; fi; echo \$i; done" "break in for"
test_cmd "while true; do break; echo fail; done; echo out" "break in while"
test_cmd "for i in 1 2; do for j in a b; do if [ \$j = a ]; then break; fi; echo \$i\$j; done; done" "break nested default"
test_cmd "for i in 1 2; do for j in a b; do if [ \$j = a ]; then break 1; fi; echo \$i\$j; done; done" "break 1 explicit"
test_cmd "for i in 1 2; do for j in a b; do break 2; echo fail; done; echo fail2; done" "break 2 nested"
test_cmd "for i in 1 2; do continue 1; echo fail; done" "continue 1 explicit"

echo "###################################################"
echo "STEP 3 - CONTINUE & BREAK IN NESTED LOOPS"
echo "###################################################"

test_cmd "for i in 1 2; do for j in a b; do echo \$i\$j; break 2; done; done" "break 2 nested loops"
test_cmd "for i in 1 2; do for j in a b; do continue 2; echo 'fail'; done; done" "continue 2 nested loops"

echo "###################################################"
echo "STEP 3 - COMMAND BLOCKS"
echo "###################################################"

test_cmd "{ echo a; echo b; }" "block simple"
test_cmd "{ echo a; } > /tmp/block_42sh; cat /tmp/block_42sh" "block redirect out"
test_cmd "echo a | { cat; }" "block pipe in"
test_cmd "{ echo a; exit 0; echo b; }" "block with exit"
test_cmd "{ { echo nested; }; }" "block nested"
test_cmd "{ echo a; echo b; } | cat" "block pipe out"
test_cmd "if true; then { echo inside; }; fi" "block in if"
test_cmd "{ var=changed; }; echo \$var" "block modifies var"
test_cmd "{ echo a; } | { cat; }" "pipe between blocks"

echo "###################################################"
echo "STEP 3 - SUBSHELLS"
echo "###################################################"

test_cmd "(echo a)" "subshell simple"
test_cmd "(exit 42); echo \$?" "subshell exit code"
test_cmd "var=1; (var=2; echo \$var); echo \$var" "subshell isolation"
test_cmd "(cd /tmp; pwd); pwd" "subshell cd isolation"
test_cmd "(echo a; echo b) | cat" "subshell pipe out"
test_cmd "echo input | (cat)" "subshell pipe in"
test_cmd "( (echo nested) )" "subshell nested"
test_cmd "if true; then (echo inside); fi" "subshell in if"
test_cmd "x=1; (x=2; (x=3; echo \$x); echo \$x); echo \$x" "subshell deep nesting"
test_cmd "( ( ( echo deep ) ) )" "triple subshell"
test_cmd "(exit 12) || echo fail" "subshell exit propagation"
test_cmd "(exit 0) && echo ok" "subshell exit 0 propagation"

echo "###################################################"
echo "STEP 3 - SUBSHELL ISOLATION"
echo "###################################################"

test_cmd "VAR=parent; (VAR=child; echo \$VAR); echo \$VAR" "subshell variable isolation"
test_cmd "START=\$PWD; (cd /tmp); if [ \"\$PWD\" = \"\$START\" ]; then echo 'cd_isolated'; fi" "subshell cd isolation"
test_cmd "(exit 123); echo \$?" "subshell exit status"

echo "###################################################"
echo "STEP 3 - FUNCTIONS BASIC"
echo "###################################################"

test_cmd "myfunc() { echo hello; }; myfunc" "function simple"
test_cmd "f() { echo \$1; }; f argument" "function arg \$1"
test_cmd "f() { echo \$#; }; f a b c" "function argc"
test_cmd "f() { echo \$@; }; f a b c" "function all args"
test_cmd "f() { return 42; }; f; echo \$?" "function return"
test_cmd "f() { echo a; }; f | cat" "function pipe"
test_cmd "x=1; f() { x=2; }; f; echo \$x" "function side effect"
test_cmd "f() { echo local; }; f; f" "function multiple calls"
test_cmd "f() { g() { echo inner; }; g; }; f" "function in function"
test_cmd "f() { if true; then echo yes; fi; }; f" "function with if"
test_cmd "f() { echo start; return 0; echo end; }; f" "function return stops"

echo "###################################################"
echo "STEP 3 - FUNCTIONS ADVANCED"
echo "###################################################"

test_cmd "f() { f() { echo v2; }; echo v1; }; f; f" "function redefine itself"
test_cmd "f() { echo a; }; f > /tmp/f_42sh; cat /tmp/f_42sh" "function redirect"
test_cmd "f() { { echo a; echo b; } > /tmp/fg_42sh; }; f; cat /tmp/fg_42sh" "function group redirect"
test_cmd "f() { if true; then return 10; fi; echo fail; }; f; echo \$?" "function return in if"
test_cmd "f() { echo \$*; }; f 'a b' c" "function args with spaces"
test_cmd "x=1; f() { x=2; }; (f); echo \$x" "function in subshell"
test_cmd "f() { echo pre; return 0; echo post; }; f && echo success" "function return logic"
test_cmd "f() { echo start; f() { echo nested; }; }; f" "function nested def"
test_cmd "f() { return \$1; }; f 5 && echo no || echo yes" "function dynamic return"
test_cmd "f() { echo a; }; var=\$(f); echo \$var" "function capture output"

echo "###################################################"
echo "STEP 3 - FUNCTION PERSISTENCE"
echo "###################################################"

test_cmd "f() { echo 'alive'; }; f; f" "function double call"
test_cmd "outer() { inner() { echo 'nested'; }; }; outer; inner" "nested function definition"
test_cmd "f() { echo 'to_file'; }; f > /tmp/42sh_func_out; cat /tmp/42sh_func_out" "function output redirection"

echo "###################################################"
echo "STEP 3 - COMMAND SUBSTITUTION"
echo "###################################################"

test_cmd "echo \$(echo hello)" "command sub basic"
test_cmd "a=\$(echo test); echo \$a" "command sub assign"
test_cmd "echo \$(echo a; echo b)" "command sub multiline"
test_cmd "echo \"\$(echo a)\"" "command sub in quotes"
test_cmd "echo '\$(echo a)'" "command sub in single quotes"
test_cmd "echo \$(echo \$(echo nested))" "command sub nested"
test_cmd "echo \$(echo a | cat)" "command sub with pipe"
test_cmd "echo \$(exit 42); echo \$?" "command sub exit status"
test_cmd "echo \$(echo a)b" "command sub concat after"
test_cmd "echo a\$(echo b)" "command sub concat before"
test_cmd "echo \$(cat /etc/hostname)" "command sub cat file"
test_cmd "x=\$(echo a b); echo \"\$x\"" "command sub spaces"
test_cmd "echo \$( echo \$( echo \$( echo deep ) ) )" "command sub triple nested"
test_cmd "echo \"\$( echo 'a b c' )\"" "command sub quotes spaces"
test_cmd "x=\$( echo a; exit 33; echo b ); echo res:\$x ret:\$?" "command sub exit code"
test_cmd "echo start \$( echo middle ) end" "command sub in middle"
test_cmd "echo \$(if true; then echo if; fi)" "command sub if"
test_cmd "echo \$(for i in 1 2; do echo \$i; done)" "command sub for"
test_cmd "x=\$(echo -n); echo \"|\$x|\"" "command sub empty"
test_cmd "echo \$(cat /dev/null)" "command sub null"
test_cmd "f() { echo \$(echo inner); }; f" "command sub in function"

echo "###################################################"
echo "STEP 3 - COMMAND SUBSTITUTION BACKQUOTE"
echo "###################################################"

test_cmd "echo '\$(echo no)'" "single quoted dollar sub"
test_cmd "echo '\`echo no\`'" "single quoted backtick sub"
test_cmd 'echo `test`' "backquote basic"
test_cmd 'echo "`test`"' "double quoted backquote"
test_cmd "echo '\`test\`'" "single quoted backquote"
test_cmd 'echo `echo simple`' "backquote simple"
test_cmd 'echo `echo \`echo nested\``' "nested backquote level 2"
test_error 'echo `echo \`echo \\\`deep\\\`\``' "nested backquote level 3"
test_cmd 'echo `echo $(echo a) and $(echo b)`' "mixed sub styles"
test_cmd 'a=test; echo $(echo "$a")' "var expansion in sub double quotes"
test_cmd "a=test; echo \$(echo '\$a')" "var expansion in sub single quotes (no expansion)"
test_cmd 'a=test; echo $(echo "x${a}y")' "brace expansion in sub"
test_cmd 'echo $(echo "(")' "special char in sub"
test_cmd 'echo $(echo "(test)")' "parens in sub"
test_cmd 'echo $( (echo subshell) )' "subshell inside cmd sub"
test_cmd '`echo echo` test' "backtick result as command"
test_cmd '`echo \`echo echo\`` test' "nested backtick result as command"
#test_cmd '`echo \`echo \\\`echo echo\\\`\`` test' "deep nested backtick command"
#test_cmd '`echo \`echo \\\`echo \\\\\\\`echo echo\\\\\\\`\\\`\`` test' "ultra deep backtick escaping"
test_error '`echo nested`' "backtick execution nested"
test_error '`echo \`echo level2\``' "backtick execution level 2"
#test_cmd '`echo \`echo \\\`echo level3\\\`\``' "backtick execution level 3"

echo "###################################################"
echo "STEP 3 - COMMAND SUBSTITUTION NESTING"
echo "###################################################"

test_cmd "echo \$(echo \$(echo deep))" "nested command substitution"
test_cmd "echo \"result: \$(echo 'spaced words')\"" "quoted command substitution"

echo "###################################################"
echo "STEP 3 - COMPLEX COMMAND SUBSTITUTION"
echo "###################################################"

test_cmd "echo \"\$(echo hello)\"" "hello" "cmdsub strips newline"
test_cmd "x=\$(echo 'multiple words'); echo \"\$x\"" "multiple words" "cmdsub assignment"
test_cmd "echo \$(echo hello > /tmp/42sh_sub; cat /tmp/42sh_sub)" "hello" "cmdsub with internal redirect"

echo "###################################################"
echo "STEP 3 - DOT COMMAND"
echo "###################################################"

echo "echo sourced_var=ok" >/tmp/source_test_42sh.sh
test_cmd ". /tmp/source_test_42sh.sh; echo \$sourced_var" "dot simple"
test_cmd "myfunc() { . /tmp/source_test_42sh.sh; }; myfunc; echo \$sourced_var" "dot in function"
#test_cmd ". /nonexistent 2>/dev/null || echo fail" "dot nonexistent" comportement different avec -c et mode input #relou
test_cmd "x=1; echo 'x=2' > /tmp/s_42sh.sh; . /tmp/s_42sh.sh; echo \$x" "dot overwrite var"
test_cmd ". /dev/null" "dot empty file"

#----------------- COMPLEX COMBINATIONS -----------------#

echo "###################################################"
echo "STEP 3 - COMPLEX COMBINATIONS"
echo "###################################################"

test_cmd "echo a && echo b || echo c" "AND OR combo"
test_cmd "false && echo a || echo b && echo c" "complex AND OR"
test_cmd "echo a | cat && echo b" "pipe then AND"
test_cmd "true && echo a | cat" "AND then pipe"
test_cmd "if true; then echo a && echo b; fi" "if with AND"
test_cmd "if true && false; then echo no; else echo yes; fi" "if condition AND false"
test_cmd "if false || true; then echo yes; fi" "if condition OR true"
test_cmd "echo a; echo b && echo c; echo d" "mixed semicolon AND"
test_cmd "! false && echo a || echo b" "negation AND OR"
test_cmd "echo a | cat | cat && echo b" "pipes then AND"
test_cmd "if true; then echo a | cat; fi" "if with pipe"
test_cmd "echo start && echo middle | cat && echo end" "AND pipe AND"
test_cmd "true && echo a | cat | cat && echo b" "AND pipes AND"
test_cmd "echo test > /tmp/42sh_x; cat /tmp/42sh_x && echo ok" "redirect then AND"
test_cmd "echo a | cat && echo b | cat" "pipe AND pipe"
test_cmd "if true; then echo -n a; echo b; fi" "if with -n then echo"

echo "###################################################"
echo "STEP 3 - PIPES WITH REDIRECTIONS"
echo "###################################################"

test_cmd "echo test > /tmp/pr_42sh; cat /tmp/pr_42sh | cat" "file then pipe"
test_cmd "echo a | cat > /tmp/pc_42sh; cat /tmp/pc_42sh" "pipe then redirect"
test_cmd "echo hello | cat | cat > /tmp/ppc_42sh; cat /tmp/ppc_42sh" "pipes then redirect"
test_cmd "{ echo a; } > /tmp/b_42sh | cat /tmp/b_42sh" "block redirect pipe"

echo "###################################################"
echo "STEP 3 - QUOTE MIXING"
echo "###################################################"

test_cmd 'a=VAR; echo "$a" '\''$a'\' "mixed double single quotes"
test_cmd 'echo "'"'$USER'"'"' "single in double quotes"
test_cmd 'a=1; echo "\$a is $a"' "escaped dollar in double"
test_cmd "echo 'It'\''s working'" "escaped single in single"
test_cmd 'echo "test $(echo nested)"' "double quotes with command sub"

echo "###################################################"
echo "STEP 3 - EDGE CASES"
echo "###################################################"

test_cmd "" "empty input"
test_cmd "       " "only spaces"
test_cmd "if true;then echo a;fi" "if no spaces"
test_cmd "if true; then echo; fi" "if empty command"
test_cmd 'echo "$VAR_NOT_EXIST"' "nonexistent variable"
test_error 'echo """' "multiple double quotes"
test_cmd "echo ''''" "multiple single quotes"
test_cmd "abc=123; echo \${abc}" "braces expansion"
test_cmd "echo a | grep a | grep a | grep a | cat" "very long pipe"
test_cmd 'foo=bar; echo "$foo"_$foo' "mixed quotes raw"
test_cmd "cat < /dev/null" "input from null"

#----------------- STEP 3: ADVANCED BUILTINS & CONSTRUCTS -----------------#

echo "###################################################"
echo "STEP 4 - ALIAS BASIC"
echo "###################################################"

test_cmd "alias l='ls -F'
l | sort" "simple alias expansion"

test_cmd "alias greet='echo hello world'
greet" "alias with multiple words"

test_cmd "alias cls='clear'
alias cls='echo cleared'
cls" "redefining an alias"

test_cmd "alias my_true='true'
my_true && echo ok" "alias for a builtin"

echo "###################################################"
echo "STEP 4 - ALIAS NESTING & RECURSION"
echo "###################################################"

test_cmd "alias a='echo'
alias b='a'
b hello" "nested alias level 2"

test_cmd "alias l='ls -F'
alias ll='l -l'
ll | sort" "nested alias"

test_cmd "alias x='echo'
alias y='x'
alias z='y'
z deep" "deep nesting level 3"

test_cmd "alias a='b'; alias b='a'
a 2>/dev/null || echo 'recursion_guarded'" "indirect recursion guard"

echo "###################################################"
echo "STEP 4 - ALIAS GRAMMAR & OPERATORS"
echo "###################################################"

test_cmd "alias list_files='ls | cat'
list_files | sort" "alias containing a pipe"

test_cmd "alias check='true && echo ok'
check" "alias with AND operator"

test_cmd "alias start_if='if true; then'
start_if 
echo yes
fi" "partial grammar expansion"

echo "###################################################"
echo "ALIAS - EXPANSION & VARIABLES"
echo "###################################################"

test_cmd "x=hello; alias greet='echo \$x'
greet" "alias with variable reference"

test_cmd "alias show='echo \$PWD'
show" "alias with environment variable"

test_cmd "alias multi='echo one; echo two'
multi" "alias with multiple commands"

test_cmd "alias cmd='echo'
x=test; cmd \$x" "alias with argument expansion"

test_cmd "alias e='echo'
e 'hello world'" "alias with quoted argument"

echo "###################################################"
echo "ALIAS - QUOTING"
echo "###################################################"

test_cmd "alias quote=\"echo 'hello'\"
quote" "alias with single quotes inside double"

test_cmd "alias dquote='echo \"test\"'
dquote" "alias with double quotes inside single"

test_cmd "alias mixed='echo \"hello\" world'
mixed" "alias with mixed quotes"

test_cmd "alias empty=''
empty" "alias with empty value"

test_cmd "alias spaces='   echo   test   '
spaces" "alias with leading/trailing spaces"

echo "###################################################"
echo "ALIAS - PRINTING & LISTING"
echo "###################################################"

test_cmd "alias test_alias='echo test'
alias test_alias" "print single alias"

test_cmd "alias a='echo a'
alias b='echo b'
alias a; alias b" "print multiple aliases"

test_error "alias nonexistent" "print nonexistent alias"

test_cmd "alias x='test'
alias x='new'
alias x" "verify alias redefinition"

echo "###################################################"
echo "ALIAS - WITH REDIRECTIONS"
echo "###################################################"

test_cmd "alias redir='echo test > /tmp/alias_test'
redir; cat /tmp/alias_test; rm /tmp/alias_test" "alias with output redirect"

test_cmd "echo content > /tmp/alias_in
alias read_file='cat < /tmp/alias_in'
read_file; rm /tmp/alias_in" "alias with input redirect"

test_cmd "alias append='echo line >> /tmp/alias_append'
append; append; cat /tmp/alias_append; rm /tmp/alias_append" "alias with append redirect"

echo "###################################################"
echo "ALIAS - WITH COMMAND SUBSTITUTION"
echo "###################################################"

test_cmd "alias sub='echo \$(echo nested)'
sub" "alias with command substitution"

test_cmd "alias date_cmd='echo \$(date +%Y)'
date_cmd" "alias with command sub and args"

test_cmd "alias multi_sub='echo \$(echo one) \$(echo two)'
multi_sub" "alias with multiple command subs"

echo "###################################################"
echo "ALIAS - COMPLEX CHAINING"
echo "###################################################"

test_cmd "alias a='echo a'
alias b='a | cat'
b" "alias piping to another alias"

test_cmd "alias cmd='echo test'
alias full='cmd && echo ok'
full" "alias with AND after alias"

test_cmd "alias t='true'
alias f='false'
t && echo yes || echo no" "alias in conditional"

test_cmd "alias e='echo'
alias a='e'
alias b='a'
alias c='b'
c works" "four level nesting"

echo "###################################################"
echo "ALIAS - WITH LOOPS"
echo "###################################################"

test_cmd "alias printer='echo'
for i in a b c; do printer \$i; done" "alias in for loop"

test_cmd "alias test_true='true'
while test_true; do echo once; break; done" "alias in while condition"

test_cmd "alias counter='echo item'
for i in 1 2 3; do counter; done" "alias in loop body"

echo "###################################################"
echo "ALIAS - WITH CONDITIONALS"
echo "###################################################"

test_cmd "alias check='true'
if check; then echo yes; fi" "alias in if condition"

test_cmd "alias printer='echo success'
if true; then printer; fi" "alias in if body"

test_cmd "alias fail='false'
if fail; then echo no; else echo yes; fi" "alias false in if"

echo "###################################################"
echo "ALIAS - ARGUMENT HANDLING"
echo "###################################################"

test_cmd "alias e='echo'
e one two three" "alias with multiple arguments"

test_cmd "alias greet='echo hello'
greet world" "alias followed by argument"

test_cmd "alias cmd='echo start'
cmd middle end" "alias with appended arguments"

test_cmd "alias show='echo'
x=test; show \$x \$x" "alias with duplicate variable args"

echo "###################################################"
echo "ALIAS - SPECIAL CHARACTERS"
echo "###################################################"

test_cmd "alias question='echo ?'
question" "alias with question mark"

test_cmd "alias at='echo @'
at" "alias with at sign"

echo "###################################################"
echo "ALIAS - RECURSION PREVENTION"
echo "###################################################"

test_cmd "alias echo='echo ALIASED'
echo test 2>/dev/null || echo ok" "direct self-recursion"

test_cmd "alias a='b'
alias b='c'
alias c='a'
a 2>/dev/null || echo protected" "circular alias chain"

test_cmd "alias cmd='cmd arg'
cmd 2>/dev/null || echo prevented" "self-reference with args"

echo "###################################################"
echo "ALIAS - ERROR CASES"
echo "###################################################"

test_error "alias =value" "alias with no name"

test_error "alias 'invalid name'='test'" "alias with spaces in name"

test_error "alias |='test'" "alias with pipe character name"

test_error "alias ;='test'" "alias with semicolon name"

echo "###################################################"
echo "ALIAS - WORD SPLITTING"
echo "###################################################"

test_cmd "IFS=:; alias test='echo a:b:c'
test" "alias with custom IFS"

test_cmd "alias sp='echo a  b  c'
sp" "alias with multiple spaces"

test_cmd "alias tab='echo a	b	c'
tab" "alias with tabs"

echo "###################################################"
echo "ALIAS - WITH FUNCTIONS"
echo "###################################################"

test_cmd "myfunc() { echo func; }
alias f='myfunc'
f" "alias to function"

test_cmd "alias a='echo alias'
func() { a; }
func" "alias used in function"

test_cmd "func() { alias local='echo test'; }
func; local 2>/dev/null || echo ok" "alias defined in function"

echo "###################################################"
echo "ALIAS - EDGE CASES"
echo "###################################################"

test_cmd "alias nl='echo
echo'
nl" "alias with newline"

test_cmd "alias long='echo aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
long" "alias with very long value"

test_cmd "alias a1='echo a'; alias a2='echo b'; alias a3='echo c'
a1; a2; a3" "multiple aliases in sequence"

test_cmd "alias x='echo 1' y='echo 2' z='echo 3'
x; y; z" "multiple alias definitions"

test_cmd "alias same='test'
alias same='test'
alias same" "redefine with same value"

echo "###################################################"
echo "ALIAS - UNALIAS"
echo "###################################################"

test_cmd "alias test_unalias='echo test'
unalias test_unalias
test_unalias 2>/dev/null || echo removed" "unalias basic"

test_cmd "alias a='echo a'
alias b='echo b'
unalias a b
a 2>/dev/null || echo ok" "unalias multiple"

test_error "unalias nonexistent" "unalias nonexistent"

test_error "alias test='echo'
unalias test
alias test='new value'
test" "redefine after unalias"

test_cmd "alias x='y'
alias y='echo test'
unalias y
x 2>/dev/null || echo chain broken" "unalias breaks chain"

echo "###################################################"
echo "ALIAS - INTERACTION WITH BUILTINS"
echo "###################################################"

test_cmd "alias cd='echo fake cd'
cd /tmp 2>/dev/null || echo aliased" "alias overriding builtin"

test_cmd "alias export='echo fake export'
export 2>/dev/null; echo ok" "alias for export"

test_cmd "alias exit='echo not exiting'
exit; echo still here" "alias for exit (safe)"

echo "###################################################"
echo "ALIAS - WHITESPACE HANDLING"
echo "###################################################"

test_cmd "alias trimmed='  echo test  '
trimmed" "alias with space trimming"

test_cmd "alias tabs='		echo	test		'
tabs" "alias with tabs"

test_cmd "alias mixed='  	echo  	test  	'
mixed" "alias with mixed whitespace"

echo "###################################################"
echo "ALIAS - COMBINATION TESTS"
echo "###################################################"

test_cmd "alias e='echo'
x=hello; e \$x > /tmp/alias_combo; cat /tmp/alias_combo; rm /tmp/alias_combo" "alias with var and redirect"

test_cmd "alias cmd='echo test'
alias wrapper='cmd | cat'
wrapper" "alias wrapping alias with pipe"

test_cmd "alias a='echo a'
alias b='echo b'
a && b || echo fail" "multiple aliases with operators"

test_cmd "alias begin='if true; then'
begin echo nested; fi" "alias starting control structure"

echo "###################################################"
echo "IFS - DEFAULT IFS BEHAVIOR"
echo "###################################################"

# Default IFS is space, tab, and newline
test_cmd "x='a b c'; echo \$x" "default IFS variable expansion"
test_cmd "x='a	b	c'; echo \$x" "default IFS with tabs"
test_cmd "x='a
b
c'; echo \$x" "default IFS with newlines"
test_cmd "echo \$(echo 'a b c')" "default IFS command substitution"
test_cmd "x=\$(echo 'hello world'); echo \$x" "default IFS assign from command sub"

echo "###################################################"
echo "IFS - CUSTOM IFS VALUES"
echo "###################################################"

# Using colon as IFS
test_cmd "IFS=:; x='a:b:c'; echo \$x" "IFS colon separator"
test_cmd "IFS=:; echo \$(echo 'a:b:c')" "IFS colon command sub"
test_cmd "IFS=,; x='one,two,three'; echo \$x" "IFS comma separator"
test_cmd "IFS=/; x='usr/bin/ls'; echo \$x" "IFS slash separator"
test_cmd "IFS=.; x='file.txt.bak'; echo \$x" "IFS dot separator"

echo "###################################################"
echo "IFS - EMPTY IFS (NO SPLITTING)"
echo "###################################################"

# Empty IFS means no field splitting at all
test_cmd "IFS=; x='a b c'; echo \$x" "empty IFS preserves spaces"
test_cmd "IFS=''; x='hello world'; echo \$x" "empty IFS quoted preserves"
test_cmd "IFS=; echo \$(echo 'a b c')" "empty IFS command sub no split"
test_cmd "IFS=''; x=\$(echo 'one two three'); echo \$x" "empty IFS assign preserves"

echo "###################################################"
echo "IFS - WHITESPACE VS NON-WHITESPACE DELIMITERS"
echo "###################################################"

# Whitespace IFS chars (space, tab, newline) behave differently
test_cmd "IFS=' '; x='a  b  c'; echo \$x" "IFS space multiple delimiters"
test_cmd "IFS=':'; x='a::b::c'; echo \$x" "IFS colon multiple delimiters"
test_cmd "IFS=' '; x='  a  b  '; echo \$x" "IFS space leading trailing"
test_cmd "IFS=':'; x='::a::b::'; echo \$x" "IFS colon leading trailing"

echo "###################################################"
echo "IFS - LEADING AND TRAILING DELIMITERS"
echo "###################################################"

test_cmd "x='  hello  '; echo \$x" "default IFS trim whitespace"
test_cmd "x='	hello	'; echo \$x" "default IFS trim tabs"
test_cmd "IFS=:; x=':hello:'; echo \$x" "IFS colon preserve empty fields"
test_cmd "IFS=,; x=',a,b,'; echo \$x" "IFS comma empty fields"
test_cmd "x='
hello
'; echo \$x" "default IFS trim newlines"

echo "###################################################"
echo "IFS - MIXED WHITESPACE AND NON-WHITESPACE"
echo "###################################################"

test_cmd "IFS=' :'; x='a:b c:d'; echo \$x" "IFS mixed space colon"
test_cmd "IFS=': '; x='a: b :c'; echo \$x" "IFS mixed colon space order"
test_cmd "IFS=' ,'; x='a,b c,d'; echo \$x" "IFS mixed space comma"
test_cmd "IFS='	:'; x='a:b	c:d'; echo \$x" "IFS mixed tab colon"

echo "###################################################"
echo "IFS - COMMAND SUBSTITUTION BEHAVIOR"
echo "###################################################"

# Command substitution strips trailing newlines, then applies IFS
test_cmd "echo \$(echo a; echo b)" "command sub multiline default IFS"
test_cmd "echo \$(echo 'a b c')" "command sub spaces default IFS"
test_cmd "IFS=:; echo \$(echo 'a:b:c')" "command sub custom IFS"
test_cmd "x=\$(echo 'multiple words'); echo \$x" "assign from command sub"
test_cmd "x=\$(echo a; echo b); echo \$x" "assign multiline command sub"

echo "###################################################"
echo "IFS - LOOPS WITH IFS"
echo "###################################################"

test_cmd "IFS=:; for i in a:b:c; do echo \$i; done" "for loop with IFS colon"
test_cmd "IFS=,; for i in x,y,z; do echo \$i; done" "for loop with IFS comma"
test_cmd "for i in \$(echo 'a:b:c'); do echo \$i; done" "for loop command sub default IFS"
test_cmd "IFS=:; for i in \$(echo 'a:b:c'); do echo \$i; done" "for loop command sub custom IFS"

echo "###################################################"
echo "IFS - QUOTED CONTEXTS (NO SPLITTING)"
echo "###################################################"

# IFS should not split in quoted contexts
test_cmd "x='a b c'; echo \"\$x\"" "quoted variable no split"
test_cmd "IFS=:; x='a:b:c'; echo \"\$x\"" "quoted variable custom IFS no split"
test_cmd "echo \"\$(echo 'a b c')\"" "quoted command sub no split"
test_cmd "IFS=:; echo \"\$(echo 'a:b:c')\"" "quoted command sub custom IFS no split"
test_cmd "x='hello world'; echo \"test \$x end\"" "quoted mixed content no split"

echo "###################################################"
echo "IFS - VARIABLE ASSIGNMENTS (NO SPLITTING)"
echo "###################################################"

# Variable assignments should not perform field splitting
test_cmd "x=\$(echo 'one two three'); echo \$x" "assign command sub no split"
test_cmd "IFS=:; x=\$(echo 'a:b:c'); echo \$x" "assign command sub custom IFS no split"
test_cmd "y='hello world'; x=\$y; echo \$x" "assign from variable no split"
test_cmd "IFS=:; y='a:b:c'; x=\$y; echo \$x" "assign from variable custom IFS no split"

echo "###################################################"
echo "IFS - SPECIAL IFS CHARACTERS"
echo "###################################################"

test_cmd "IFS='|'; x='a|b|c'; echo \$x" "IFS pipe character"
test_cmd "IFS='&'; x='a&b&c'; echo \$x" "IFS ampersand character"
test_cmd "IFS=';'; x='a;b;c'; echo \$x" "IFS semicolon character"
test_cmd "IFS='*'; x='a*b*c'; echo \$x" "IFS asterisk character"

echo "###################################################"
echo "IFS - MULTIPLE CONSECUTIVE DELIMITERS"
echo "###################################################"

test_cmd "x='a    b    c'; echo \$x" "multiple spaces collapse"
test_cmd "x='a		b		c'; echo \$x" "multiple tabs collapse"
test_cmd "IFS=:; x='a:::b:::c'; echo \$x" "multiple colons create empty fields"
test_cmd "IFS=,; x='a,,,b,,,c'; echo \$x" "multiple commas create empty fields"

echo "###################################################"
echo "IFS - COMPLEX SCENARIOS"
echo "###################################################"

test_cmd "IFS=:; x='a:b c:d'; echo \$x" "IFS colon with spaces"
test_cmd "IFS=' :'; x='a: b :c  :d'; echo \$x" "complex mixed IFS"
test_cmd "x=\$(printf 'a\tb\nc'); echo \$x" "command sub with tab and newline"
test_cmd "IFS=:; for i in \$(echo 'x:y:z'); do echo \$i; done" "for loop complex IFS"
test_cmd "IFS=,; x=''; echo \"empty:\$x\"" "empty variable with custom IFS"

echo "###################################################"
echo "IFS - IFS RESTORATION"
echo "###################################################"

test_cmd "IFS=:; (IFS=' '; x='a b'; echo \$x); x='a:b'; echo \$x" "IFS subshell local"
test_cmd "OLD_IFS=\$IFS; IFS=:; x='a:b'; IFS=\$OLD_IFS; echo \$x" "IFS restore manual"

echo "###################################################"
echo "IFS - EDGE CASES"
echo "###################################################"

test_cmd "IFS=; x=''; echo \"result:\$x\"" "empty IFS empty variable"
test_cmd "x='   '; echo \"result:\$x\"" "variable only whitespace"
test_cmd "IFS=:; x=':'; echo \"result:\$x\"" "variable only IFS char"
test_cmd "IFS=' '; x='a	b'; echo \$x" "IFS space but tab in value"
test_cmd "IFS='
'; x='a
b
c'; echo \$x" "IFS newline separator"

test_cmd "x=\$(printf 'line1\nline2\nline3'); echo \$x" "command sub with newlines"

echo "###################################################"
echo "IFS - UNSET IFS (DEFAULT BEHAVIOR)"
echo "###################################################"

test_cmd "unset IFS; x='a b c'; echo \$x" "unset IFS uses default"
test_cmd "unset IFS; echo \$(echo 'a b c')" "unset IFS command sub default"
test_cmd "IFS=:; unset IFS; x='a:b'; echo \$x" "unset IFS after custom"

#----------------- SYNTAX ERRORS -----------------#

echo "###################################################"
echo "SYNTAX ERRORS"
echo "###################################################"

test_error ";" "lone semicolon"
test_error ";;" "double semicolon"
test_error "&&" "lone AND"
test_error "||" "lone OR"
test_error "|" "lone pipe"
test_error "echo &&" "AND without after"
test_error "&& echo test" "AND without before"
test_error "echo ||" "OR without after"
test_error "|| echo test" "OR without before"
test_error "echo |" "pipe without after"
test_error "| echo test" "pipe without before"
test_error "if" "incomplete if"
test_error "if true" "if without then"
test_error "if true; then" "if without fi"
test_error "if true; then echo test" "if then without fi"
test_error "while" "incomplete while"
test_error "while true" "while without do"
test_error "while true; do" "while without done"
test_error "while true; do echo test" "while do without done"
test_error "until" "incomplete until"
test_error "until false" "until without do"
test_error "until false; do" "until without done"
test_error "until false; do echo test" "until do without done"
test_error "for" "incomplete for"
test_error "for i" "for without in"
test_error "for i in" "for in without do"
test_error "for i in a b; do" "for without done"
test_error "for i in a b; do echo test" "for do without done"
test_error ">" "lone redirect out"
test_error ">>" "lone redirect append"
test_error "<" "lone redirect in"
test_error "2>" "lone redirect stderr"
test_error "echo >" "redirect without file"
test_error "echo >>" "redirect append without file"
test_error "< /tmp/file" "redirect without command"
test_error "!" "lone negation"
test_error "if; then echo test; fi" "if empty condition"
test_error "while; do echo test; done" "while empty condition"
test_error "until; do echo test; done" "until empty condition"

echo "###################################################"
echo "CASE - BASIC PATTERN MATCHING"
echo "###################################################"

test_cmd "x='hello'
case \"\$x\" in
  hello) echo 'match' ;;
esac" "exact string match"

test_cmd "x='test.txt'
case \"\$x\" in
  *.txt) echo 'text file' ;;
  *.pdf) echo 'pdf file' ;;
esac" "wildcard extension match"

test_cmd "x='file.pdf'
case \"\$x\" in
  *.txt) echo 'text' ;;
  *.pdf) echo 'pdf' ;;
  *) echo 'other' ;;
esac" "match second pattern"

test_cmd "x='unknown.xyz'
case \"\$x\" in
  *.txt) echo 'text' ;;
  *.pdf) echo 'pdf' ;;
  *) echo 'default' ;;
esac" "default catch-all pattern"

test_cmd "x='a'
case \"\$x\" in
  ?) echo 'single char' ;;
  ??) echo 'two chars' ;;
esac" "question mark pattern"

echo "###################################################"
echo "CASE - RANGE AND CHARACTER CLASSES"
echo "###################################################"

test_cmd "x='a'
case \"\$x\" in
  [a-z]) echo 'lowercase' ;;
  [A-Z]) echo 'uppercase' ;;
esac" "lowercase range match"

test_cmd "x='Z'
case \"\$x\" in
  [a-z]) echo 'lower' ;;
  [A-Z]) echo 'upper' ;;
  *) echo 'other' ;;
esac" "uppercase range match"

test_cmd "x='5'
case \"\$x\" in
  [0-9]) echo 'digit' ;;
  [a-z]) echo 'letter' ;;
esac" "digit range match"

test_cmd "x='hello123'
case \"\$x\" in
  [a-z]*) echo 'starts with letter' ;;
  [0-9]*) echo 'starts with digit' ;;
esac" "range with wildcard"

test_cmd "x='abc'
case \"\$x\" in
  [!0-9]*) echo 'not digit' ;;
  *) echo 'starts with digit' ;;
esac" "negation pattern [!...]"

echo "###################################################"
echo "CASE - POSIX CHARACTER CLASSES"
echo "###################################################"

test_cmd "x='5'
case \"\$x\" in
  [[:digit:]]) echo 'digit' ;;
  [[:alpha:]]) echo 'letter' ;;
esac" "digit class match"

test_cmd "x='a'
case \"\$x\" in
  [[:digit:]]) echo 'digit' ;;
  [[:alpha:]]) echo 'letter' ;;
esac" "alpha class match"

test_cmd "x='hello'
case \"\$x\" in
  [[:digit:]]*) echo 'digits' ;;
  [[:alpha:]]*) echo 'letters' ;;
esac" "alpha class with wildcard"

test_cmd "x='A'
case \"\$x\" in
  [[:lower:]]) echo 'lower' ;;
  [[:upper:]]) echo 'upper' ;;
esac" "upper class match"

test_cmd "x='test123'
case \"\$x\" in
  [[:alnum:]]*) echo 'alphanumeric' ;;
  *) echo 'other' ;;
esac" "alnum class match"

test_cmd "x=' '
case \"\$x\" in
  [[:space:]]) echo 'space' ;;
  *) echo 'not space' ;;
esac" "space class match"

echo "###################################################"
echo "CASE - MULTIPLE PATTERNS (ALTERNATIVES)"
echo "###################################################"

test_cmd "x='quit'
case \"\$x\" in
  quit|exit) echo 'exiting' ;;
  help) echo 'helping' ;;
esac" "pipe alternative match first"

test_cmd "x='exit'
case \"\$x\" in
  quit|exit) echo 'exiting' ;;
  help) echo 'helping' ;;
esac" "pipe alternative match second"

test_cmd "x='main.c'
case \"\$x\" in
  *.c|*.h) echo 'C source' ;;
  *.cpp|*.hpp) echo 'C++ source' ;;
  *) echo 'other' ;;
esac" "multiple wildcard alternatives"

test_cmd "x='header.h'
case \"\$x\" in
  *.c|*.h) echo 'C source' ;;
  *.cpp) echo 'C++' ;;
esac" "alternative wildcard match second"

test_cmd "x='test'
case \"\$x\" in
  foo|bar|baz) echo 'first' ;;
  test|check|verify) echo 'second' ;;
esac" "multiple word alternatives"

echo "###################################################"
echo "CASE - COMPLEX PATTERNS"
echo "###################################################"

test_cmd "x='archive.tar.gz'
case \"\$x\" in
  *.tar.gz) echo 'tarball' ;;
  *.tar) echo 'tar' ;;
  *.gz) echo 'gzip' ;;
esac" "double extension pattern"

test_cmd "x='a1'
case \"\$x\" in
  [a-z][0-9]) echo 'letter-digit' ;;
  [0-9][a-z]) echo 'digit-letter' ;;
esac" "letter followed by digit"

test_cmd "x='/home/user/test.txt'
case \"\$x\" in
  /home/*) echo 'home dir' ;;
  /tmp/*) echo 'tmp dir' ;;
  *) echo 'other' ;;
esac" "path pattern matching"

test_cmd "x='test_file_v2'
case \"\$x\" in
  *test*) echo 'contains test' ;;
  *file*) echo 'contains file' ;;
esac" "wildcard in middle of pattern"

test_cmd "x='a'
case \"\$x\" in
  [a-z][a-z]*) echo 'multi letter' ;;
  [a-z]) echo 'single letter' ;;
esac" "single vs multiple letters"

echo "###################################################"
echo "CASE - EMPTY AND EDGE CASES"
echo "###################################################"

test_cmd "x=''
case \"\$x\" in
  '') echo 'empty' ;;
  *) echo 'not empty' ;;
esac" "empty string match"

test_cmd "x='anything'
case \"\$x\" in
  *) echo 'catch all' ;;
esac" "universal match with *"

test_cmd "x='test'
case \"\$x\" in
  ?????) echo 'five chars' ;;
  ????) echo 'four chars' ;;
  *) echo 'other' ;;
esac" "exact length with ?"

test_cmd "case 'fixed' in
  fixed) echo 'inline match' ;;
esac" "inline case expression"

test_cmd "x='#comment'
case \"\$x\" in
  \\#*) echo 'starts with hash' ;;
  *) echo 'no hash' ;;
esac" "escaped special char"

echo "###################################################"
echo "CASE - NO MATCH SCENARIOS"
echo "###################################################"

test_cmd "x='nomatch'
case \"\$x\" in
  foo) echo 'foo' ;;
  bar) echo 'bar' ;;
esac" "no pattern matches - no output"

test_cmd "x='test'
case \"\$x\" in
  [A-Z]*) echo 'uppercase' ;;
  [0-9]*) echo 'digit' ;;
esac" "no match with ranges"

echo "###################################################"
echo "CASE - COMPOUND LISTS IN CASE ITEMS"
echo "###################################################"

test_cmd "x='multi'
case \"\$x\" in
  multi)
    echo 'line 1'
    echo 'line 2'
    ;;
esac" "multiple commands in case item"

test_cmd "x='test'
case \"\$x\" in
  test)
    echo 'first'
    echo 'second'; echo 'third'
    ;;
esac" "compound list with semicolons"

test_cmd "x='var'
case \"\$x\" in
  var)
    a=10
    echo \"\$a\"
    ;;
esac" "variable assignment in case"

echo "###################################################"
echo "CASE - MULTIPLE CASE ITEMS"
echo "###################################################"

test_cmd "x='b'
case \"\$x\" in
  a) echo 'first' ;;
  b) echo 'second' ;;
  c) echo 'third' ;;
esac" "match second of three items"

test_cmd "x='z'
case \"\$x\" in
  a) echo 'a' ;;
  b) echo 'b' ;;
  c) echo 'c' ;;
  *) echo 'default' ;;
esac" "fall through to default"

echo "###################################################"
echo "CASE - SEMICOLON VARIATIONS"
echo "###################################################"

test_cmd "x='test'
case \"\$x\" in
  test) echo 'match';;
esac" "no space before ;;"

test_cmd "x='test'
case \"\$x\" in
  test) echo 'match' ;;
  other) echo 'no' ;;
esac" "semicolon after last item"

test_cmd "x='first'
case \"\$x\" in
  first) echo 'one';;
  second) echo 'two'
esac" "missing ;; on last item"

echo "###################################################"
echo "CASE - EXIT STATUS"
echo "###################################################"

test_cmd "case 'match' in
  match) true ;;
esac
echo \$?" "exit status 0 on match"

test_cmd "case 'nomatch' in
  other) false ;;
esac
echo \$?" "exit status 0 on no match"

test_cmd "case 'test' in
  test) false ;;
esac
echo \$?" "exit status from matched command"

echo "###################################################"
echo "CASE - WITH LOOPS AND CONDITIONS"
echo "###################################################"

test_cmd "for i in a b c; do
  case \"\$i\" in
    a) echo 'first' ;;
    b) echo 'second' ;;
    c) echo 'third' ;;
  esac
done" "case inside for loop"

test_cmd "x='test'
if true; then
  case \"\$x\" in
    test) echo 'in if' ;;
  esac
fi" "case inside if statement"

test_cmd "x='yes'
case \"\$x\" in
  yes)
    if true; then
      echo 'nested if'
    fi
    ;;
esac" "if inside case"

echo "###################################################"
echo "CASE - PARENTHESES IN PATTERNS"
echo "###################################################"

test_cmd "x='test'
case \"\$x\" in
  (test) echo 'with parens' ;;
esac" "optional opening parenthesis"

test_cmd "x='foo'
case \"\$x\" in
  (foo|bar) echo 'alternative with parens' ;;
esac" "parenthesis with alternatives"

echo "###################################################"
echo "CASE - QUOTES IN PATTERNS"
echo "###################################################"

test_cmd "x='hello world'
case \"\$x\" in
  'hello world') echo 'quoted match' ;;
esac" "single quoted pattern with space"

test_cmd "x='test'
case \"\$x\" in
  \"test\") echo 'double quoted' ;;
esac" "double quoted pattern"

test_cmd "x='*'
case \"\$x\" in
  '*') echo 'literal star' ;;
  *) echo 'wildcard' ;;
esac" "quoted special character"

echo "###################################################"
echo "CASE - NESTED CASES"
echo "###################################################"

test_cmd "x='a'
y='1'
case \"\$x\" in
  a)
    case \"\$y\" in
      1) echo 'a1' ;;
      2) echo 'a2' ;;
    esac
    ;;
  b) echo 'b' ;;
esac" "nested case statements"

echo "###################################################"
echo "CASE - VARIABLES IN PATTERNS"
echo "###################################################"

test_cmd "pattern='*.txt'
x='file.txt'
case \"\$x\" in
  \"\$pattern\") echo 'var pattern' ;;
esac" "variable as pattern - literal match"

test_cmd "x='test'
case \"\$x\" in
  te*) echo 'starts with te' ;;
esac" "pattern with prefix"

echo "###################################################"
echo "CASE - REDIRECTIONS"
echo "###################################################"

test_cmd "x='test'
case \"\$x\" in
  test) echo 'redirected' > /tmp/case_test.txt ;;
esac
cat /tmp/case_test.txt
rm /tmp/case_test.txt" "redirection in case item"

test_cmd "x='input'
case \"\$x\" in
  input) cat < /dev/null ;;
esac" "input redirection in case"

echo "###################################################"
echo "CASE - NEWLINES AND FORMATTING"
echo "###################################################"

test_cmd "x='test'
case \"\$x\" in
  test)
    echo 'multiline'
    ;;
esac" "newline before ;;"

test_cmd "x='a'
case \"\$x\" in

  a) echo 'match' ;;

esac" "newlines around case items"

echo "###################################################"
echo "CASE - COMPLEX REAL WORLD EXAMPLES"
echo "###################################################"

test_cmd "file='script.sh'
case \"\$file\" in
  *.sh) echo 'shell script' ;;
  *.py) echo 'python' ;;
  *.js) echo 'javascript' ;;
  *.c|*.cpp|*.h) echo 'C/C++' ;;
  Makefile|*.mk) echo 'make' ;;
  *) echo 'unknown type' ;;
esac" "file type detector"

test_cmd "cmd='start'
case \"\$cmd\" in
  start|begin|run) echo 'starting...' ;;
  stop|end|quit) echo 'stopping...' ;;
  restart) echo 'restarting...' ;;
  status) echo 'checking status...' ;;
  *) echo 'unknown command' ;;
esac" "command dispatcher"

test_cmd "response='yes'
case \"\$response\" in
  y|yes|Y|YES) echo 'affirmative' ;;
  n|no|N|NO) echo 'negative' ;;
  *) echo 'invalid response' ;;
esac" "yes/no input handler"

#----------------- FILE EXECUTION -----------------#

echo "###################################################"
echo "FILE EXECUTION"
echo "###################################################"

mkdir -p /tmp/42sh_test_files

cat >/tmp/42sh_test_files/simple.sh <<'EOF'
echo "Hello from file"
echo "Line 2"
EOF
test_file "/tmp/42sh_test_files/simple.sh" "file simple"

cat >/tmp/42sh_test_files/variables.sh <<'EOF'
x=hello
y=world
echo "$x" "$y"
EOF
test_file "/tmp/42sh_test_files/variables.sh" "file with variables"

cat >/tmp/42sh_test_files/conditions.sh <<'EOF'
if true; then
    echo "condition true"
fi
if false; then
    echo "not shown"
else
    echo "condition false"
fi
EOF
test_file "/tmp/42sh_test_files/conditions.sh" "file with conditions"

cat >/tmp/42sh_test_files/loops.sh <<'EOF'
for i in a b c
do
    echo $i
done
EOF
test_file "/tmp/42sh_test_files/loops.sh" "file with loops"

cat >/tmp/42sh_test_files/pipes.sh <<'EOF'
echo "test" | cat
echo "redirect" > /tmp/42sh_redir_file
cat /tmp/42sh_redir_file
EOF
test_file "/tmp/42sh_test_files/pipes.sh" "file with pipes"

cat >/tmp/42sh_test_files/comments.sh <<'EOF'
# This is a comment
echo "visible"
echo "test" # inline comment
# Another comment
echo "end"
EOF
test_file "/tmp/42sh_test_files/comments.sh" "file with comments"

cat >/tmp/42sh_test_files/functions.sh <<'EOF'
myfunc() {
    echo "inside function"
    echo $1
}
myfunc hello
EOF
test_file "/tmp/42sh_test_files/functions.sh" "file with functions"

#----------------- STDIN MODE -----------------#

echo "###################################################"
echo "STDIN MODE"
echo "###################################################"

test_stdin "echo simple stdin" "stdin basic"
test_stdin "echo line1; echo line2" "stdin multiple commands"
test_stdin "if true; then echo yes; fi" "stdin if structure"
test_stdin "x=42; echo \$x" "stdin variables"
test_stdin "echo a | cat" "stdin pipeline"
test_stdin "      echo     spaces    " "stdin with spaces"
test_stdin "
echo newline
echo test" "stdin with newlines"
test_stdin "# comment only" "stdin comment"
test_stdin "" "stdin empty"
test_stdin "echo -n no_newline" "stdin no newline"
test_stdin "echo 'quote test'" "stdin quotes"
test_stdin "func() { echo inside; }; func" "stdin function"

####################################################


test_cmd "IFS=''; x='a b c'; for i in \$x; do echo \$i; done" "IFS empty disables splitting"
test_cmd "unset IFS; x='a b c'; for i in \$x; do echo \$i; done" "IFS unset restores default splitting"
test_cmd "IFS=:; for i in :a:b:c:; do echo \"[\$i]\"; done" "IFS leading and trailing empty fields"
test_cmd "IFS=' \n'; x='a b\nc'; for i in \$x; do echo \$i; done" "IFS space and newline combined"

test_cmd "x=world; echo 'hello \$x'" "single quotes prevent expansion"
test_cmd "x=world; echo \"hello \$x\"" "double quotes allow expansion"
test_cmd "echo \"'single inside double'\"" "single quotes inside double"
test_cmd "echo '\"double inside single\"'" "double quotes inside single"
test_cmd "x=''; echo \"\$x\"" "empty quoted variable"
test_cmd "echo '' ''" "multiple empty arguments"

test_cmd "echo test > /tmp/r1 2>/tmp/r2; cat /tmp/r1; rm /tmp/r1 /tmp/r2" "stdout redirection only"
test_cmd "echo test > /tmp/r1 > /tmp/r2; cat /tmp/r2; rm /tmp/r1 /tmp/r2" "last redirection wins"
test_cmd "f=/tmp/redir_var; echo hi > \"\$f\"; cat \$f; rm \$f" "redirect using variable"

test_cmd "false | true; echo \$?" "pipeline exit status last command"
test_cmd "true | false; echo \$?" "pipeline exit status false"
test_error "echo test | | cat" "double pipe syntax error"

test_cmd "if echo hi && true; then echo ok; fi" "if compound condition"

test_cmd "f() { false; }; f; echo \$?" "function return status"
test_cmd "x=global; f() { x=local; }; f; echo \$x" "function modifies global variable"
test_cmd "f() { echo hi; }; (f)" "function in subshell"

test_cmd "x=1; { x=2; }; echo \$x" "brace group affects parent"
test_cmd "x=1; ( x=2 ); echo \$x" "subshell does not affect parent"

test_cmd "echo hello \\
# comment
world" "line continuation ignores comment line"

test_cmd "echo hello # ignored" "inline comment ignored"

test_cmd "x=abc
case \"\$x\" in
  a*) echo first ;;
  ab*) echo second ;;
esac" "case first match wins"

test_cmd "x='a*b'
case \"\$x\" in
  a\\*b) echo literal ;;
  *) echo no ;;
esac" "escaped glob in pattern"

test_error "case x in esac" "case with no items"

test_stdin "" "stdin immediate EOF"

test_stdin "




echo ok" "stdin leading blank lines"

test_cmd "echo one
echo two" "newline separator"

test_cmd "echo test;" "trailing semicolon allowed"

test_cmd "(exit 42); echo \$?" "exit in subshell"

test_cmd ":; echo ok" "colon builtin"


#----------------- CLEANUP -----------------#

rm -rf /tmp/42sh_* /tmp/*_42sh* 2>/dev/null
rm -rf file in out empty_file

PERCENT=$(($SUCCESS * 100 / $TOTAL))
echo -e "${BBLU}Succeed:${GRN} $SUCCESS${WHT}"
echo -e "${BBLU}Failed:${RED} $(($TOTAL - $SUCCESS))${WHT}"
echo -e "${BBLU}Results:${BYEL} $PERCENT%${WHT}"

if [ -n "$OUTPUT_FILE" ]; then
  echo "$PERCENT" >"$OUTPUT_FILE"
fi

exit 0
