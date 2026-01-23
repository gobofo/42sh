check_PROGRAMS = unit_tests

unit_tests_SOURCES = \
    lexer_tests.c \
	echo_tests.c \
	cd_tests.c \
	export_tests.c \
	unset_tests.c \
    $(top_srcdir)/src/environment/environment.c \
    $(top_srcdir)/src/ast/ast.c \
    $(top_srcdir)/src/hash_map/hash_map.c $(top_srcdir)/src/hash_map/hash.c \ 
    $(top_srcdir)/src/lexer/lexer_utils.c \
    $(top_srcdir)/src/lexer/lexer.c \
	$(top_srcdir)/src/execution/builtins/my_echo/my_echo.c \
	$(top_srcdir)/src/execution/builtins/my_cd/my_cd.c \
	$(top_srcdir)/src/execution/builtins/my_export/my_export.c \
	$(top_srcdir)/src/execution/builtins/my_unset/my_unset.c \
    $(top_srcdir)/src/utils.c

unit_tests_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
unit_tests_LDADD = -lcriterion
