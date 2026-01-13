#lib_LIBRARIES = libexecution.a

libexecution_a_SOURCES = execution.c execution.h \
						 my_echo.c my_echo.h

libexecution_a_CPPFLAGS = -I$(top_srcdir)/src

libexecution_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

noinst_LIBRARIES = libexecution.a
