#!/bin/sh

#----------------- COLOR -----------------#
# 0 - No style | 1 - Bold
RED="\e[0;31m"
GRN="\e[0;32m"
BBLU="\e[1;34m"
WHT="\e[0m"
BYEL="\e[1;33m"

TIMEOUT=1

TOTAL=0
SUCCESS=0

if [ -z "$BIN_PATH" ]; then
    echo "Error: BIN_PATH not set"
    exit 0
fi

test_cmd()
{
	TOTAL=$((TOTAL + 1))
    
    local expected=$(timeout $TIMEOUT bash --posix -c "$1" 2>&1)
    local actual=$(timeout $TIMEOUT "$BIN_PATH" -c "$1" 2>&1)

	if [ "$expected" = "$actual" ]; then
		SUCCESS=$((SUCCESS + 1))
	else
		echo -e "${RED}$2${WHT}"
		echo -e "${RED}Expected:${WHT} $expected"
		echo -e "${RED}Actual:${WHT} $actual"
	fi
}

test_file()
{
	TOTAL=$((TOTAL + 1))
    
    local expected=$(timeout $TIMEOUT bash --posix "$1" 2>&1)
    local actual=$(timeout $TIMEOUT "$BIN_PATH" "$1" 2>&1)

	if [ "$expected" = "$actual" ]; then
		SUCCESS=$((SUCCESS + 1))
	else
		echo -e "${RED}$2${WHT}"
		echo -e "${RED}Expected:${WHT} $expected"
		echo -e "${RED}Actual:${WHT} $actual"
	fi
}

