set -e

clear
cd build
cmake ..
make

cd ..
valgrind -s --leak-check=full build/test

# valgrind --tool=massif --stacks=yes build/blink
# ms_print massif.out.