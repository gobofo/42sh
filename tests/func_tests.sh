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
LOG_FILE="test_failed"

if [ -z "$BIN_PATH" ]; then
  echo "Error: BIN_PATH not set"
  exit 0
fi

# Initialisation du fichier de log
echo "Test Failures Log - $(date)" >"$LOG_FILE"
echo "---------------------------------------------------" >>"$LOG_FILE"

test_cmd() {
  TOTAL=$((TOTAL + 1))
  local expected=$(timeout $TIMEOUT /bin/sh --posix -c "$1" 2>&1)
  local actual=$(timeout $TIMEOUT "$BIN_PATH" -c "$1" 2>&1)
  if [ "$expected" = "$actual" ]; then
    SUCCESS=$((SUCCESS + 1))
    echo -e "${GRN}[OK] $2${WHT}"
  else
    echo -e "${RED}[KO] $2${WHT}"
    {
      echo "Test: $2"
      echo "Command: $1"
      echo "Expected: $expected"
      echo "Actual:   $actual"
      echo "---------------------------------------------------"
    } >>"$LOG_FILE"
  fi
}

test_file() {
  TOTAL=$((TOTAL + 1))
  local expected=$(timeout $TIMEOUT bash --posix "$1" 2>&1)
  local actual=$(timeout $TIMEOUT "$BIN_PATH" "$1" 2>&1)
  if [ "$expected" = "$actual" ]; then
    SUCCESS=$((SUCCESS + 1))
    echo -e "${GRN}[OK] $2${WHT}"
  else
    echo -e "${RED}[KO] $2${WHT}"
    {
      echo "Test File: $2 ($1)"
      echo "Expected: $expected"
      echo "Actual:   $actual"
      echo "---------------------------------------------------"
    } >>"$LOG_FILE"
  fi
}

test_error() {
  TOTAL=$((TOTAL + 1))
  timeout $TIMEOUT "$BIN_PATH" -c "$1" >/dev/null 2>&1
  local exit_code=$?
  if [ $exit_code -ne 0 ]; then
    SUCCESS=$((SUCCESS + 1))
    echo -e "${GRN}[OK] $2${WHT}"
  else
    echo -e "${RED}[KO] $2${WHT}"
    {
      echo "Test Error: $2"
      echo "Command: $1"
      echo "Expected: non-zero exit code"
      echo "Actual: exit code $exit_code"
      echo "---------------------------------------------------"
    } >>"$LOG_FILE"
  fi
}

test_stdin() {
  TOTAL=$((TOTAL + 1))
  local expected=$(echo "$1" | timeout $TIMEOUT bash --posix 2>&1)
  local actual=$(echo "$1" | timeout $TIMEOUT "$BIN_PATH" 2>&1)
  if [ "$expected" = "$actual" ]; then
    SUCCESS=$((SUCCESS + 1))
    echo -e "${GRN}[OK] $2${WHT}"
  else
    echo -e "${RED}[KO] $2${WHT}"
    {
      echo "Test Stdin: $2"
      echo "Input: $1"
      echo "Expected: $expected"
      echo "Actual:   $actual"
      echo "---------------------------------------------------"
    } >>"$LOG_FILE"
  fi
}

