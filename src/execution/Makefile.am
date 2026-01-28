#lib_LIBRARIES = libexecution.a

libexecution_a_SOURCES = execution.c execution.h \
						 execution_utils.c execution_utils.h \
						 execute_command/execute_command.c execute_command/execute_command.h \
						 builtins/my_echo/my_echo.c builtins/my_echo/my_echo.h \
						 builtins/my_redir/my_redir.c builtins/my_redir/my_redir.h \
						 builtins/my_cd/my_cd.c builtins/my_cd/my_cd.h \
						 builtins/my_exit/my_exit.c builtins/my_exit/my_exit.h \
						 builtins/my_break/my_break.c builtins/my_break/my_break.h \
						 builtins/my_continue/my_continue.c builtins/my_continue/my_continue.h \
						 builtins/my_unset/my_unset.c builtins/my_unset/my_unset.h \
						 builtins/my_export/my_export.c builtins/my_export/my_export.h\
						 builtins/my_dot/my_dot.c builtins/my_dot/my_dot.h \
						 builtins/operators/operators.c builtins/operators/operators.h \
						 builtins/my_alias/my_alias.c builtins/my_alias/my_alias.h

libexecution_a_CPPFLAGS = -I$(top_srcdir)/src

libexecution_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

noinst_LIBRARIES = libexecution.a
