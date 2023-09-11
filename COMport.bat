@echo off
WMIC PATH Win32_PnPEntity GET Caption | findstr "COM[0-50].*"
pause