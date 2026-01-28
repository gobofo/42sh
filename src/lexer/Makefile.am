#lib_LIBRARIES = liblexer.a

liblexer_a_SOURCES = lexer.c lexer.h \
					 lexer_utils.c lexer_utils.h \
					 input_stack.c input_stack.h

liblexer_a_CPPFLAGS = -I$(top_srcdir)/src
liblexer_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
noinst_LIBRARIES = liblexer.a

#lexer_main_SOURCES = main.c
#lexer_main_LDADD = liblexer.a \
#				   $(top_builddir)/src/hash_map/libhash.a \
#				   $(top_builddir)/src/environment/libenv.a
#lexer_main_CPPFLAGS = -I$(top_srcdir)/src
#lexer_main_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
#noinst_PROGRAMS = lexer_main
