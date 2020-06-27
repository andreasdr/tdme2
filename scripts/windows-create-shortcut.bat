@if "%~1" == "" goto usage
@if "%~2" == "" goto usage
@if "%~3" == "" goto usage
@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
SET Esc_Executable=%1
SET Esc_Link=%2
SET Esc_Icon=%3
SET CreateShortcutVBSFileName=CreateShortcut.vbs
((
  echo Set oWS = WScript.CreateObject^("WScript.Shell"^)
  echo Set oFSO = CreateObject^("Scripting.FileSystemObject"^)
  echo sLinkFile = oWS.ExpandEnvironmentStrings^(!Esc_Link!^)
  echo Set oLink = oWS.CreateShortcut^(sLinkFile^)
  echo oLink.TargetPath = oWS.ExpandEnvironmentStrings^(!Esc_Executable!^)
  echo oLink.WorkingDirectory = oFSO.GetParentFolderName^(oLink.TargetPath!^)
  echo oLink.IconLocation = oLink.WorkingDirectory ^& "\" ^& ^(!Esc_Icon!^)
  echo oLink.Save
)1>!CreateShortcutVBSFileName!
cscript //nologo .\!CreateShortcutVBSFileName!
DEL !CreateShortcutVBSFileName! /f /q
)1>>nul 2>>&1
goto :eof
:usage
@echo Syntax
@echo     windows-create-shortcut "executable" "link" "icon"
@echo Parameters
@echo     executable: The path and file name of the application/document to open.
@echo     link: The path and file name (.link) of the shortcut file.
@echo     icon: The path and file name of icon (.ico) of the shortcut file.