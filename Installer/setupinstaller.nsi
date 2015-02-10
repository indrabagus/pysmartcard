/*
 * NSIS script untuk installer Xirka Emulator / Development Board
 *  
 */
!include "MUI2.nsh"
!include "logiclib.nsh"
!include "python.nsh"

!define PRODUCT_NAME "Python Xirka Smart Card Module"
!define PRODUCT_VERSION "1.00"
!define PRODUCT_PUBLISHER "PT Xirka Silicon Technology"
!define PRODUCT_WEB_SITE "http://xirkachipset.com"
!define COMPANY_NAME "Xirka Silicon Tech"
!define DIR_NAME "Xirka VeriU XVU"

; Following constants you should never change
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

Name "${PRODUCT_NAME}"
OutFile "XirkaSCard.exe"
InstallDir "${DEFAULTPYTHONPATH}"

!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
;--------------------------------
;Pages
!define MUI_WELCOMEPAGE_TITLE 'Smart Card Module'
!define MUI_WELCOMEPAGE_TEXT 'Xirka Smart Card Python Modules'
!insertmacro MUI_PAGE_WELCOME
#!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"

!define MUI_PAGE_CUSTOMFUNCTION_SHOW ShowDirectoryPage
!insertmacro MUI_PAGE_DIRECTORY


!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_INSTFILES
;--------------------------------
;Languages
!insertmacro MUI_LANGUAGE "English"
;--------------------------------

;RequestExecutionLevel admin
RequestExecutionLevel user
Function .onInit
        # the plugins dir is automatically deleted when the installer exits
        InitPluginsDir
FunctionEnd

;Installer Sections

Function ShowDirectoryPage
    FindWindow $0 "#32770" "" $HWNDPARENT
    GetDlgItem $1 $0 1020 ; IDC_SELDIRTEXT
    StrCpy $2 "Keil Directory"
    SendMessage $1 ${WM_SETTEXT} 0 "STR:$2"
    GetDlgItem $1 $0 1006
    StrCpy $2 "Keil Location"
    SendMessage $1 ${WM_SETTEXT} 0 "STR:$2"
    GetDlgItem $1 $HWNDPARENT 1037
    StrCpy $2 "Keil application path"
    SendMessage $1 ${WM_SETTEXT} 0 "STR:$2"
    GetDlgItem $1 $HWNDPARENT 1038
    StrCpy $2 "Keil Installation directory"
    SendMessage $1 ${WM_SETTEXT} 0 "STR:$2"
FunctionEnd

Section -Post
SectionEnd  

 
Section "Main Install" SECCall01
    ; It will launch the error message box upon failure
    Call CheckForPython
    DetailPrint "PYTHON Executable : $PythonExecutable"
    DetailPrint "PYTHON Root: $PythonRoot"

SectionEnd


