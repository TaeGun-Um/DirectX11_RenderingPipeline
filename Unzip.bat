@echo off
\

IF NOT EXIST "%~dp0%ThirdParty\DirectXTex\lib\x64\Release\DirectXTex.lib" (
    powershell expand-archive ThirdParty\DirectXTex\lib.zip ThirdParty\DirectXTex\lib\
)

IF NOT EXIST "%~dp0%ThirdParty\Assimp\lib\x64\Release\assimp-vc143-mt.lib" (
    powershell expand-archive ThirdParty\Assimp\lib.zip ThirdParty\Assimp\lib\
)