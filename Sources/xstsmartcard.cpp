#include "stdafx.h"
#include "xstsmartcard.h"


static void throw_systemerror(const char* message,LONG errorcode)
{
    char *err;
    if (!::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        errorcode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
        (LPTSTR)&err,
        0,
        NULL))
    {
        PyErr_Format(PyExc_SystemError, "%s, errcode: 0x%x", message, errorcode);
        boostpy::throw_error_already_set();
        return;
    }

    PyErr_Format(PyExc_SystemError, "%s, errcode: 0x%x message: %s", message, errorcode,err);
    ::LocalFree(err);
    boostpy::throw_error_already_set();
}

static void throw_systemerror(const char* message)
{
    PyErr_Format(PyExc_SystemError,"%s", message);
    boostpy::throw_error_already_set();
}

void connector::connect()
{
    LONG lretval;
    DWORD dwproto = 0;
    lretval = ::SCardConnect(m_psccontext->get_handler(),
        (LPCTSTR)m_szname.c_str(),
        SCARD_SHARE_EXCLUSIVE,
        SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
        &m_handle,&dwproto);

    if (lretval != SCARD_S_SUCCESS)
    {
        throw_systemerror("Failed to establishes a connection", lretval);
    }
    m_prototype = dwproto;
}

void connector::direct_connect()
{
    LONG lretval;
    DWORD dwproto = 0;
    lretval = ::SCardConnect(m_psccontext->get_handler(),
        (LPCTSTR)m_szname.c_str(),
        SCARD_SHARE_DIRECT,
        0,
        &m_handle, &dwproto);

    if (lretval != SCARD_S_SUCCESS)
    {
        throw_systemerror("Failed to establishes a connection", lretval);
    }
    m_prototype = 0;
}

void connector::disconnect()
{
    if(m_handle){
        SCardDisconnect(m_handle, SCARD_LEAVE_CARD);
        m_handle = NULL;
    }
    else{
        throw_systemerror("your silly soul tried to disconnect something that is not connected",-1);
    }
}

void connector::reset()
{
    if (m_handle){
        SCardDisconnect(m_handle, SCARD_RESET_CARD);
        m_handle = NULL;
    }
    else{
        throw_systemerror("your silly soul tried to reset something that is not connected",-1);
    }
}

void connector::unpowered()
{
    if (m_handle){
        SCardDisconnect(m_handle, SCARD_UNPOWER_CARD);
        m_handle = NULL;
    }
    else{
        throw_systemerror("your silly's soul tried to unpowered something that is not connected",-1);
    }
}

void connector::eject()
{
    if(m_handle){
        SCardDisconnect(m_handle,SCARD_EJECT_CARD);
        m_handle = NULL;
    }else{
        throw_systemerror("your silly's soul tried to eject something that is not connected", -1);
    }
}

#ifdef USING_PYTHONLIST_TRANSCEIVE
boostpy::object connector::transceive(boostpy::object const& ob)
{
    if(m_handle == NULL)
        boostpy::throw_error_already_set();


    boostpy::stl_input_iterator<ubyte_t> begin(ob),end;
    std::vector<ubyte_t> vectinput(begin,end);

    m_io_request.dwProtocol = m_prototype;
    m_io_request.cbPciLength = static_cast<DWORD>(sizeof(SCARD_IO_REQUEST));
    std::vector<ubyte_t> response;
    DWORD dwlength = RXBUFFERSIZE;
    LONG retval = ::SCardTransmit(m_handle,&m_io_request,vectinput.data(),vectinput.size(),0,m_rxbuffer,&dwlength);
    if(retval != SCARD_S_SUCCESS)
    {
        throw_systemerror("Error during transmission", retval);
        
    }
    response.assign(m_rxbuffer, m_rxbuffer + dwlength);
    boostpy::object get_iter=boostpy::iterator<std::vector<ubyte_t>>();
    boostpy::object iter = get_iter(response);
    return boostpy::list(iter);
}

