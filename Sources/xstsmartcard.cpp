#include "xstsmartcard.h"

boost::python::long_ connector::connect()
{
    LONG lretval;
    DWORD dwproto = 0;
    lretval = ::SCardConnect(m_pcontext->get_handler(),
        (LPCTSTR)m_szname.c_str(),
        SCARD_SHARE_EXCLUSIVE,
        SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
        &m_handle,&dwproto);

    m_prototype = dwproto;

    return boost::python::long_(lretval);
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


    boost::python::stl_input_iterator<ubyte_t> begin(ob), end;
    std::vector<ubyte_t> vectinput(begin,end);

    m_io_request.dwProtocol = m_prototype;
    m_io_request.cbPciLength = static_cast<DWORD>(sizeof(SCARD_IO_REQUEST));
    std::vector<ubyte_t> response;
    DWORD dwlength = RXBUFFERSIZE;
    LONG retval = ::SCardTransmit(m_handle,&m_io_request,vectinput.data(),vectinput.size(),0,m_rxbuffer,&dwlength);
    if(retval == SCARD_S_SUCCESS)
    {
        response.assign(m_rxbuffer,m_rxbuffer+dwlength);
        
    }

    boost::python::object get_iter=boost::python::iterator<std::vector<ubyte_t>>();
    boost::python::object iter = get_iter(response);
    return boost::python::list(iter);
}

boost::python::long_ connector::get_current_event()
{
    SCARD_READERSTATE readerstate;
    LONG lretval ;
    DWORD dwtimeout = 100;
    DWORD dwreaderstatesize=1;
    ::ZeroMemory(&readerstate,sizeof(SCARD_READERSTATE));
    readerstate.szReader = m_szname.c_str();
    lretval = ::SCardGetStatusChange(m_pcontext->get_handler(),dwtimeout,&readerstate,dwreaderstatesize);
    if(lretval != SCARD_S_SUCCESS)
        return boost::python::long_(0x00);

    return boost::python::long_(readerstate.dwEventState);
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
        static char buffer[255];
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
        PyErr_SetString(PyExc_SystemError,"Connector list is empty");
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


