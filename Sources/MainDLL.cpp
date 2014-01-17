#define BOOST_PYTHON_STATIC_LIB
//
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <vector>
#include <WinSCard.h>

using namespace boost::python;
typedef unsigned char ubyte_t;
typedef std::vector<std::string> stringlist;
typedef std::vector<int> intvect_t;
typedef std::vector<ubyte_t> ubytevect_t;

enum returnvalue {
    SUCCESS         = SCARD_S_SUCCESS,
    INTERNAL_ERROR  = SCARD_F_INTERNAL_ERROR,
    CANCELLED       = SCARD_E_CANCELLED
};


class context;

class connector
{
public:
    connector()
    {
    }


    explicit connector(std::string name,context* pctx)
    {
        m_szname = name;
        m_pcontext = pctx;
    }

    ~connector()
    {
        /* Close handle */

    }

    boost::python::long_ connect()
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

    inline void set_name(std::string szname){
        m_szname = szname;
    }

    inline std::string& get_name(void){

        return m_szname;
    }

    boost::python::str get_pythonstring(void)
    {
        return boost::python::str(m_szname.c_str());
    }
    
    boost::python::long_ get_status_change()
    {
        long retval = 12;
        return boost::python::long_(retval);
    }

    boost::python::list transceive(object const& ob)
    {
        stl_input_iterator<ubyte_t> begin(ob), end;
        std::vector<ubyte_t> vs(begin,end);
        for(int i = 0; i<vs.size();++i)
        {
            vs[i] = vs[i] + 2;
        }
        boost::python::object get_iter=boost::python::iterator<std::vector<ubyte_t>>();
        boost::python::object iter = get_iter(vs);
        return boost::python::list(iter);

    }


private:
    std::string m_szname;
    /* context handle */
    SCARDHANDLE m_handle;
    context* m_pcontext;
};


class context
{
public:
    context()
    {
        long ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_ctxhandle);
        
        if (ret != SCARD_S_SUCCESS) {
            //m_errordetail = ret;
            throw_error_already_set();
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
    ~context()
    {
        SCardReleaseContext(m_ctxhandle);
    }
    stringlist enumreaders()
    {
        stringlist retval;
        retval.push_back(std::string("BOOST"));
        retval.push_back(std::string("PYTHON"));
        return retval;
    }
    /* cara lebih panjang tetapi disisi python lebih "elegan" */
    boost::python::list get_list_readers()
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

    connector* get_connector(boost::python::long_ idx )
    {
        if(m_connectorlist.empty())
            throw_error_already_set();

        long val = ::PyLong_AsLong(idx.ptr());
        if(val >= m_connectorlist.size())
        {
            throw_error_already_set();
        }
        return &m_connectorlist[val];
    }
    
    inline SCARDCONTEXT get_handler(){ return m_ctxhandle; }
    
    
    static context* createcontext()
    {
        return &s_context;
    }

private:
    std::vector<connector> m_connectorlist;
    SCARDCONTEXT	m_ctxhandle;

    static context s_context;
};

boost::python::str about()
{
    return boost::python::str("Smart Card Python Module ( Writer: Indra Bagus <indra@xirkachipset.com> )");
}


context context::s_context;


BOOST_PYTHON_MODULE(scard)
{
    enum_<returnvalue>("RETURNVALUE")
        .value("SUCCESS",SUCCESS)
        .value("INTERNAL_ERROR",INTERNAL_ERROR)
        .value("CANCELLED",CANCELLED);


    def("about",&about);
    /* harus ditambahkan jika ingin setiap converter vector<std::string>->boost::python::list berhasil */
    class_<stringlist>("stringlist")
        .def(vector_indexing_suite<stringlist>());
    /* idem dengan atas */
    class_<intvect_t>("intlist")
        .def(vector_indexing_suite<intvect_t>());

    class_<ubytevect_t>("bytelist")
        .def(vector_indexing_suite<ubytevect_t>());

    def("context",&context::createcontext,return_value_policy<reference_existing_object>());
    class_<context>("context")
        .def("get_list_readers",&context::get_list_readers,return_value_policy<return_by_value>())
        .def("enumreaders",&context::enumreaders)
        .def("get_connector",&context::get_connector,return_value_policy<reference_existing_object>());

    class_<connector>("connector")
        .def("get_status_change",&connector::get_status_change)
        .def("get_name",&connector::get_pythonstring)
        .def("transceive",&connector::transceive,return_value_policy<return_by_value>());
}

