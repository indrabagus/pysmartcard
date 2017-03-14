#ifndef SCARDGETATTR_HPP
#define SCARDGETATTR_HPP
#include <windows.h>
#include <Winscard.h>

template<typename T>
T get_attribute(DWORD dwattr)
{
    LONG lretval;
    LPBYTE pbattr;
    DWORD dwlen = SCARD_AUTOALLOCATE;
    lretval = ::SCardGetAttrib(m_handle, dwattr, (LPBYTE)&pbattr, &dwlen);
    if (lretval != SCARD_S_SUCCESS){
        throw_systemerror("Failed to get ATR string", lretval);
    }
    PyObject* pobj = Py_BuildValue("y#", pbattr, dwlen);
    ::SCardFreeMemory(this->m_psccontext->get_handler(), (LPCVOID)pbattr);
    return T(boostpy::handle<>(pobj));
}

#endif