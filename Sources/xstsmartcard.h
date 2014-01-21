#pragma once

/* we use static library */
#ifndef BOOST_PYTHON_STATIC_LIB
#define BOOST_PYTHON_STATIC_LIB
#endif

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <vector>
#include <WinSCard.h>

#define RXBUFFERSIZE    260

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
    connector():m_handle(NULL) { 
        /* ctor */ 
    }
    
    explicit connector(std::string name,context* pctx):m_handle(NULL)
    {
        m_szname = name;
        m_pcontext = pctx;
    }

    ~connector()
    {
        /* Close handle */
        this->disconnect();

    }

    boost::python::long_ connect();

    void disconnect();

    inline void set_name(std::string szname){m_szname = szname;}

    inline std::string& get_name(void){return m_szname;}

    inline boost::python::str get_pythonstring(void) { return boost::python::str(m_szname.c_str()); }

    boost::python::long_ get_current_event();

    boost::python::tuple transceive(boost::python::object const& ob);

private:
    std::string m_szname;
    /* context handle */
    SCARDHANDLE m_handle;
    context* m_pcontext;
    DWORD m_prototype;
    SCARD_IO_REQUEST m_io_request;
    ubyte_t m_rxbuffer[RXBUFFERSIZE];
};


class context
{
public:
    context();
    ~context();

    /* cara lebih panjang tetapi disisi python lebih "elegan" */
    boost::python::list get_list_readers();

    connector* get_connector(boost::python::long_ idx );

    inline SCARDCONTEXT get_handler(){ return m_ctxhandle; }
    
    static inline context* get_context(){ return &s_context; }

private:
    std::vector<connector> m_connectorlist;
    SCARDCONTEXT	m_ctxhandle;

    static context s_context;
};



