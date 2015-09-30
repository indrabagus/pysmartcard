/*
 * Simple Python module to access SmartCard Reader
 * Written by Indra Bagus <indra.bagus@gmail.com>
 */

#pragma once

/* we use static library */
#ifndef BOOST_PYTHON_STATIC_LIB
#define BOOST_PYTHON_STATIC_LIB
#endif

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/smart_ptr.hpp>

#include <vector>
#include <WinSCard.h>

#define RXBUFFERSIZE    260
#define IOCTL_CCID_ESCAPE_SCARD_CTL_CODE        SCARD_CTL_CODE(3500)
#define IOCTL_SMARTCARD_ACR128_ESCAPE_COMMAND   SCARD_CTL_CODE(2079)
namespace boostpy = boost::python;


typedef unsigned char ubyte_t;
typedef std::vector<std::string> stringlist;
typedef std::vector<int> intvect_t;
typedef std::vector<ubyte_t> ubytevect_t;

class sccontext;

struct READERSTATE
{
    unsigned int current_state;
    unsigned int event_state;
};

class connector
{
public:
    connector():m_handle(NULL) { 
        /* ctor */ 
    }
    
    explicit connector(std::string name,sccontext* pctx):m_handle(NULL)
    {
        m_szname = name;
        m_psccontext = pctx;
    }

    ~connector()
    {
        if (m_handle != NULL)
        {
            /* Close handle */
            this->disconnect();
            m_handle=NULL;
        }

    }

    void connect();
    void direct_connect();
    void disconnect();
    void reset();
    void unpowered();
    void eject();


    READERSTATE* get_readerstate();
    boost::python::object transceive(boost::python::object const& ob);
    boost::python::long_ get_transmit_count();
    boost::python::object direct_control(boost::python::long_ ctl, boost::python::object const& ob);
    // Inline section
    inline void set_name(std::string szname){ 
        m_szname = szname; 
    }
    
    inline std::string& get_name(void){ 
        return m_szname; 
    }
    
    inline boost::python::str get_pythonname(void) { 
        return boostpy::str(m_szname.c_str()); 
    }
private:
    std::string m_szname;
    /* sccontext handle */
    SCARDHANDLE m_handle;
    sccontext* m_psccontext;
    DWORD m_prototype;
    SCARD_IO_REQUEST m_io_request;
    ubyte_t m_rxbuffer[RXBUFFERSIZE];
    READERSTATE m_readerstate;
};


class sccontext
{
public:
    static const char* class_doc;

public:
    sccontext();
    ~sccontext();

    /* cara lebih panjang tetapi disisi python lebih "elegan" */
    boostpy::list get_list_readers();

    connector* get_connector(boost::python::long_ idx );

    inline SCARDCONTEXT get_handler(){ return m_ctxhandle; }
    

private:
    std::vector<connector*> m_connectorlist;
    SCARDCONTEXT	m_ctxhandle;

//    static sccontext s_sccontext;
};



