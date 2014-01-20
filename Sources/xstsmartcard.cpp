#include "xstsmartcard.h"

boost::python::long_ connector::connect()
{
    LONG lretval;
    DWORD dwproto = 0;
    lretval = ::SCardConnectA(m_pcontext->get_handler(),
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

boost::python::tuple connector::transceive(boost::python::object const& ob)
{
    if(m_handle == NULL)
        boost::python::throw_error_already_set();


    boost::python::stl_input_iterator<ubyte_t> begin(ob), end;
    std::vector<ubyte_t> vectinput(begin,end);
    if(vectinput.size() < 5)
        boost::python::throw_error_already_set();

    m_io_request.dwProtocol = m_prototype;
    m_io_request.cbPciLength = static_cast<DWORD>(sizeof(SCARD_IO_REQUEST));
    std::vector<ubyte_t> response;
    static ubyte_t respbuffer[260];
    DWORD dwlength = sizeof(respbuffer);
    LONG retval = ::SCardTransmit(m_handle,&m_io_request,vectinput.data(),vectinput.size(),0,respbuffer,&dwlength);
    unsigned long lstatus = -1;
    if(retval == SCARD_S_SUCCESS)
    {
        lstatus = (respbuffer[dwlength-2] << 8) | respbuffer[dwlength-1];
        if(dwlength > 2)
            response.assign(respbuffer,respbuffer+(dwlength-2));
        
    }
    boost::python::object get_iter=boost::python::iterator<std::vector<ubyte_t>>();
    boost::python::object iter = get_iter(response);
    return boost::python::make_tuple(boost::python::long_(lstatus),boost::python::list(iter));
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
                        Context goes here 
    =========================================================== 
*/

context context::s_context;

context::context()
{
    long ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_ctxhandle);

    if (ret != SCARD_S_SUCCESS) {
        //m_errordetail = ret;
        boost::python::throw_error_already_set();
        //return -1;//(IDCANCEL);
    }

    LPTSTR prespbuffer;
    LPTSTR szreader;
    DWORD resplength = SCARD_AUTOALLOCATE;
    ret = SCardListReaders(m_ctxhandle, 0, (LPTSTR)&prespbuffer, &resplength);

    if (ret != SCARD_S_SUCCESS) {
        //PyErr_SetString(PyExc_BufferError,"No Readers Attached");
        //PyErr_Occurred();
        //throw_error_already_set();
        //m_errordetail = ret;
        //return -1;//(IDCANCEL);
        return;
    }


    szreader = prespbuffer;
    while(*szreader != '\0')
    {
        std::string readername(szreader);
        m_connectorlist.push_back(connector(readername,this));
        szreader = szreader + strlen((char*)szreader) + 1;
    }
    SCardFreeMemory(m_ctxhandle,prespbuffer);

}


context::~context()
{
    SCardReleaseContext(m_ctxhandle);
}

connector* context::get_connector(boost::python::long_ idx )
{
    if(m_connectorlist.empty())
        boost::python::throw_error_already_set();
    size_t val = ::PyLong_AsSize_t(idx.ptr());
    if(val >= m_connectorlist.size())
    {
        boost::python::throw_error_already_set();
    }
    return &m_connectorlist[val];
}


boost::python::list context::get_list_readers()
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