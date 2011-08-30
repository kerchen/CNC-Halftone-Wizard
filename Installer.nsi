;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

  !define ALL_USERS
  
;--------------------------------
;General
  
  !define COMPANY_DIR_NAME	"Whirling Chair"
  !define APP_NAME "CNC Halftone Wizard"
  !define VERSION "1.0.0"
  !define APP_EXECUTABLE "CNCHalftoneWizard.exe"
  
  ;Name and file
  Name "CNC Halftone Wizard (${VERSION})"
  OutFile "CNCHalftoneWizInstaller.${VERSION}.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\${COMPANY_DIR_NAME}\${APP_NAME}"
  
;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "LICENSE.GPL3"
  ;!insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section

  SetOutPath $INSTDIR
  
  File "release\${APP_EXECUTABLE}"
  
  File ChangeLog.txt

  CreateDirectory $INSTDIR\ui
  CreateDirectory $INSTDIR\ui\res
  SetOutPath $INSTDIR\ui\res
  File ui\res\*.png
  File ui\res\*.qrc

  SetOutPath $INSTDIR
  File $%SystemRoot%\system32\msvcp71d.dll
  File $%SystemRoot%\system32\msvcr71d.dll
  File $%SystemRoot%\system32\msvcp71.dll
  File $%SystemRoot%\system32\msvcr71.dll
  File $%SystemRoot%\system32\msvcrt.dll
  File $%QT_DIR%\bin\QtCore4.dll
  File $%QT_DIR%\bin\QtGui4.dll

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  CreateDirectory "$SMPROGRAMS\${COMPANY_DIR_NAME}"
  CreateDirectory "$SMPROGRAMS\${COMPANY_DIR_NAME}\${APP_NAME}"

  CreateShortCut "$SMPROGRAMS\${COMPANY_DIR_NAME}\${APP_NAME}\HalftoneCNCWiz.lnk" "$INSTDIR\${APP_EXECUTABLE}"
  CreateShortCut "$SMPROGRAMS\${COMPANY_DIR_NAME}\${APP_NAME}\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  CreateShortCut "$SMPROGRAMS\${COMPANY_DIR_NAME}\${APP_NAME}\Change Log.lnk" "$INSTDIR\ChangeLog.txt"

SectionEnd


;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete $INSTDIR\*.exe
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\*.txt
  
  Delete $INSTDIR\UI\*.*
  RMDir /r $INSTDIR\UI

  RMDir "$INSTDIR"
  RMDir "$PROGRAMFILES\${COMPANY_DIR_NAME}"

  Delete "$SMPROGRAMS\${COMPANY_DIR_NAME}\${APP_NAME}\CNCHalftoneWiz.lnk"
  Delete "$SMPROGRAMS\${COMPANY_DIR_NAME}\${APP_NAME}\Uninstall.lnk"
  Delete "$SMPROGRAMS\${COMPANY_DIR_NAME}\${APP_NAME}\Change Log.lnk"

  RMDir "$SMPROGRAMS\${COMPANY_DIR_NAME}\${APP_NAME}"
  RMDir "$SMPROGRAMS\${COMPANY_DIR_NAME}"
SectionEnd
