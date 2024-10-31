gcc test.c mmu.c -o2 -s -ffunction-sections -fdata-sections -Wl,--gc-sections -m128bit-long-double -o release/x64/test.exe

@echo;
@echo off

cd release
cd x64
pause
test.exe