#elif defined USING_PYTHONBUFFER_TRANSCEIVE
boostpy::object connector::transceive(boostpy::object const& obj)
{
    Py_buffer buffer;
    if (m_handle == NULL){
        PyErr_Format(PyExc_SystemError, "no connection handle detected");
        boostpy::throw_error_already_set();
    }

    if (!PyObject_CheckBuffer(obj.ptr())){
        PyErr_Format(PyExc_TypeError, "Type must support buffer interface");
        boostpy::throw_error_already_set();

    }

    LONG retval = PyObject_GetBuffer(obj.ptr(), &buffer, PyBUF_SIMPLE);
    if (retval == -1)
    {
        boostpy::throw_error_already_set();
    }

    BYTE* pbuff = static_cast<BYTE*>(buffer.buf);

    m_io_request.dwProtocol = m_prototype;
    m_io_request.cbPciLength = static_cast<DWORD>(sizeof(SCARD_IO_REQUEST));
    DWORD dwlength = RXBUFFERSIZE;
    retval = ::SCardTransmit(m_handle, &m_io_request, pbuff, buffer.len, 0, m_rxbuffer, &dwlength);
    if (retval != SCARD_S_SUCCESS)
    {
        PyBuffer_Release(&buffer);
        throw_systemerror("Error during transmission", retval);

    }
    PyBuffer_Release(&buffer);

    /* now deliver the response to python */
    PyObject* pobj = Py_BuildValue("y#", m_rxbuffer, dwlength);
    return boostpy::object(boostpy::handle<>(pobj));
}


#endif



READERSTATE* connector::get_readerstate()
{
    SCARD_READERSTATE readerstate;
    LONG lretval;
    DWORD dwtimeout = 100;
    DWORD dwreaderstatesize = 1;
    ::ZeroMemory(&readerstate, sizeof(SCARD_READERSTATE));
    readerstate.szReader = m_szname.c_str();
    lretval = ::SCardGetStatusChange(m_psccontext->get_handler(), dwtimeout, &readerstate, dwreaderstatesize);
    if (lretval != SCARD_S_SUCCESS)
        throw_systemerror("Error retrieving reader state", lretval);
    m_readerstate.current_state = readerstate.dwCurrentState;
    m_readerstate.event_state = readerstate.dwEventState;
    return &m_readerstate;
}


boostpy::long_ connector::get_transmit_count()
{
    DWORD dwcount;
    LONG lretval = ::SCardGetTransmitCount(m_handle, &dwcount);
    if (lretval != SCARD_S_SUCCESS)
    {
        throw_systemerror("Error retrieving the number of transmit operation", lretval);
    }
    return boostpy::long_(dwcount);
}

