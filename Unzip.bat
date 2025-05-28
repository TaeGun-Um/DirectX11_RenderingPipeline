@echo off
\

IF NOT EXIST "%~dp0%ThirdParty\DirectXTex\lib\x64\Release\DirectXTex.lib" (
    powershell expand-archive ThirdParty\DirectXTex\lib.zip ThirdParty\DirectXTex\lib\
)