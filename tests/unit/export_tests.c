#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdlib.h>

#include "../../src/environment/environment.h"
#include "../../src/execution/builtins/my_export/my_export.h"

struct env *env;

static void setup(void)
{
    char *argv[] = { "42sh" };
    env = init_env(1, argv);
}

TestSuite(export_tests, .init = setup);

/* ======== TESTS SIMPLE ======== */

Test(export_tests, export_simple)
{
    char *args[] = { strdup("VAR=value"), NULL };
    int ret = my_export(args);

    cr_assert_eq(ret, 0);

    char *val = hash_map_get(env->variables, "VAR");
    cr_assert_str_eq(val, "value");

    free(args[0]);
}

Test(export_tests, export_vide)
{
    char *args[] = { strdup("EMPTY="), NULL };
    int ret = my_export(args);

    cr_assert_eq(ret, 0);

    char *val = hash_map_get(env->variables, "EMPTY");
    cr_assert_str_eq(val, "");

    free(args[0]);
}

Test(export_tests, export_espace)
{
    char *args[] = { strdup("boss=Pitchoune et Kiki"), NULL };
    int ret = my_export(args);

    cr_assert_eq(ret, 0);

    char *val = hash_map_get(env->variables, "boss");
    cr_assert_str_eq(val, "Pitchoune et Kiki");

    free(args[0]);
}

Test(export_tests, export_multiple)
{
    char *args[] = { strdup("A=1"), strdup("B=2"), NULL };
    int ret = my_export(args);

    cr_assert_eq(ret, 0);

    char *val1 = hash_map_get(env->variables, "A");
    char *val2 = hash_map_get(env->variables, "B");

    cr_assert_str_eq(val1, "1");
    cr_assert_str_eq(val2, "2");

    free(args[0]);
    free(args[1]);
}

Test(export_tests, double_def_var)
{
    char *args1[] = { strdup("VAR=prems"), NULL };
    my_export(args1);

    char *args2[] = { strdup("VAR=deums"), NULL };
    my_export(args2);

    char *val = hash_map_get(env->variables, "VAR");
    cr_assert_str_eq(val, "deums");

    free(args1[0]);
    free(args2[0]);
}

/* ======== TESTS ERREURS ======== */

Test(export_tests, bad_name)
{
    cr_redirect_stderr();

    char *args[] = { strdup("123=invalid"), NULL };
    int ret = my_export(args);

    cr_assert_eq(ret, 2);

    free(args[0]);
}

Test(export_tests, rien)
{
    cr_redirect_stderr();

    char *args[] = { NULL };
    int ret = my_export(args);

    cr_assert_eq(ret, 2);
}
