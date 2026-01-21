check_PROGRAMS = unit_tests
unit_tests_SOURCES = \
    lexer_tests.c \
	echo_tests.c \
    $(top_srcdir)/src/lexer/lexer.c \
    $(top_srcdir)/src/lexer/lexer_utils.c \
	$(top_srcdir)/src/execution/my_echo/my_echo.c \
    $(top_srcdir)/src/utils.c


unit_tests_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
unit_tests_LDADD = -lcriterion
