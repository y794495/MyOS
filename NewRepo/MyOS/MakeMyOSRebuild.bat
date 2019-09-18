
@echo off

cd Source
make clean
make all

pause
vmplayer
@echo on