#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../../src/execution/my_echo/my_echo.h"

static void setup_stdout(void)
{
	cr_redirect_stdout();
}

TestSuite(echo_tests, .init = setup_stdout);


/* ======== TESTS VIDE ======== */

Test(echo_tests, empty)
{
    char *argv[] = { NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("\n");
}

Test(echo_tests, empty_string)
{
    char *argv[] = { "", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("\n");
}

/* ======== TESTS ECHO SIMPLE ======== */

Test(echo_tests, one_word)
{
    char *argv[] = { "a", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\n");
}

Test(echo_tests, two_words)
{
    char *argv[] = { "a", "b", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a b\n");
}

Test(echo_tests, multiple_words)
{
    char *argv[] = { "Bonjour", "je", "m'appelle", "crampte", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("Bonjour je m'appelle crampte\n");
}

/* ======== TESTS FLAG -n ======== */

Test(echo_tests, flag_n_simple)
{
    char *argv[] = { "-n", "hello", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("hello");
}

Test(echo_tests, flag_n_seul)
{
    char *argv[] = { "-n", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("");
}

Test(echo_tests, flag_n_double)
{
    char *argv[] = { "-n", "-n", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("-n");
}

Test(echo_tests, flag_n_multiple_words)
{
    char *argv[] = { "-n", "Le", "gateau", "il", "etait", "sec", "?", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("Le gateau il etait sec ?");
}

/* ======== TESTS FLAG -e ======== */

Test(echo_tests, flag_e_nvligne)
{
    char *argv[] = { "-e", "a\\nb", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\nb\n");
}

Test(echo_tests, flag_e_tab)
{
    char *argv[] = { "-e", "a\\tb", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\tb\n");
}

Test(echo_tests, flag_e_backslash_mult)
{
    char *argv[] = { "-e", "\\\\", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("\\\n");
}

Test(echo_tests, flag_e_backslash)
{
    char *argv[] = { "-e", "\\x", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("\\x\n");
}

/* ======== TESTS FLAG -E ======== */

Test(echo_tests, flag_E_nvligne)
{
    char *argv[] = { "-E", "a\\nb", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\\nb\n");
}

Test(echo_tests, flag_E_tab)
{
    char *argv[] = { "-E", "a\\tb", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\\tb\n");
}

Test(echo_tests, flag_E_backslah_mult)
{
    char *argv[] = { "-E", "\\\\", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("\\\\\n");
}

Test(echo_tests, flag_n_and_e)
{
    char *argv[] = { "-n", "-e", "a\\nb", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\nb");
}

Test(echo_tests, flag_n_and_E)
{
    char *argv[] = { "-n", "-E", "a\\nb", NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\\nb");
}