boostpy::object connector::direct_control(boostpy::long_ ctl, boostpy::object const& ob)
{
    /*
    public const long FILE_DEVICE_SMARTCARD = 0x310000; // Reader action IOCTLs
    public const long IOCTL_SMARTCARD_DIRECT = FILE_DEVICE_SMARTCARD + 2050 * 4;
    public const long IOCTL_SMARTCARD_SELECT_SLOT = FILE_DEVICE_SMARTCARD + 2051 * 4;
    public const long IOCTL_SMARTCARD_DRAW_LCDBMP = FILE_DEVICE_SMARTCARD + 2052 * 4;
    public const long IOCTL_SMARTCARD_DISPLAY_LCD = FILE_DEVICE_SMARTCARD + 2053 * 4;
    public const long IOCTL_SMARTCARD_CLR_LCD = FILE_DEVICE_SMARTCARD + 2054 * 4;
    public const long IOCTL_SMARTCARD_READ_KEYPAD = FILE_DEVICE_SMARTCARD + 2055 * 4;
    public const long IOCTL_SMARTCARD_READ_RTC = FILE_DEVICE_SMARTCARD + 2057 * 4;
    public const long IOCTL_SMARTCARD_SET_RTC = FILE_DEVICE_SMARTCARD + 2058 * 4;
    public const long IOCTL_SMARTCARD_SET_OPTION = FILE_DEVICE_SMARTCARD + 2059 * 4;
    public const long IOCTL_SMARTCARD_SET_LED = FILE_DEVICE_SMARTCARD + 2060 * 4;
    public const long IOCTL_SMARTCARD_LOAD_KEY = FILE_DEVICE_SMARTCARD + 2062 * 4;
    public const long IOCTL_SMARTCARD_READ_EEPROM = FILE_DEVICE_SMARTCARD + 2065 * 4;
    public const long IOCTL_SMARTCARD_WRITE_EEPROM = FILE_DEVICE_SMARTCARD + 2066 * 4;
    public const long IOCTL_SMARTCARD_GET_VERSION = FILE_DEVICE_SMARTCARD + 2067 * 4;
    public const long IOCTL_SMARTCARD_GET_READER_INFO = FILE_DEVICE_SMARTCARD + 2051 * 4;
    public const long IOCTL_SMARTCARD_SET_CARD_TYPE = FILE_DEVICE_SMARTCARD + 2060 * 4;
    public const long IOCTL_SMARTCARD_ACR128_ESCAPE_COMMAND = FILE_DEVICE_SMARTCARD + 2079 * 4;
    public const long IOCTL_CCID_ESCAPE_SCARD_CTL_CODE = FILE_DEVICE_SMARTCARD + 3500 * 4;
    */
    Py_buffer buffer;
    if (m_handle == NULL){
        PyErr_Format(PyExc_SystemError, "no connection handle detected");
        boostpy::throw_error_already_set();
    }

    if (!PyObject_CheckBuffer(ob.ptr())){
        PyErr_Format(PyExc_TypeError, "Type must support buffer interface");
        boostpy::throw_error_already_set();

    }

    LONG retval = PyObject_GetBuffer(ob.ptr(), &buffer, PyBUF_SIMPLE);
    if (retval == -1)
    {
        boostpy::throw_error_already_set();
    }
    BYTE* pbuff = static_cast<BYTE*>(buffer.buf);
    DWORD dwreturned;
    DWORD dwbufferlen = RXBUFFERSIZE;
    DWORD dwctl = ::PyLong_AsLong(ctl.ptr());
    LONG lretval = ::SCardControl(m_handle, 
                        dwctl, 
                        pbuff,
                        buffer.len,
                        m_rxbuffer, 
                        dwbufferlen, 
                        &dwreturned);
    if (retval != SCARD_S_SUCCESS)
    {
        PyBuffer_Release(&buffer);
        throw_systemerror("Error during transmission", retval);

    }
    PyBuffer_Release(&buffer);

    /* now deliver the response to python */
    PyObject* pobj = Py_BuildValue("y#", m_rxbuffer, dwreturned);
    return boostpy::object(boostpy::handle<>(pobj));
}


boostpy::object connector::get_attribute(DWORD attrid, LPCTSTR strpytype)
{
    LONG lretval;
    LPBYTE pbattr;
    DWORD dwlen = SCARD_AUTOALLOCATE;
    lretval = ::SCardGetAttrib(m_handle, attrid, (LPBYTE)&pbattr, &dwlen);
    if (lretval != SCARD_S_SUCCESS){
        throw_systemerror("Failed to get ATR string", lretval);
    }
    PyObject* pobj = Py_BuildValue(strpytype, pbattr, dwlen);
    ::SCardFreeMemory(this->m_psccontext->get_handler(), (LPCVOID)pbattr);
    return boostpy::object(boostpy::handle<>(pobj));
}

/* 
    ========================================================================================================
                                    sccontext Class Implementation 
    ========================================================================================================
*/

const char* sccontext::class_doc = "The smart card sccontext management that mostly used "
                                    "to create connector and readers enumeration";

SCARD_READERSTATE sccontext::s_readerstate[MAXIMUM_SMARTCARD_READERS];

