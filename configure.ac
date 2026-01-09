AC_INIT([42sh], [1.0])

AM_INIT_AUTOMAKE([subdir-objects] [foreign])

AM_SILENT_RULES([yes])

AM_PROG_AR

AC_PROG_RANLIB

AC_PROG_CC

CFLAGS="$CFLAGS -std=c99 -pedantic -Werror -Wall -Wextra -Wvla"

PKG_CHECK_MODULES([CRITERION], [criterion], [has_criterion=yes], [has_criterion=no])

# Handle coverage mode for Testsuites
AM_CONDITIONAL([COVERAGE_ASKED], [test "x$COVERAGE" = "xyes"])

AC_SUBST([CRITERION_LIBS])

AC_CONFIG_FILES([
				 Makefile
				 src/Makefile
				 src/ast/Makefile
				 src/lexer/Makefile
				 src/parser/Makefile
				 src/expansion/Makefile
				 src/execution/Makefile
				 src/io_backend/Makefile
				 tests/Makefile
				 ])

AC_OUTPUT
