#lib_LIBRARIES = libexecution.a

libexecution_a_SOURCES = execution.c execution.h \
						 execute_command/execute_command.c execute_command/execute_command.h \
						 builtins/my_echo/my_echo.c my_echo/my_echo.h \
						 builtins/my_redir/my_redir.c my_redir/my_redir.h \
						 builtins/my_cd/my_cd.c my_cd/my_cd.h \
						 builtins/my_exit/my_exit.c my_exit/my_exit.h \
						 builtins/my_break/my_break.c my_break/my_break.h \
						 builtins/my_continue/my_continue.c my_continue/my_continue.h \
						 builtins/my_unset/my_unset.c my_unset/my_unset.h \
						 builtins/my_export/my_export.c my_export/my_export.h\
						 builtins/my_dot/my_dot.c my_dot/my_dot.h

libexecution_a_CPPFLAGS = -I$(top_srcdir)/src

libexecution_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

noinst_LIBRARIES = libexecution.a
