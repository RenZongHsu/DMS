@echo off
:loop_start
main.exe COM4 baudrate:921600 "bfile:D:/GP22C_DMS_code/tool/G+ Code Packer V1.9.9.14/OutputBin/GPA7XXXA_SPIFC/Code.bin"
@ping 127.0.0.1 -n 10 -w 1000 > nul
goto loop_start 
pause