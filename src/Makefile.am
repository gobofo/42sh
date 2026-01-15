SUBDIRS = \
		  execution \
		  expansion \
		  ast \
		  parser \
		  lexer \
		  io_backend \
		  environment \
		  hash_map 

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

# %D% is the current dir
42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

42sh_LDADD =  \
	   execution/libexecution.a \
	   expansion/libexpansion.a \
	   ast/libast.a \
       parser/libparser.a \
	   lexer/liblexer.a \
	   io_backend/libio_backend.a \
	   environment/libenv.a \
	   hash_map/libhash.a
