tcc -m32 -shared mmu.c -DBUILD_DLL -o release/x86/mmu.dll

@echo;
@echo off

pause
