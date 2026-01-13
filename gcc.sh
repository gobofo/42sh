rm 42sh

autoreconf --install
./configure

make

cp src/42sh 42

make clean

rm AST_def_node.txt

rm -rf ar-lib
rm config.log
rm depcomp
rm -rf autom4te.cache/
rm aclocal.m4
rm compile
rm config.status
rm configure
rm install-sh
rm missing
rm test-driver
rm Makefile.in
rm Makefile

rm tests/Makefile.in
rm tests/Makefile
rm -rf tests/.deps

rm $(find src -name "Makefile" -o -name "Makefile.in")
rm -rf $(find src -type d -name ".deps")

mv 42 42sh
