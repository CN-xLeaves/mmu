tcc -m64 -shared mmu.c -DBUILD_DLL -o release/x64/mmu.dll

@echo;
@echo off

pause
