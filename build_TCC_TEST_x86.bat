tcc -m32 test.c mmu.c -o release/x86/test.exe

@echo;
@echo off

cd release
cd x86
pause
test.exe