sccontext::sccontext()
{
    long ret = ::SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_ctxhandle);

    if (ret != SCARD_S_SUCCESS)
        throw_systemerror("Failed to established smart card context",ret);

    LPTSTR prespbuffer;
    LPTSTR szreader;
    DWORD resplength = SCARD_AUTOALLOCATE;
    ret = ::SCardListReaders(m_ctxhandle, 0, (LPTSTR)&prespbuffer, &resplength);

    /* I'll let python user know that list reader will returned empy list */
    if (ret != SCARD_S_SUCCESS) {
        return;
    }


    szreader = prespbuffer;
    while(*szreader != '\0')
    {
        std::string readername(szreader);
        m_connectorlist.push_back(new connector(readername,this));
        szreader = szreader + strlen((char*)szreader) + 1;
    }
    ::SCardFreeMemory(m_ctxhandle,prespbuffer);

    // check the initial status/event
    for (std::size_t i = 0; i < m_connectorlist.size(); ++i)
    {
        s_readerstate[i].szReader = (LPCTSTR)m_connectorlist[i]->get_name().c_str();
        s_readerstate[i].dwCurrentState = SCARD_STATE_UNAWARE;
        s_readerstate[i].dwEventState = SCARD_STATE_UNKNOWN;
    }
    ret = SCardGetStatusChange(this->m_ctxhandle, INFINITE, s_readerstate, m_connectorlist.size());

}


sccontext::~sccontext()
{
    std::vector<connector*>::iterator itercon = m_connectorlist.begin();
    while (itercon != m_connectorlist.end())
    {
        delete *itercon;
        ++itercon;
    }
    ::SCardReleaseContext(m_ctxhandle);
}

connector* sccontext::get_connector(boostpy::long_ idx )
{
    if(m_connectorlist.empty()){
        PyErr_SetString(PyExc_SystemError,"No connector detected");
        boostpy::throw_error_already_set();
    }

    size_t val = ::PyLong_AsSize_t(idx.ptr());
    if(val >= m_connectorlist.size())
    {
        boostpy::throw_error_already_set();
    }
    return m_connectorlist[val];
}

connector* sccontext::get_connector_byname(boostpy::str szname)
{
    std::string stdstrname = boostpy::extract<const char*>(szname);
    connectorlist_t::iterator iter = m_connectorlist.begin();
    while (iter != m_connectorlist.end())
    {
        if ((*iter)->get_name() == stdstrname)
            return (*iter);
    }
    throw_systemerror("Connector's name unrecognize");
}


boostpy::list sccontext::get_list_readers()
{
    std::vector<std::string> strlist;
    std::vector<connector*>::iterator itercon = m_connectorlist.begin();
    while(itercon != m_connectorlist.end())
    {
        strlist.push_back((*itercon)->get_name());
        ++itercon;
    }
    boostpy::object get_iter=boostpy::iterator<std::vector<std::string>>();
    boostpy::object iter = get_iter(strlist);
    return boostpy::list(iter);
}

boostpy::list sccontext::get_status_change(boostpy::long_ tmout)
{
    statelist_t lists;
    //DWORD dwStateMask = ~(SCARD_STATE_INUSE |
    //    SCARD_STATE_EXCLUSIVE |
    //    SCARD_STATE_UNAWARE |
    //    SCARD_STATE_IGNORE |
    //    SCARD_STATE_CHANGED);
    
    for (int i = 0; i < m_connectorlist.size(); ++i){
        s_readerstate[i].dwCurrentState = s_readerstate[i].dwEventState;
    }

    DWORD tmval = boostpy::extract<DWORD>(tmout);
    LONG lret = ::SCardGetStatusChange(this->m_ctxhandle, 
                                        INFINITE,
                                        s_readerstate,
                                        m_connectorlist.size());
    if (lret != SCARD_S_SUCCESS)
    {
        throw_systemerror("Error to get status change", lret);
    }
    
    for (std::size_t i = 0; i < m_connectorlist.size(); ++i)
    {
        READERSTATE state;
        state.current_state = s_readerstate[i].dwCurrentState;
        state.event_state= s_readerstate[i].dwEventState;
        lists.push_back(state);
    }
    boostpy::object get_iter = boostpy::iterator<statelist_t>();
    boostpy::object iter = get_iter(lists);
    return boostpy::list(iter);
}
