@echo off
@ping 127.0.0.1 -n 4 >nul
del /q gpumaker50.exe
copy /y gpumaker.tmp gpumaker50.exe
del /q gpumaker.tmp
gpumaker50.exe
del %0
