del .\output\Target.elf.bin.bak
copy .\output\Target.elf.bin .\output\Target.elf.bin.bak
.\output\MAPPACK2 .\output\Target.elf.bin
.\output\APND_CRC .\output\Target.elf.bin F
.\output\printcrc .\output\Target.elf.bin
.\output\printcrc .\output\Target.elf.bin > .\output\Header.info
::cd \output
::GenROM.exe 
::cd ..\..
