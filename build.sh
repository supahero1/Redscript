echo Building...

if [ "$1" == "-d" ]; then
g++ -o rscript ./src/*.cpp entry.cpp -Isrc -std=c++20 -g -pipe
echo "Build done."
gdb rscript
else
g++ -o rscript ./src/*.cpp entry.cpp -Isrc -std=c++20 -pipe
echo "Build done. (no debug symbols)"
fi