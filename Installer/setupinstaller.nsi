/*
 * NSIS script untuk installer Xirka Emulator / Development Board
 *  
 */
!include "MUI2.nsh"
!include "logiclib.nsh"
!include "python.nsh"

!define Version 2.5.0.1
!define PRODUCT_NAME "SCard "
!define PRODUCT_VERSION "${Version}"
!define PRODUCT_PUBLISHER "PT Xirka Silicon Technology"
!define PRODUCT_WEB_SITE "http://xirkachipset.com"
!define COMPANY_NAME "Xirka Silicon Technology"

Name "${PRODUCT_NAME}"
OutFile "scardsetup-${Version}.exe"
InstallDir "${DEFAULTPYTHONPATH}"

!define MUI_ABORTWARNING
!define MUI_ICON "Graphics\xsmartcard.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
;--------------------------------
;Pages
!define MUI_WELCOMEPAGE_TITLE 'Xirka Silicon Technology'
!define MUI_WELCOMEPAGE_TEXT 'Python Extension for Xirka Smart Card Module'
!insertmacro MUI_PAGE_WELCOME


!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_INSTFILES
;--------------------------------
;Languages
!insertmacro MUI_LANGUAGE "English"
;--------------------------------

VIProductVersion ${Version}
VIAddVersionKey ProductName "Xirka Smart Card Python Extension"
VIAddVersionKey Comments "Python 3.4 extension for accessing Smart Card written by Indra Bagus <indra@xirkachipset.com>"
VIAddVersionKey CompanyName "Xirka Silicon Technology"
VIAddVersionKey LegalCopyright "Copyright to Xirka Silicon Technology"
VIAddVersionKey FileDescription "SCard Python Extension from Xirka Silicon Technology"
VIAddVersionKey FileVersion ${Version}
VIAddVersionKey ProductVersion ${Version}
VIAddVersionKey InternalName "Python Smart Card Extension"
VIAddVersionKey LegalTrademarks "Python Smart Card Extension is a Trademark of Xirka Silicon Technology."


RequestExecutionLevel admin
;RequestExecutionLevel user
Function .onInit
        # the plugins dir is automatically deleted when the installer exits
        InitPluginsDir
FunctionEnd

;Installer Sections


Section -Post
SectionEnd  

 
Section "Main Install" SECCall01
    ; It will launch the error message box upon failure
    Call CheckForPython
    StrCmp $PythonExecutable "" NotFound Found

    NotFound:
        MessageBox MB_OK|MB_ICONSTOP "${STRING_PYTHON_NOT_FOUND}"
        Quit
    
    Found:
        IfFileExists $PythonRoot\DLLs\scard.pyd 0 +2
            Delete $PythonRoot\DLLs\scard.pyd
        ;SetOverwrite ifnewer 
        File /oname=$SYSDIR\msvcp120.dll "Sources\msvcp120.dll"
        File /oname=$SYSDIR\msvcr120.dll "Sources\msvcr120.dll"
        File /oname=$SYSDIR\msvcr120.dll "Sources\MP300Com.dll"        
        File /oname=$PythonRoot\DLLs\_scard.pyd "Sources\_scard.pyd"
        File /oname=$PythonRoot\Lib\scard.py "Sources\scard.py"
        File /oname=$PythonRoot\DLLs\_scard.pyd "Sources\_mp300.pyd"
        File /oname=$PythonRoot\Lib\scard.py "Sources\mp300.py"        

SectionEnd

; Replace the following strings to suite your needs
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "Application was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove ${PRODUCT_NAME} and all of its components?" IDYES +2
  Abort
FunctionEnd

Section "Uninstall"
    SetShellVarContext all
    Call un.CheckForPython
    StrCmp $PythonExecutable "" NotFound Found

    NotFound:
        MessageBox MB_OK "${STRING_PYTHON_NOT_FOUND}"
        Quit
    
    Found:
        Delete $PythonRoot\DLLs\_scard.pyd
        Delete $PythonRoot\Lib\scard.py
        Delete $SYSDIR\msvcp120.dll
        Delete $SYSDIR\msvcr120.dll
    
SectionEnd


