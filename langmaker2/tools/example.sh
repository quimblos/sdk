set -e

clear
cd build
cmake ..
make
cd ..

rm -rf zoo
mkdir zoo

echo "grammar ::= animal+
animal ::= 'cat'|'dog'" > zoo/syntax.ebnf

echo "grammar -> Zoo:
  animals = animal[]
animal -> Animal:
  type = #" > zoo/semantics.gsf

valgrind -s --leak-check=full build/langmaker zoo zoo/syntax.ebnf zoo/semantics.gsf

cd zoo
sh build.sh

valgrind -s --leak-check=full build/zoo-cli "catdog"