if [ "$COVERAGE" = "yes" ]; then

  echo "###################################################"
  echo "RUNNING UNIT TESTS"
  echo "###################################################"

  if [ -f "unit/unit_tests" ]; then

    UNIT_OUT=$(./unit/unit_tests --verbose --color=never 2>&1)
    UNIT_EXIT=$?

    # Use awk to extract numbers reliably
    SYNTHESIS=$(echo "$UNIT_OUT" | grep "Synthesis:")
    PASSED=$(echo "$SYNTHESIS" | awk -F'Passing: ' '{print $2}' | awk '{print $1}')
    TESTED=$(echo "$SYNTHESIS" | awk -F'Tested: ' '{print $2}' | awk '{print $1}')

    if [ -n "$PASSED" ] && [ -n "$TESTED" ]; then
      SUCCESS=$((SUCCESS + PASSED))
      TOTAL=$((TOTAL + TESTED))
      echo -e "${GRN}Unit tests completed: $PASSED/$TESTED${WHT}"
    else
      # Fallback if parsing fails
      echo -e "${RED}Failed to parse Criterion output${WHT}"
      if [ $UNIT_EXIT -eq 0 ]; then
        SUCCESS=$((SUCCESS + 1))
      fi
      TOTAL=$((TOTAL + 1))
    fi

    # Log failing tests to file instead of stdout
    if [ $UNIT_EXIT -ne 0 ]; then
      echo -e "${RED}[KO] Some unit tests failed. See $LOG_FILE for details.${WHT}"
      {
        echo "### UNIT TEST FAILURES ###"
        ./unit/unit_tests --verbose --color=always
        echo "---------------------------------------------------"
      } >>"$LOG_FILE"
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
test_cmd 'echo $$ | grep -E "^[0-9]+$" > /dev/null && echo "is_pid"' "pid is number"
test_cmd 'echo $RANDOM | grep -E "^[0-9]+$" > /dev/null && echo "is_num"' "random is number"
test_cmd 'echo $UID | grep -E "^[0-9]+$" > /dev/null && echo "is_uid"' "uid is number"
test_cmd 'echo $@' "all args empty"
test_cmd 'echo $*' "all args star empty"
test_cmd 'echo ${1}' "positional 1 empty"
test_cmd 'echo "$VAR_NOT_EXIST"' "nonexistent variable"

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

#----------------- STEP 3: ADVANCED BUILTINS & CONSTRUCTS -----------------#

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
test_cmd "unset PWD; cd /tmp; pwd" "cd without PWD"

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
echo "STEP 3 - UNSET BUILTIN"
echo "###################################################"

test_cmd 'x=1; unset x; echo "$x"' "unset variable"
test_cmd 'x=1; unset x; env | grep "^x="' "unset from env"
test_cmd 'export x=1; unset x; echo "$x"' "unset exported"
test_cmd "unset NONEXISTENT" "unset nonexistent"
test_cmd 'x=1 y=2; unset x y; echo $x $y' "unset multiple"
test_cmd "f() { echo ok; }; unset -f f; f 2>/dev/null || echo deleted" "unset function"
test_cmd 'x=1; unset -v x; echo $x' "unset with -v"

echo "###################################################"
echo "STEP 3 - CONTINUE/BREAK"
echo "###################################################"

test_cmd "for i in 1 2 3; do continue; echo \$i; done" "continue in for"
test_cmd "for i in 1 2 3; do if true; then break; fi; echo \$i; done" "break in for"
test_cmd "x=0; while true; do break; echo fail; done; echo out" "break in while"
test_cmd "for i in 1 2; do for j in a b; do if [ \$j = a ]; then break; fi; echo \$i\$j; done; done" "break nested default"
test_cmd "for i in 1 2; do for j in a b; do if [ \$j = a ]; then break 1; fi; echo \$i\$j; done; done" "break 1 explicit"
test_cmd "for i in 1 2; do for j in a b; do break 2; echo fail; done; echo fail2; done" "break 2 nested"
test_cmd "for i in 1 2; do continue 1; echo fail; done" "continue 1 explicit"

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

#----------------- CLEANUP -----------------#

rm -rf /tmp/42sh_* /tmp/*_42sh* 2>/dev/null

PERCENT=$(($SUCCESS * 100 / $TOTAL))
echo -e "${BBLU}Succeed:${GRN} $SUCCESS${WHT}"
echo -e "${BBLU}Failed:${RED} $(($TOTAL - $SUCCESS))${WHT}"
echo -e "${BBLU}Results:${BYEL} $PERCENT%${WHT}"
echo -e "${BBLU}Failed tests log:${WHT} $LOG_FILE"

if [ -n "$OUTPUT_FILE" ]; then
  echo "$PERCENT" >"$OUTPUT_FILE"
fi

exit 0
