tcc -m64 test.c mmu.c -o release/x64/test.exe

@echo;
@echo off

cd release
cd x64
pause
test.exe
