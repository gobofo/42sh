AC_INIT([deuteragonist], [1.0], [yann.fernandez-puig@epita.fr])

AM_INIT_AUTOMAKE([subdir-objects] [foreign])

AM_SILENT_RULES([yes])

AM_PROG_AR

AC_PROG_RANLIB

AC_PROG_CC

AX_COMPILER_FLAGS([], [], [], [-std=c99 -pedantic -Werror -Wall -Wextra -Wvla])

AC_CONFIG_FILES([
				 Makefile
				 src/Makefile
				 src/ast/Makefile
				 src/lexer/Makefile
				 src/parser/Makefile
				])

AC_OUTPUT
