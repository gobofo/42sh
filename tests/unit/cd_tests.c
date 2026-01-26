#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../src/environment/environment.h"
#include "../../src/execution/builtins/my_cd/my_cd.h"

extern struct env *env;

void setup_cd(void)
{
    char *argv[] = { "./42sh", NULL };
    env = init_env(1, argv);
    cr_redirect_stderr();
    cr_redirect_stdout();
}

TestSuite(cd_tests, .init = setup_cd);

Test(cd_tests, cd_simple)
{
    char *args[] = { "/tmp", NULL };

    int res = my_cd(args);

    cr_assert_eq(res, 0);

    char *pwd = hash_map_get(env->variables, "PWD");

    cr_assert_str_eq(pwd, "/tmp");
}

Test(cd_tests, cd_back)
{
    hash_map_insert(env->variables, "PWD", strdup("/usr/bin"), free);

    char *args[] = { "..", NULL };
    int res = my_cd(args);

    cr_assert_eq(res, 0);

    char *pwd = hash_map_get(env->variables, "PWD");

    cr_assert_str_eq(pwd, "/usr/");
}

Test(cd_tests, cd_dash)
{
    hash_map_insert(env->variables, "PWD", strdup("/tmp"), free);
    hash_map_insert(env->variables, "OLDPWD", strdup("/home"), free);

    char *args[] = { "-", NULL };

    int res = my_cd(args);

    fflush(stdout);

    cr_assert_eq(res, 0);

    cr_assert_str_eq(hash_map_get(env->variables, "PWD"), "/home");
    cr_assert_str_eq(hash_map_get(env->variables, "OLDPWD"), "/tmp");

    cr_assert_stdout_eq_str("/home\n");
}

Test(cd_tests, error_too_many_args)
{
    char *args[] = { "/tmp", "/home", NULL };

    int res = my_cd(args);

    cr_assert_eq(res, 2);

    cr_assert_stderr_eq_str("Error: cd: too many arguments\n");
}

Test(cd_tests, non_existent_dir)
{
    char *args[] = { "/path/that/does/not/exist", NULL };

    int res = my_cd(args);

    cr_assert_eq(res, 1);

    cr_assert_stderr_eq_str(
        "Error: cd: no such file or directory: /path/that/does/not/exist\n");
}
