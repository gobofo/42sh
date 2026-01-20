#lib_LIBRARIES = libparser.a

libparser_a_SOURCES =  parser.h \
					   parser_fnct/parser_command.c \
					   parser_fnct/parser_condition.c \
					   parser_fnct/parser_loop.c \
					   parser_fnct/parser_sequence.c \
					   parser_fnct/parser_token.c \
					   parser_fnct/usefull_fnct.c \
					   parser_first.c \
					   parser_first.h \
					   parser_follow.c \
					   parser_follow.h \
					   print_parser.c 

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

noinst_LIBRARIES = libparser.a
