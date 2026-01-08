SUBDIRS = \
		  expansion \
		  execution \
		  ast \
		  parser \
		  lexer \
		  IO

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

# %D% is the current dir
42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

42sh_LDADD =  \
	   expansion/libexpansion.a \
	   execution/libexecution.a \
	   ast/libast.a \
       parser/libparser.a \
	   lexer/liblexer.a \
	   IO/libio.a
