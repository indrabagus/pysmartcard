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
        boost::python::throw_error_already_set();
        return;
    }

    PyErr_Format(PyExc_SystemError, "%s, errcode: 0x%x message: %s", message, errorcode,err);
    ::LocalFree(err);
    boost::python::throw_error_already_set();
}

void connector::connect()
{
    LONG lretval;
    DWORD dwproto = 0;
    lretval = ::SCardConnect(m_pcontext->get_handler(),
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
    lretval = ::SCardConnect(m_pcontext->get_handler(),
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
        SCardDisconnect(m_handle,SCARD_EJECT_CARD);
        m_handle = NULL;
    }
}

boost::python::list connector::transceive(boost::python::object const& ob)
{
    if(m_handle == NULL)
        boost::python::throw_error_already_set();


    boost::python::stl_input_iterator<ubyte_t> begin(ob),end;
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
    boost::python::object get_iter=boost::python::iterator<std::vector<ubyte_t>>();
    boost::python::object iter = get_iter(response);
    return boost::python::list(iter);
}



READERSTATE* connector::get_readerstate()
{
    SCARD_READERSTATE readerstate;
    LONG lretval;
    DWORD dwtimeout = 100;
    DWORD dwreaderstatesize = 1;
    ::ZeroMemory(&readerstate, sizeof(SCARD_READERSTATE));
    readerstate.szReader = m_szname.c_str();
    lretval = ::SCardGetStatusChange(m_pcontext->get_handler(), dwtimeout, &readerstate, dwreaderstatesize);
    if (lretval != SCARD_S_SUCCESS)
        throw_systemerror("Error retrieving reader state", lretval);
    m_readerstate.current_state = readerstate.dwCurrentState;
    m_readerstate.event_state = readerstate.dwEventState;
    return &m_readerstate;
}


boost::python::long_ connector::get_transmit_count()
{
    DWORD dwcount;
    LONG lretval = ::SCardGetTransmitCount(m_handle, &dwcount);
    if (lretval != SCARD_S_SUCCESS)
    {
        throw_systemerror("Error retrieving the number of transmit operation", lretval);
    }
    return boost::python::long_(dwcount);
}

boost::python::list connector::direct_control(boost::python::long_ ctl, boost::python::object const& ob)
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
    if (m_handle == NULL)
        boost::python::throw_error_already_set();

    DWORD dwreturned;
    boost::python::stl_input_iterator<ubyte_t> begin(ob), end;
    std::vector<ubyte_t> vectinput(begin, end);

    std::vector<ubyte_t> response;
    DWORD dwbufferlen = RXBUFFERSIZE;
    DWORD dwctl = ::PyLong_AsLong(ctl.ptr());
    LONG lretval = ::SCardControl(m_handle, 
                        dwctl, 
                        vectinput.data(), 
                        vectinput.size(), 
                        m_rxbuffer, 
                        dwbufferlen, 
                        &dwreturned);
    if (lretval != SCARD_S_SUCCESS)
    {
        throw_systemerror("Error during direct control", lretval);

    }
    response.assign(m_rxbuffer, m_rxbuffer + dwreturned);
    boost::python::object get_iter = boost::python::iterator<std::vector<ubyte_t>>();
    boost::python::object iter = get_iter(response);
    return boost::python::list(iter);
}

/* 
    ===========================================================
                        sccontext goes here 
    =========================================================== 
*/

sccontext::sccontext()
{
    long ret = ::SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_ctxhandle);

    if (ret != SCARD_S_SUCCESS) {

        char *err;
        if (!::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                           NULL,
                           ret,
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
                           (LPTSTR) &err,
                           0,
                           NULL))
        {
            PyErr_SetString(PyExc_SystemError,"Failed on established smart card context");
            boost::python::throw_error_already_set();
            return;
        }

        PyErr_SetString(PyExc_SystemError,err);
        ::LocalFree(err);
        boost::python::throw_error_already_set();
    }

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
        m_connectorlist.push_back(connector(readername,this));
        szreader = szreader + strlen((char*)szreader) + 1;
    }
    ::SCardFreeMemory(m_ctxhandle,prespbuffer);

}


sccontext::~sccontext()
{
    ::SCardReleaseContext(m_ctxhandle);
}

connector* sccontext::get_connector(boost::python::long_ idx )
{
    if(m_connectorlist.empty()){
        PyErr_SetString(PyExc_SystemError,"No connector detected");
        boost::python::throw_error_already_set();
    }

    size_t val = ::PyLong_AsSize_t(idx.ptr());
    if(val >= m_connectorlist.size())
    {
        boost::python::throw_error_already_set();
    }
    return &m_connectorlist[val];
}


boost::python::list sccontext::get_list_readers()
{
    std::vector<std::string> strlist;
    std::vector<connector>::iterator itercon = m_connectorlist.begin();
    while(itercon != m_connectorlist.end())
    {
        strlist.push_back((*itercon).get_name());
        ++itercon;
    }
    boost::python::object get_iter=boost::python::iterator<std::vector<std::string>>();
    boost::python::object iter = get_iter(strlist);
    return boost::python::list(iter);
}


