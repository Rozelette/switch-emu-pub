C:\Users\Shea\Downloads\gcc-linaro-6.2.1-2016.11-i686-mingw32_aarch64-linux-gnu\bin\aarch64-linux-gnu-gcc.exe -c -fpic test.c
C:\Users\Shea\Downloads\gcc-linaro-6.2.1-2016.11-i686-mingw32_aarch64-linux-gnu\bin\aarch64-linux-gnu-gcc.exe -shared -o libtest.so test.o
C:\Users\Shea\Downloads\gcc-linaro-6.2.1-2016.11-i686-mingw32_aarch64-linux-gnu\bin\aarch64-linux-gnu-gcc.exe -ffreestanding -nostdlib -L. -ltest ../common/crt0.S ../common/syscall.S recursion.c -o recursion.out
C:\Users\Shea\Downloads\gcc-linaro-6.2.1-2016.11-i686-mingw32_aarch64-linux-gnu\bin\aarch64-linux-gnu-objdump.exe -d recursion.out
pause