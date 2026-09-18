rm src/semantics/impl.cpp

rm -rf build
mkdir build

cd build
cmake ..
make