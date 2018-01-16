E:\temp\gcc-linaro\bin\aarch64-linux-gnu-as.exe -g startup.s -o startup.o
E:\temp\gcc-linaro\bin\aarch64-linux-gnu-gcc.exe -c -g test.c -o test.o
E:\temp\gcc-linaro\bin\aarch64-linux-gnu-ld.exe -T test.ld test.o startup.o -o test.elf
E:\temp\gcc-linaro\bin\aarch64-linux-gnu-objcopy.exe -O binary test.elf test.bin
pause