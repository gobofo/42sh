#lib_LIBRARIES = liblexer.a

liblexer_a_SOURCES = lexer.c lexer.h \
					 lexer_utils.c lexer_utils.h \
					 ../utils.c ../utils.h
liblexer_a_CPPFLAGS = -I$(top_srcdir)/src
liblexer_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
noinst_LIBRARIES = liblexer.a

lexer_main_SOURCES = main.c
lexer_main_LDADD = liblexer.a
lexer_main_CPPFLAGS = -I$(top_srcdir)/src
lexer_main_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
noinst_PROGRAMS = lexer_main
