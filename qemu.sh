qemu-system-riscv32 -M virt -cpu rv32 \
-nographic \
-singlestep \
-smp 1 \
-d unimp,exec \
-bios test/app/hello/build/hello.bin \
-D qemu.log \
-S -s
