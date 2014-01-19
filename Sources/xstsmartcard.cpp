#include "xstsmartcard.h"

boost::python::long_ connector::connect()
{
    LONG lretval;
    DWORD dwproto;
    lretval = ::SCardConnectA(m_pcontext->get_handler(),
        m_szname.c_str(),
        SCARD_SHARE_SHARED,
        SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1,
        &m_handle,&dwproto);

    return boost::python::long_(lretval);
}



boost::python::list connector::transceive(boost::python::object const& ob)
{
    boost::python::stl_input_iterator<ubyte_t> begin(ob), end;
    std::vector<ubyte_t> vs(begin,end);
    for(size_t i = 0; i<vs.size();++i)
    {
        vs[i] = vs[i] + 2;
    }
    boost::python::object get_iter=boost::python::iterator<std::vector<ubyte_t>>();
    boost::python::object iter = get_iter(vs);
    return boost::python::list(iter);
}




/* Context goes here */
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