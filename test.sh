clear
rm -rf build
mkdir build
cd build
cmake ..
make

cd ..
valgrind --leak-check=full build/blink

# valgrind --tool=massif --stacks=yes build/blink
# ms_print massif.out.