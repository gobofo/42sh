FUNCT_TESTS = func_tests.sh
check_PROGRAMS = unit_tests

if COVERAGE_ASKED

unit_tests_SOURCES = unit/lexer_tests.c
unit_tests_LDADD = $(top_builddir)/src/lexer/liblexer.a $(CRITERION_LIBS)

TESTS = $(check_PROGRAMS) $(FUNCT_TESTS)

else

TESTS = $(FUNCT_TESTS)

endif

AM_TESTS_ENVIRONMENT = \
	export BIN_PATH="$(BIN_PATH)"; \
	export OUTPUT_FILE="$(OUTPUT_FILE)"; \
	export COVERAGE="$(COVERAGE)";
