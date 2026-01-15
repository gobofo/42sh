check-local:
	@chmod +x $(srcdir)/testsuite.sh
	@BIN_PATH="$(BIN_PATH)" \
	 COVERAGE="$(COVERAGE)" \
	 OUTPUT_FILE="$(OUTPUT_FILE)" \
	 $(srcdir)/testsuite.sh || true
