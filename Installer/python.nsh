/*
 * NSIS script untuk installer Xirka Emulator / Development Board
 *  
 */
!define DEFAULTPYTHONPATH "C:\Python"

!define STRING_PYTHON_NOT_FOUND "Python 3.4 is not installed on this system. $\nPlease install Python 3.4 first. $\n$\nClick OK to cancel installation and remove installation Files."

!define STRING_PYTHON_CURRENT_USER_FOUND "Python is installed for the current user only. $\n$\n${PRODUCT_NAME} does not support use with Python so configured. $\n$\nClick OK to cancel installation and remove installation Files."

Var PythonRoot
Var PythonExecutable

; This must be a macro because the registry root key cannot be a variable
!macro EnumeratePython id hk_root hk
    StrCpy $0 0
    loop${id}:
        ClearErrors
        EnumRegKey $1 ${hk_root} ${hk} $0
        StrCmp $1 "" done${id}
        IntOp $0 $0 + 1
        ReadRegStr $7 ${hk_root} "${hk}\$1\InstallPath" ""
        StrCpy $PythonExecutable "$7\python.exe"
        StrCpy $PythonRoot $7
        Call ValidatePythonVersion
        Pop $7
        StrCmp $7 0 found loop${id}    

    done${id}:
        StrCpy $PythonExecutable ""
        StrCpy $PythonRoot ""        
!macroend




Function ValidatePythonVersion
  ClearErrors
  ; We should work in python 32 bit
  nsExec::ExecToStack '"$PythonExecutable" "-c" "import sys; exit ({True:0,False:1}[sys.maxsize < 2**32])"'
  Pop $1
  IntCmp $1 0 +3 0
  Push 1
  Return
  nsExec::ExecToStack '"$PythonExecutable" "-c" "import sys; ver=sys.version_info[:2]; exit({True:0,False:1}[ver>=(3,0) and ver<=(3,4)])"'
FunctionEnd



Function CheckForPython
    ; Find all in these path, however if your system 64 bit and you installed 32 bit we still able to enumerate the HKLM\Software\Python 
    ; since it will  mapped to HKLM\Software\Wow6432Node\Python
    !insertmacro EnumeratePython 1 HKLM Software\Python\PythonCore
    !insertmacro EnumeratePython 2 HKLM Software\Wow6432Node\Python\PythonCore
    !insertmacro EnumeratePython 3 HKCU Software\Python\PythonCore
    !insertmacro EnumeratePython 4 HKCU Software\Wow6432Node\Python\PythonCore
        
    found:
        Return

FunctionEnd


