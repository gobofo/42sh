#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>

#include "../../src/environment/environment.h"
#include "../../src/execution/builtins/my_unset/my_unset.h"

extern struct env *env;

void setup_unset(void)
{
    char *argv[] = { "./42sh", NULL };
    env = init_env(1, argv);
}

TestSuite(unset_tests, .init = setup_unset);

Test(unset_tests, unset_variable)
{
    hash_map_insert(env->variables, "caca", strdup("prout"), free);
    
    char *args[] = { "caca", NULL };
    cr_assert_eq(my_unset(args), 0);
    
    cr_assert_null(hash_map_get(env->variables, "prout"));
}

Test(unset_tests, unset_nonexistent) {
    char *args[] = { "nope", NULL };
    cr_assert_eq(my_unset(args), 0);
}
