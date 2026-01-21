AC_INIT([42sh], [1.0])

AM_INIT_AUTOMAKE([subdir-objects] [foreign])

AM_SILENT_RULES([yes])

AM_PROG_AR

AC_PROG_RANLIB

AC_PROG_CC

CFLAGS="$CFLAGS -std=c99 -pedantic -Werror -Wall -Wextra -Wvla -O0 -g3 -ggdb -fno-omit-frame-pointer -fno-inline " #"-fsanitize=address -g -ggdb3 -Og"

AC_CONFIG_FILES([
				 Makefile
				 src/Makefile
				 src/ast/Makefile
				 src/lexer/Makefile
				 src/parser/Makefile
				 src/expansion/Makefile
				 src/execution/Makefile
				 src/io_backend/Makefile
				 src/environment/Makefile
				 src/hash_map/Makefile
				 tests/Makefile
				 ])

AC_OUTPUT
