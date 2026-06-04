set -e

clear
cd build
cmake ..
make

cd ..
valgrind -s --leak-check=full build/langmaker example target/example.ebnf

cd target
sh build.sh

valgrind -s --leak-check=full build/example-compiler "HELLO=
OLA=MUNDO
"

# valgrind --tool=massif --stacks=yes build/blink
# ms_print massif.out.