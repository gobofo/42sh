AC_INIT([42sh], [1.0])

AM_INIT_AUTOMAKE([subdir-objects] [foreign])

AM_SILENT_RULES([yes])

AM_PROG_AR

AC_PROG_RANLIB

AC_PROG_CC

CFLAGS="$CFLAGS -std=c99 -pedantic -Werror -Wall -Wextra -Wvla -g3 -O0 -fsanitize=address -fno-omit-frame-pointer"

# Check for Criterion library
AC_CHECK_LIB([criterion], [criterion_initialize],
			 [CRITERION_LIBS="-lcriterion" has_criterion=yes],
			 [AC_MSG_ERROR([Criterion library not found. Please install it.])]
)

# Check for Criterion headers
AC_CHECK_HEADERS([criterion/criterion.h], [],
				 [AC_MSG_ERROR([Criterion headers not found.])]
)

# Make the variables available to your Makefile
AC_SUBST([CRITERION_LIBS])

LIBS="$LIBS $CRITERION_LIBS"

# Handle coverage mode for Testsuites
AM_CONDITIONAL([COVERAGE_ASKED], [test "x$COVERAGE" = "xyes"])

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
