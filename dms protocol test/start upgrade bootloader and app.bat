@echo off
:loop_start
main.exe COM6 "bfile:D:/GP22C_Turnkey_code_DMS/tool/G+ Code Packer V1.9.9.14/OutputBin/GPA7XXXA_SPIFC/Code.bin"
@ping 127.0.0.1 -n 10 -w 1000 > nul
goto loop_start 
pause