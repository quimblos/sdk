rm -rf build
mkdir build
cd build
emcmake cmake ..
make

cp quimblos.js ../examples/wasm
cp quimblos.wasm ../examples/wasm

cp quimblos.js ../../ui/lib
cp quimblos.wasm ../../ui/lib