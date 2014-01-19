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

    boost::python::long_ connect();

    inline void set_name(std::string szname)
    {
        m_szname = szname;
    }

    inline std::string& get_name(void)
    {
        return m_szname;
    }

    inline boost::python::str get_pythonstring(void)
    {
        return boost::python::str(m_szname.c_str());
    }

    boost::python::long_ get_status_change()
    {
        long retval = 12;
        return boost::python::long_(retval);
    }

    boost::python::list transceive(boost::python::object const& ob);

private:
    std::string m_szname;
    /* context handle */
    SCARDHANDLE m_handle;
    context* m_pcontext;
};


class context
{
public:
    context();
    ~context();

    //stringlist enumreaders()
    //{
    //    stringlist retval;
    //    retval.push_back(std::string("BOOST"));
    //    retval.push_back(std::string("PYTHON"));
    //    return retval;
    //}


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


