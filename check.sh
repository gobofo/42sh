autoreconf --install >/dev/null
./configure CFLAGS="-fsanitize=address -g" >/dev/null

BIN_PATH="$(pwd)/src/42sh" OUTPUT_FILE="$(pwd)/out" make check
make clean >/dev/null

rm AST_def_node.txt >/dev/null

rm -rf ar-lib >/dev/null
rm config.log >/dev/null
rm depcomp >/dev/null
rm -rf autom4te.cache/ >/dev/null
rm aclocal.m4 >/dev/null
rm compile >/dev/null
rm config.status >/dev/null
rm configure >/dev/null
rm install-sh >/dev/null
rm missing >/dev/null
rm test-driver >/dev/null
rm Makefile.in >/dev/null
rm Makefile >/dev/null

rm tests/Makefile.in >/dev/null
rm tests/Makefile >/dev/null
rm -rf tests/.deps >/dev/null

rm $(find src -name "Makefile" -o -name "Makefile.in") >/dev/null
rm -rf $(find src -type d -name ".deps") >/dev/null
