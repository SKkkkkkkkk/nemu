# CC=riscv32-unknown-elf-gcc cmake -S ../.. -B build -DPROJECT_NAME=hello -DPROJECT_PATH=. -DCMAKE_BUILD_TYPE=Release
CC=riscv64-unknown-elf-gcc cmake -S ../.. -B build -DPROJECT_NAME=hello -DPROJECT_PATH=.
cmake --build build