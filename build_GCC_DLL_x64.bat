gcc -m64 -shared mmu.c -o2 -s -ffunction-sections -fdata-sections -Wl,--gc-sections -m128bit-long-double -DBUILD_DLL -o release/x64/mmu.dll

@echo;
@echo off

pause