test_error()
{
	TOTAL=$((TOTAL + 1))
    
    timeout $TIMEOUT "$BIN_PATH" -c "$1" > /dev/null 2>&1
    local exit_code=$?

	if [ $exit_code -ne 0 ]; then
		SUCCESS=$((SUCCESS + 1))
	else
		echo -e "${RED}$2${WHT}"
		echo -e "${RED}Expected:${WHT} mauvais code retour"
		echo -e "${RED}Actual: exit code ${WHT}$exit_code"
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

echo

#----------------- TESTS SIMPLE COMMAND -----------------#
echo "###################################################"
echo "TESTS SIMPLE COMMAND"
echo "###################################################"

test_cmd "echo a;" "Test echo super simple"

test_cmd "echo a" "Test echo super simple sans ;"

test_cmd "echo a; echo b;" "Test echo double"

test_cmd "echo a; echo b" "Test echo double sans ;"

test_cmd "echo aa; echo bb; echo cc" "Test echo multiple"

test_cmd "echo aa; echo bb; echo cc" "Test echo multiple sans ;"

test_cmd "echo hello world" "Test echo avec plusieurs arguments"

test_cmd "echo 'hello world'" "Test echo avec guillemets simples"

test_cmd 'echo "hello world"' "Test echo avec guillemets doubles"

#----------------- TESTS ECHO OPTIONS -----------------#
echo "###################################################"
echo "TESTS ECHO OPTIONS"
echo "###################################################"

test_cmd "echo -n test" "Test echo avec option -n"

test_cmd "echo -n hello world" "Test echo -n avec plusieurs mots"

test_cmd "echo -n -n -n test" "Test echo avec multiple -n"

test_cmd "echo -e 'hello\nworld'" "Test echo -e avec retour ligne"

test_cmd "echo -e 'tab\there'" "Test echo -e avec tabulation"

test_cmd "echo -e 'back\\\\slash'" "Test echo -e avec backslash"

test_cmd "echo -e 'line1\nline2\nline3'" "Test echo -e avec plusieurs retours ligne"

test_cmd "echo -E 'no\nnewline'" "Test echo -E desactive echappements"

test_cmd "echo -n -e 'test\n'" "Test echo -n -e combines"

test_cmd "echo -e -n 'test\n'" "Test echo -e -n combines (ordre inverse)"

test_cmd "echo -e '\t\ttabs'" "Test echo -e avec doubles tabulations"

#--------------- TESTS ECHO ESCAPE CHAR ---------------#
echo "###################################################"
echo "TESTS ECHO WITH OPTIONS AND ESCAPED CHARACTERS"
echo "###################################################"

test_cmd 'echo -e "a\nb"' "Test newline in middle"

test_cmd 'echo -E "a\nb"' "Test newline to print"

test_cmd 'echo -e "tab:\tend"' "Test tabulation in echo"

#----------------- TESTS AND_OR (&&, ||) -----------------#
echo "###################################################"
echo "TESTS AND_OR (&&, ||)"
echo "###################################################"

test_cmd "true && echo success" "Test ET simple reussi"

test_cmd "false && echo fail" "Test ET simple echoue"

test_cmd "false || echo fallback" "Test OU simple reussi"

test_cmd "true || echo not_shown" "Test OU simple echoue"

test_cmd "true && true && echo ok" "Test ET multiple"

test_cmd "false || false || echo final" "Test OU multiple"

test_cmd "true && false || echo rescue" "Test combinaison ET OU"

test_cmd "false && echo no || echo yes" "Test ET puis OU"

test_cmd "true && echo a && echo b" "Test ET avec plusieurs echo"

test_cmd "false || echo a || echo b" "Test OU court-circuit"

test_cmd "echo start && echo end" "Test ET avec echo des deux côtes"

test_cmd "echo start || echo end" "Test OU avec echo qui reussit"

test_cmd "true && true && true && true && echo deep" "Test longue chaîne de ET"

test_cmd "false || false || false || echo last" "Test longue chaîne de OU"

test_cmd "echo a && echo b && echo c" "Test ET avec tous echo"

test_cmd "true && echo ok || echo ko" "Test ET OU sans echec"

#----------------- TESTS PIPELINE -----------------#
echo "###################################################"
echo "TESTS PIPELINE"
echo "###################################################"

test_cmd "echo hello | cat" "Test pipe simple"

test_cmd "echo test | cat | cat" "Test double pipe"

test_cmd "echo line1; echo line2 | cat" "Test pipe avec point-virgule"

test_cmd "echo a | cat | cat | cat" "Test triple pipe"

test_cmd "echo multiple words | cat" "Test pipe avec plusieurs mots"

test_cmd "echo 'quoted text' | cat" "Test pipe avec guillemets"

test_cmd "echo line1 | cat | cat | cat | cat | cat" "Test long pipeline"

#----------------- TESTS CAT -----------------#
echo "###################################################"
echo "TESTS CAT"
echo "###################################################"

test_cmd "cat /etc/hostname" "Test cat simple"

test_cmd "cat /etc/hostname | cat" "Test cat pipe cat"

test_cmd "cat /etc/hostname | cat | cat" "Test cat pipes multiples"

# Create temp file for cat tests
echo "line1" > /tmp/42sh_test_cat.txt
echo "line2" >> /tmp/42sh_test_cat.txt

test_cmd "cat /tmp/42sh_test_cat.txt" "Test cat fichier multilignes"

# Test cat -e
echo -e "test\n" > /tmp/42sh_test_cat_e.txt

test_cmd "cat -e /tmp/42sh_test_cat_e.txt" "Test cat -e affiche dollar"

echo -e "line1\nline2\n" > /tmp/42sh_test_cat_e2.txt

test_cmd "cat -e /tmp/42sh_test_cat_e2.txt" "Test cat -e lignes multiples"

test_cmd "echo hello | cat -e" "Test cat -e avec pipe"

#----------------- TESTS NEGATION -----------------#
echo "###################################################"
echo "TESTS NEGATION"
echo "###################################################"

test_cmd "! false" "Test negation false"

test_cmd "! true" "Test negation true (code retour seulement)"

test_cmd "! false && echo ok" "Test negation dans ET"

test_cmd "! true || echo ok" "Test negation dans OU"

test_cmd "! echo test | cat" "Test negation avec pipe"

test_cmd "! false && ! false && echo double" "Test double negation"

#----------------- TESTS IF -----------------#
echo "###################################################"
echo "TESTS IF"
echo "###################################################"

test_cmd "if true; then echo yes; fi" "Test if simple vrai"

test_cmd "if false; then echo no; fi" "Test if simple faux"

test_cmd "if true; then echo yes; else echo no; fi" "Test if else vrai"

test_cmd "if false; then echo yes; else echo no; fi" "Test if else faux"

test_cmd "if false; then echo a; elif true; then echo b; fi" "Test if elif"

test_cmd "if false; then echo a; elif false; then echo b; else echo c; fi" "Test if elif else"

test_cmd "if true; then echo a; echo b; fi" "Test if avec plusieurs commandes"

test_cmd "if true; then echo a; fi; echo b" "Test if suivi d'une commande"

test_cmd "if true; then if true; then echo nested; fi; fi" "Test if imbrique"

test_cmd "if false; then echo a; elif false; then echo b; elif true; then echo c; fi" "Test if avec plusieurs elif"

test_cmd "if true && true; then echo ok; fi" "Test if condition ET"

test_cmd "if false || true; then echo ok; fi" "Test if condition OU"

test_cmd "if ! false; then echo ok; fi" "Test if condition negation"

test_cmd "if true; then echo -n inline; fi" "Test if avec echo -n"

test_cmd "if true; then echo -e 'line1\\nline2'; fi" "Test if avec echo -e"

#----------------- TESTS WHILE -----------------#
echo "###################################################"
echo "TESTS WHILE"
echo "###################################################"

test_cmd "while false; do echo loop; done" "Test while faux"

test_cmd "while false; do echo a; echo b; done" "Test while faux commandes multiples"

test_cmd "while false; do echo never; done; echo after" "Test while puis commande apres"

#----------------- TESTS UNTIL -----------------#
echo "###################################################"
echo "TESTS UNTIL"
echo "###################################################"

test_cmd "until true; do echo loop; done" "Test until vrai"

test_cmd "until true; do echo a; echo b; done" "Test until vrai commandes multiples"

test_cmd "until true; do echo never; done; echo after" "Test until puis commande apres"

#----------------- TESTS REDIRECTIONS -----------------#
echo "###################################################"
echo "TESTS REDIRECTIONS"
echo "###################################################"

test_cmd "echo test > /tmp/42sh_out; cat /tmp/42sh_out" "Test redirection sortie simple"

test_cmd "echo line1 > /tmp/42sh_out; echo line2 >> /tmp/42sh_out; cat /tmp/42sh_out" "Test redirection append"

test_cmd "echo test 2> /tmp/42sh_err" "Test redirection stderr"

test_cmd "echo a > /tmp/42sh_t1; echo b > /tmp/42sh_t2; cat /tmp/42sh_t1 /tmp/42sh_t2" "Test redirections multiples fichiers"

test_cmd "echo hello > /tmp/42sh_redirect; cat < /tmp/42sh_redirect" "Test redirection sortie puis entree"

test_cmd "echo test1 > /tmp/42sh_file; echo test2 > /tmp/42sh_file; cat /tmp/42sh_file" "Test ecrasement fichier"

test_cmd "echo -n test > /tmp/42sh_n; cat /tmp/42sh_n" "Test redirection avec echo -n"

test_cmd "echo -e 'line1\\nline2' > /tmp/42sh_e; cat /tmp/42sh_e" "Test redirection avec echo -e"

#----------------- TESTS COMBINAISONS COMPLEXES -----------------#
echo "###################################################"
echo "TESTS COMBINAISONS COMPLEXES"
echo "###################################################"

test_cmd "echo a && echo b || echo c" "Test ET OU combine"

test_cmd "false && echo a || echo b && echo c" "Test ET OU multiple"

test_cmd "echo a | cat && echo b" "Test pipe puis ET"

test_cmd "true && echo a | cat" "Test ET puis pipe"

test_cmd "if true; then echo a && echo b; fi" "Test if avec ET"

test_cmd "if true && false; then echo no; else echo yes; fi" "Test if condition ET faux"

test_cmd "if false || true; then echo yes; fi" "Test if condition OU vrai"

test_cmd "echo a; echo b && echo c; echo d" "Test mix point-virgule ET"

test_cmd "true && true && true && echo ok" "Test chaîne de ET"

test_cmd "false || false || false || echo ok" "Test chaîne de OU"

test_cmd "! false && echo a || echo b" "Test negation ET OU"

test_cmd "echo a | cat | cat && echo b" "Test pipes multiples puis ET"

test_cmd "if true; then echo a | cat; fi" "Test if avec pipe"

test_cmd "while false; do echo a; done; echo b" "Test while puis commande"

test_cmd "until true; do echo a; done; echo b" "Test until puis commande"

test_cmd "echo start && echo middle | cat && echo end" "Test ET pipe ET"

test_cmd "true && echo a | cat | cat && echo b" "Test ET pipes multiples ET"

test_cmd "echo test > /tmp/42sh_x && cat /tmp/42sh_x" "Test redirection puis ET"

test_cmd "echo a | cat && echo b | cat" "Test pipe ET pipe"

test_cmd "if true; then echo -n a; echo b; fi" "Test if avec echo -n puis echo"

#----------------- TESTS VIDE & EOF -----------------#
echo "###################################################"
echo "TESTS VIDE & EOF"
echo "###################################################"

test_cmd "" "Test entree vide"

#----------------- TESTS COMMANDES BUILTIN -----------------#
echo "###################################################"
echo "TESTS BUILTINS"
echo "###################################################"

test_cmd "true" "Test true seul"

test_cmd "false" "Test false seul"

test_cmd "true; echo after" "Test true puis echo"

test_cmd "false; echo after" "Test false puis echo"

test_cmd "true && false && echo no" "Test true ET false ET"

test_cmd "false || true || echo no" "Test false OU true OU"

#----------------- TESTS GUILLEMETS ET ECHAPPEMENTS -----------------#
echo "###################################################"
echo "TESTS QUOTES ET ECHAP"
echo "###################################################"

test_cmd "echo 'a b c'" "Test guillemets simples avec espaces"

test_cmd 'echo "a b c"' "Test guillemets doubles avec espaces"

test_cmd "echo a'b'c" "Test concatenation avec guillemets simples"

test_cmd 'echo a"b"c' "Test concatenation avec guillemets doubles"

test_cmd "echo 'single quote text'" "Test texte long guillemets simples"

test_cmd 'echo "double quote text"' "Test texte long guillemets doubles"

test_cmd "echo 'line1\nline2'" "Test guillemets simples multilignes"

test_cmd "echo ''" "Test guillemets simples vides"

test_cmd 'echo ""' "Test guillemets doubles vides"

test_cmd "echo '' '' '' | cat -e" "Test guillemets multiples vides"

test_cmd "echo 'tab	inside'" "Test tabulation dans guillemets"

test_cmd 'echo hello \
	world' "Test line continuation"

#----------------- TESTS QUOTE NESTING -----------------#
echo "###################################################"
echo "TESTS QUOTES NESTING"
echo "###################################################"

test_cmd 'a=VAR; echo "$a" '\''$a'\' "Mixed quotes: double then single"

test_cmd 'echo "'"'$USER'"'"' "Single quotes inside double quotes"

test_cmd 'a=1; echo "\$a is $a"' "Escaped dollar inside double quotes"

test_cmd "echo 'It'\''s working'" "Escaped single quote inside single quotes"

#----------------- TESTS COMMANDES EXTERNES -----------------#
echo "###################################################"
echo "TESTS COMMANDES EXTERNES"
echo "###################################################"

test_cmd "ls /tmp > /dev/null; echo done" "Test ls avec redirection"

test_cmd "/bin/echo hello" "Test echo avec chemin absolu"

test_cmd "/bin/true && echo ok" "Test true chemin absolu"

#----------------- TESTS EDGE CASES -----------------#
echo "###################################################"
echo "TESTS EDGE CASES"
echo "###################################################"

test_cmd "echo" "Test echo sans argument"

test_cmd "echo ''" "Test echo avec chaîne vide"

test_cmd 'echo ""' "Test echo avec guillemets doubles vides"

test_cmd "true && true && true" "Test ET sans echo"

test_cmd "echo a | cat | cat | cat | cat | cat" "Test pipeline tres long"

test_cmd "if true; then true; fi" "Test if sans echo"

test_cmd "if false; then false; else true; fi" "Test if else sans echo"

test_cmd "echo -n" "Test echo -n sans texte"

test_cmd "echo -e" "Test echo -e sans texte"

test_cmd "echo -E" "Test echo -E sans texte"

test_cmd "echo -n -e" "Test echo -n -e sans texte"

test_cmd "cat /dev/null" "Test cat fichier vide"

test_cmd "echo test | cat | cat | cat | cat | cat | cat | cat | cat" "Test pipeline extrêmement long"

test_cmd "true && true || false && echo ok" "Test complexe ET OU"

test_cmd "echo a && echo b && echo c && echo d && echo e" "Test longue chaîne echo ET"

#----------------- TESTS REDIRECTIONS AVANCEES -----------------#
echo "###################################################"
echo "TESTS REDIRECTIONS AVANCEES"
echo "###################################################"

test_cmd "echo test > /tmp/42sh_pipe | cat /tmp/42sh_pipe" "Test redirection avec pipe (apres ecriture)"

test_cmd "echo hello > /tmp/42sh_h; echo world >> /tmp/42sh_h; cat /tmp/42sh_h | cat" "Test append puis pipe"

#----------------- TESTS WHILE/UNTIL AVEC CONDITIONS -----------------#
echo "###################################################"
echo "TESTS WHILE/UNTIL AVEC CONDITIONS"
echo "###################################################"

test_cmd "while false && false; do echo no; done; echo after" "Test while condition ET"

test_cmd "while false || false; do echo no; done; echo after" "Test while condition OU"

test_cmd "until true && true; do echo no; done; echo after" "Test until condition ET"

test_cmd "until true || false; do echo no; done; echo after" "Test until condition OU"

#----------------- TESTS COMBINAISONS ECHO OPTIONS -----------------#
echo "###################################################"
echo "TESTS COMBINAISONS ECHO OPTIONS"
echo "###################################################"

test_cmd "echo -e 'a\nb' | cat" "Test echo -e pipe cat"

test_cmd "echo -n test | cat" "Test echo -n pipe cat"

test_cmd "echo -e -n 'test\nline' | cat" "Test echo -e -n pipe cat"

test_cmd "echo -n a && echo -n b && echo c" "Test echo -n chaîne"

test_cmd "echo -e 'a\tb\tc'" "Test echo -e tabulations multiples"

test_cmd "echo -e '\\\\test'" "Test echo -e echappement backslash"

#----------------- TESTS IF COMPLEXES -----------------#
echo "###################################################"
echo "TESTS IF COMPLEXES"
echo "###################################################"

test_cmd "if echo test > /tmp/42sh_if; then cat /tmp/42sh_if; fi" "Test if avec redirection condition"

test_cmd "if true; then echo a > /tmp/42sh_then; fi; cat /tmp/42sh_then" "Test if avec redirection then"

test_cmd "if false; then echo no; elif true && true; then echo yes; fi" "Test if elif condition ET"

test_cmd "if false; then echo no; elif false || true; then echo yes; fi" "Test if elif condition OU"

test_cmd "if ! true; then echo no; else echo yes; fi" "Test if negation else"

#----------------- TESTS PIPES AVEC REDIRECTIONS -----------------#
echo "###################################################"
echo "TESTS PIPES AVEC REDIR"
echo "###################################################"

test_cmd "echo test > /tmp/42sh_pr; cat /tmp/42sh_pr | cat" "Test fichier puis pipe"

test_cmd "echo a | cat > /tmp/42sh_pc; cat /tmp/42sh_pc" "Test pipe puis redirection"

test_cmd "echo hello | cat | cat > /tmp/42sh_ppc; cat /tmp/42sh_ppc" "Test pipes multiples puis redirection"

#----------------- TESTS COMMENTS -----------------#
echo "###################################################"
echo "TESTS COMMENTS"
echo "###################################################"

test_cmd "echo test # this is a comment" "Test commentaire simple"

test_cmd "echo test # this is a comment\necho world" "Test commentaire puis commande"

test_cmd "# full line comment\necho visible" "Test ligne commentaire complete"

test_cmd "echo a; # comment
echo b" "Test commentaire apres point-virgule"

test_cmd "echo not#comment" "Test # pas en debut de mot"

#----------------- TESTS FOR -----------------#
echo "###################################################"
echo "TESTS FOR"
echo "###################################################"

test_cmd 'for i in a b c; do echo "$i"; done' "Test for simple"

test_cmd 'for i in 1 2 3; do echo "$i"; done' "Test for avec nombres"

test_cmd 'for word in hello world test; do echo "$word"; done' "Test for avec mots"

test_cmd 'for i in a; do echo "$i"; done' "Test for avec un seul element"

test_cmd 'for i in; do echo "$i"; done' "Test for sans elements"

test_cmd 'for i in a b c; do echo "$i"; echo next; done' "Test for avec plusieurs commandes"

test_cmd 'for i in x y; do for j in 1 2; do echo "$i$j"; done; done' "Test for imbrique"

test_error 'for i in a b c\ndo echo "$i"\ndone' "Test for avec retours ligne"

test_cmd 'for i in a b c; do echo "$i"; done; echo after' "Test for puis commande"

test_cmd 'for i in one two three; do echo -n "$i"; done' "Test for avec echo -n"

test_cmd 'for var in aa bb cc; do echo test "$var"; done' "Test for avec texte et variable"

test_cmd 'for i in a b; do true && echo "$i"; done' "Test for avec operateur ET"

test_cmd 'for i in a b; do echo "$i" | cat; done' "Test for avec pipe"

test_cmd "for i in 'hello world' test; do echo \"$i\"; done" "Test for avec guillemets"

#----------------- TESTS VARIABLES -----------------#
echo "###################################################"
echo "TESTS VARIABLES"
echo "###################################################"

test_cmd 'x=hello; echo "$x"' "Test assignation et expansion simple"

test_cmd 'x=hello; y=world; echo "$x" "$y"' "Test plusieurs variables"

test_cmd 'var=test; echo "$var"' "Test expansion variable simple"

test_cmd 'x=a; y=b; z=c; echo "$x$y$z"' "Test concatenation variables"

test_cmd 'name=value; echo prefix"$name"' "Test expansion avec prefix"

test_cmd 'name=value; echo "${name}"' "Test expansion avec accolades"

test_cmd 'x=hello; echo "$x" world' "Test expansion avec texte apres"

test_cmd 'x=; echo "$x"' "Test variable vide"

test_cmd 'x=test; echo "$x$x"' "Test meme variable deux fois"

test_cmd 'a=1; b=2; echo "$a" and "$b"' "Test variables avec texte entre"

test_cmd 'x=test; x=new; echo "$x"' "Test reassignation variable"

test_cmd 'x=a; echo "$x"; x=b; echo "$x"' "Test changement de valeur"

test_cmd 'x=hello; echo "$x" && echo "$x"' "Test variable avec ET"

test_cmd 'x=test; echo "$x" | cat' "Test variable avec pipe"

test_cmd 'x=value; if true; then echo "$x"; fi' "Test variable dans if"

test_cmd 'x=val; for i in a b; do echo "$x$i"; done' "Test variable dans for"

test_cmd 'x=5; y=10; echo "$x" "$y"' "Test variables numeriques"

test_cmd 'a=1 b=2; echo $a $b' "Test multiple variable assignation in same command"

#----------------- TESTS RECURSIVE VARIABLES -----------------#
echo "###################################################"
echo "TESTS RECURSIVE VARIABLES"
echo "###################################################"

test_cmd 'a=1; b=$a; echo "$b"' "Test recursive simple a=1; b=\$a"

test_cmd 'a=hello; b=$a; c=$b; echo "$c"' "Test triple recursive a=hello; b=\$a; c=\$b"

test_cmd 'a=1; b=$a"2"; c=$b"3"; echo "$c"' "Test recursive with concatenation"

test_cmd 'a=1; a=$a; echo "$a"' "Test self-reference re-assignment a=\$a"

test_cmd 'a=1; b=2; c=$a$b; echo "$c"' "Test recursive multi-variable concatenation"

test_cmd 'x=5; y=$x; x=10; echo "$y"' "Test variable snapshot (y should remain 5)"

#----------------- TESTS VARIABLE CONCATENATION -----------------#
echo "###################################################"
echo "TESTS VARIABLES CONCATENATION"
echo "###################################################"

test_cmd 'a=hello; b=world; c=$a$b; echo "$c"' "Concatenation of two variables"

test_cmd 'a=42; b=sh; c=$a$b" tests"; echo "$c"' "Variables mixed with literal double quotes"

test_cmd 'x=1; y=$x; x=2; echo "$y"' "Variable snapshot: y should be 1"

test_cmd 'a=1; b=$a; c=$b; d=$c; echo "$d"' "Deep recursive assignment"

#----------------- TESTS SPECIAL VARIABLES -----------------#
echo "###################################################"
echo "TESTS SPECIAL VARIABLES"
echo "###################################################"

test_cmd "true; echo \$?" "Test \$? après true"

test_cmd "false; echo \$?" "Test \$? après false"

test_cmd "ls /nonexistent 2>/dev/null; echo \$?" "Test \$? après une erreur de commande"

test_cmd 'echo $#' "Test \$# sans arguments"

test_cmd 'echo $#' "Test \$# avec arguments"

test_cmd 'echo $$ | grep -E "^[0-9]+$" > /dev/null && echo "is_pid"' "Test \$\$ est un nombre"

test_cmd 'echo $1' "Test \$1 vide"

test_cmd 'echo $RANDOM | grep -E "^[0-9]+$" > /dev/null && echo "is_num"' "Test \$RANDOM est un nombre"

test_cmd 'echo $UID | grep -E "^[0-9]+$" > /dev/null && echo "is_uid"' "Test \$UID est un nombre"

test_cmd 'echo $@' "Test \$@ vide"

test_cmd 'echo $*' "Test \$* vide"

test_cmd 'echo ${1}' "Test \${1} vide"

#----------------- TESTS SPECIAL VARS EDGE CASES -----------------#
echo "###################################################"
echo "TESTS VARIABLES EDGE CASES"
echo "###################################################"

test_cmd 'ls /nonexistent 2>/dev/null;echo $?;true;echo $?' "Sequence of exit codes"

test_cmd 'echo $# $1 $2' "Positional parameters when none are provided"

test_cmd 'IFS=_; a=1; b=2; echo "$a$b"' "Variable expansion with modified IFS"

test_cmd 'echo $$ | grep -E "^[0-9]+$" > /dev/null && echo "is_pid"' "PID expansion inside double quotes"

#----------------- TESTS ERREURS -----------------#
echo "###################################################"
echo "TESTS ERREURS"
echo "###################################################"

test_error ";" "Erreur point-virgule seul"

test_error ";;" "Erreur double point-virgule"

test_error "&&" "Erreur double esperluette seul"

test_error "||" "Erreur double pipe seul"

test_error "|" "Erreur pipe seul"

test_error "echo &&" "Erreur ET sans commande apres"

test_error "&& echo test" "Erreur ET sans commande avant"

test_error "echo ||" "Erreur OU sans commande apres"

test_error "|| echo test" "Erreur OU sans commande avant"

test_error "echo |" "Erreur pipe sans commande apres"

test_error "| echo test" "Erreur pipe sans commande avant"

test_error "if" "Erreur if incomplet"

test_error "if true" "Erreur if sans then"

test_error "if true; then" "Erreur if sans fi"

test_error "if true; then echo test" "Erreur if then sans fi"

test_error "while" "Erreur while incomplet"

test_error "while true" "Erreur while sans do"

test_error "while true; do" "Erreur while sans done"

test_error "while true; do echo test" "Erreur while do sans done"

test_error "until" "Erreur until incomplet"

test_error "until false" "Erreur until sans do"

test_error "until false; do" "Erreur until sans done"

test_error "until false; do echo test" "Erreur until do sans done"

test_error "for" "Erreur for incomplet"

test_error "for i" "Erreur for sans in"

test_error "for i in" "Erreur for in sans do"

test_error "for i in a b; do" "Erreur for sans done"

test_error "for i in a b; do echo test" "Erreur for do sans done"

test_error ">" "Erreur redirection seule"

test_error ">>" "Erreur redirection append seule"

test_error "<" "Erreur redirection input seule"

test_error "2>" "Erreur redirection stderr seule"

test_error "echo >" "Erreur redirection sans fichier"

test_error "echo >>" "Erreur redirection append sans fichier"

test_error "< /tmp/file" "Erreur redirection input sans commande"

test_error "!" "Erreur negation seule"

test_error "if; then echo test; fi" "Erreur if condition vide"

test_error "while; do echo test; done" "Erreur while condition vide"

test_error "until; do echo test; done" "Erreur until condition vide"

#----------------- TESTS EXIT STATUS CHAINS -----------------#
echo "###################################################"
echo "TESTS EXIT STATUS"
echo "###################################################"

test_cmd "true && false || true; echo \$?" "Exit code of a logical chain"

test_cmd "! true; echo \$?" "Exit code of a negation"

test_cmd "echo hello | grep 'notfound'; echo \$?" "Exit status of a failed pipeline member"

#----------------- TESTS FICHIERS -----------------#
echo "###################################################"
echo "TESTS FILES"
echo "###################################################"

mkdir -p /tmp/42sh_test_files

cat > /tmp/42sh_test_files/simple.sh << 'EOF'
echo "Hello from file"
echo "Line 2"
EOF

test_file "/tmp/42sh_test_files/simple.sh" "Test fichier simple"

cat > /tmp/42sh_test_files/variables.sh << 'EOF'
x=hello
y=world
echo "$x" "$y"
EOF

test_file "/tmp/42sh_test_files/variables.sh" "Test fichier avec variables"

cat > /tmp/42sh_test_files/conditions.sh << 'EOF'
if true; then
    echo "condition true"
fi

if false; then
    echo "not shown"
else
    echo "condition false"
fi
EOF

test_file "/tmp/42sh_test_files/conditions.sh" "Test fichier avec conditions"

cat > /tmp/42sh_test_files/loops.sh << 'EOF'
for i in a b c
do
    echo $i
done
EOF

test_file "/tmp/42sh_test_files/loops.sh" "Test fichier avec boucles for"

cat > /tmp/42sh_test_files/pipes.sh << 'EOF'
echo "test" | cat
echo "redirect" > /tmp/42sh_redir_test
cat /tmp/42sh_redir_test
EOF

test_file "/tmp/42sh_test_files/pipes.sh" "Test fichier avec pipes et redirections"

cat > /tmp/42sh_test_files/comments.sh << 'EOF'
# This is a comment
echo "visible"
echo "test" # inline comment
# Another comment
echo "end"
EOF

test_file "/tmp/42sh_test_files/comments.sh" "Test fichier avec commentaires"

cat > /tmp/42sh_test_files/operators.sh << 'EOF'
true && echo "success"
false || echo "fallback"
true && true && echo "multiple"
EOF

test_file "/tmp/42sh_test_files/operators.sh" "Test fichier avec opérateurs && ||"

cat > /tmp/42sh_test_files/empty.sh << 'EOF'
EOF

test_file "/tmp/42sh_test_files/empty.sh" "Test fichier vide"

cat > /tmp/42sh_test_files/only_comments.sh << 'EOF'
# Just comments
# Nothing else
EOF

test_file "/tmp/42sh_test_files/only_comments.sh" "Test fichier avec seulement commentaires"

#----------------- NETTOYAGE -----------------#

rm -rf /tmp/42sh_* 2>/dev/null


PERCENT=$(($SUCCESS * 100 / $TOTAL))
echo -e "${BBLU}Succeed:${GRN} $SUCCESS${WHT}"
echo -e "${BBLU}Failed:${RED} $(($TOTAL - $SUCCESS))${WHT}"
echo -e "${BBLU}Results:${BYEL} $PERCENT%${WHT}"

if [ -n "$OUTPUT_FILE" ]; then
    echo "$PERCENT" > "$OUTPUT_FILE"
fi

exit 0
