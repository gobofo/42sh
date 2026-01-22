#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdlib.h>

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
    char *argv[] = { strdup(""), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("\n");
    free(argv[0]);
}

/* ======== TESTS ECHO SIMPLE ======== */

Test(echo_tests, one_word)
{
    char *argv[] = { strdup("a"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\n");
    free(argv[0]);
}

Test(echo_tests, two_words)
{
    char *argv[] = { strdup("a"), strdup("b"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a b\n");
    free(argv[0]);
    free(argv[1]);
}

Test(echo_tests, multiple_words)
{
    char *argv[] = { strdup("Bonjour"), strdup("je"), strdup("m'appelle"), strdup("crampte"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("Bonjour je m'appelle crampte\n");
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);
}

/* ======== TESTS FLAG -n ======== */

Test(echo_tests, flag_n_simple)
{
    char *argv[] = { strdup("-n"), strdup("hello"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("hello");
    free(argv[0]);
    free(argv[1]);
}

Test(echo_tests, flag_n_seul)
{
    char *argv[] = { strdup("-n"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("");
    free(argv[0]);
}

Test(echo_tests, flag_n_double)
{
    char *argv[] = { strdup("-n"), strdup("-n"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("-n");
    free(argv[0]);
    free(argv[1]);
}

Test(echo_tests, flag_n_multiple_words)
{
    char *argv[] = { strdup("-n"), strdup("Le"), strdup("gateau"), strdup("il"), strdup("etait"), strdup("sec"), strdup("?"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("Le gateau il etait sec ?");
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);
    free(argv[4]);
    free(argv[5]);
    free(argv[6]);
}

/* ======== TESTS FLAG -e ======== */

Test(echo_tests, flag_e_nvligne)
{
    char *argv[] = { strdup("-e"), strdup("a\\nb"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\nb\n");
    free(argv[0]);
    free(argv[1]);
}

Test(echo_tests, flag_e_tab)
{
    char *argv[] = { strdup("-e"), strdup("a\\tb"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\tb\n");
    free(argv[0]);
    free(argv[1]);
}

Test(echo_tests, flag_e_backslash_mult)
{
    char *argv[] = { strdup("-e"), strdup("\\\\"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("\\\n");
    free(argv[0]);
    free(argv[1]);
}

Test(echo_tests, flag_e_backslash)
{
    char *argv[] = { strdup("-e"), strdup("\\x"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("\\x\n");
    free(argv[0]);
    free(argv[1]);
}

/* ======== TESTS FLAG -E ======== */

Test(echo_tests, flag_E_nvligne)
{
    char *argv[] = { strdup("-E"), strdup("a\\nb"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\\nb\n");
    free(argv[0]);
    free(argv[1]);
}

Test(echo_tests, flag_E_tab)
{
    char *argv[] = { strdup("-E"), strdup("a\\tb"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\\tb\n");
    free(argv[0]);
    free(argv[1]);
}

Test(echo_tests, flag_E_backslah_mult)
{
    char *argv[] = { strdup("-E"), strdup("\\\\"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("\\\\\n");
    free(argv[0]);
    free(argv[1]);
}

Test(echo_tests, flag_n_and_e)
{
    char *argv[] = { strdup("-n"), strdup("-e"), strdup("a\\nb"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\nb");
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
}

Test(echo_tests, flag_n_and_E)
{
    char *argv[] = { strdup("-n"), strdup("-E"), strdup("a\\nb"), NULL };
    my_echo(argv);
    cr_assert_stdout_eq_str("a\\nb");
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
}

