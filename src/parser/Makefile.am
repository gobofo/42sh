#lib_LIBRARIES = libparser.a

libparser_a_SOURCES =  parser.h \
					   parser_fnct/parser_command.c \
					   parser_fnct/parser_condition.c \
					   parser_fnct/parser_loop.c \
					   parser_fnct/parser_sequence.c \
					   parser_fnct/parser_token.c \
					   usefull_fnct.c \
					   parser_firsts/first_command.c \
					   parser_firsts/first_command.h \
					   parser_firsts/first_sequence.c \
					   parser_firsts/first_sequence.h \
					   parser_firsts/first_condition.c \
					   parser_firsts/first_condition.h \
					   parser_firsts/first_loop.c \
					   parser_firsts/first_loop.h \
					   parser_firsts/first_token.c \
					   parser_firsts/first_token.h \
					   parser_follow.c \
					   parser_follow.h \
					   usefull_fnct.h \
					   print_parser.c 

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

noinst_LIBRARIES = libparser.a
