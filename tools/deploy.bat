@echo off
REM Usage: tools\deploy.bat path\to\binary [target_dir]
if "%~1"=="" goto usage
set REMOTE_DIR=%~2
if "%REMOTE_DIR%"=="" set REMOTE_DIR=/tmp
scp -P 2222 -o Ciphers=aes128-ctr -o MACs=hmac-sha2-256 -o StrictHostKeyChecking=no -o UserKnownHostsFile=NUL "%~1" root@localhost:%REMOTE_DIR%/%~nx1
goto end
:usage
echo Usage: deploy.bat path\to\binary [target_dir]
:end