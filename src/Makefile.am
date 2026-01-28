SUBDIRS = \
		  hash_map \
		  environment \
		  execution \
		  expansion \
		  ast \
		  parser \
		  lexer \
		  io_backend

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c launch_shell.c

# %D% is the current dir
42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

42sh_LDADD =  \
	   hash_map/libhash.a \
	   environment/libenv.a \
	   io_backend/libio_backend.a \
	   execution/libexecution.a \
	   expansion/libexpansion.a \
	   ast/libast.a \
       parser/libparser.a \
	   lexer/liblexer.a
