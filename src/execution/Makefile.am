#lib_LIBRARIES = libexecution.a

libexecution_a_SOURCES = execution.c execution.h \
						 my_echo.c my_echo.h \
						 my_pipe.c my_pipe.h \
						 execute_command.c execute_command.h

libexecution_a_CPPFLAGS = -I$(top_srcdir)/src

libexecution_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

noinst_LIBRARIES = libexecution.a
