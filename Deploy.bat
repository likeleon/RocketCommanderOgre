@echo off

set DEPLOY_DIR=Deploy_%date:~-10,4%%date:~-5,2%%date:~-2,2%
set RELEASE_DIR=Release

mkdir %DEPLOY_DIR%

@rem Copy binary(.exe, .dll..) files
set DEPLOY_BIN_DIR=%DEPLOY_DIR%\Bin
mkdir %DEPLOY_BIN_DIR%
xcopy %RELEASE_DIR%\*.dll %DEPLOY_BIN_DIR%\ /f /y
xcopy %RELEASE_DIR%\*.exe %DEPLOY_BIN_DIR%\ /f /y

@rem Copy content(images, meshes, sounds...) files
set CONTENT_DIR=Source\Main\Content
set DEPLOY_CONTENT_DIR=%DEPLOY_DIR%\Content
xcopy %CONTENT_DIR%\* %DEPLOY_CONTENT_DIR%\ /f /y /s