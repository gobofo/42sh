#lib_LIBRARIES = libexecution.a

libexecution_a_SOURCES = execution.c execution.h \
						 my_echo/my_echo.c my_echo/my_echo.h \
						 execute_command/execute_command.c execute_command/execute_command.h \
						 my_redir/my_redir.c my_redir/my_redir.h \
						 my_cd/my_cd.c my_cd/my_cd.h

libexecution_a_CPPFLAGS = -I$(top_srcdir)/src

libexecution_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

noinst_LIBRARIES = libexecution.a
