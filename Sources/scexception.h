/*
 * Simple Python module to access SmartCard Reader
 * Written by Indra Bagus <indra.bagus@gmail.com>
 */

#pragma once

//namespace boostpy = boost::python;
//
//#define RXBUFFERSIZE    260
//#define IOCTL_CCID_ESCAPE_SCARD_CTL_CODE        SCARD_CTL_CODE(3500)
//#define IOCTL_SMARTCARD_ACR128_ESCAPE_COMMAND   SCARD_CTL_CODE(2079)
//
//struct READERSTATE
//{
//    unsigned int current_state;
//    unsigned int event_state;
//
//    bool operator == (const struct READERSTATE& rhs)
//    {
//        if ((current_state == rhs.current_state) && (event_state == rhs.event_state))
//            return true;
//        
//        return false;
//    }
//};
//
//typedef unsigned char ubyte_t;
//typedef std::vector<std::string> stringlist;
//typedef std::vector<int> intvect_t;
//typedef std::vector<ubyte_t> ubytevect_t;
//typedef std::vector<struct READERSTATE> statelist_t;
//
//class sccontext;
//
//
//class connector
//{
//public:
//    connector():m_handle(NULL) { 
//        /* ctor */ 
//    }
//    
//    explicit connector(std::string name,sccontext* pctx):m_handle(NULL)
//    {
//        m_szname = name;
//        m_psccontext = pctx;
//    }
//
//    ~connector()
//    {
//        if (m_handle != NULL)
//        {
//            /* Close handle */
//            this->disconnect();
//            m_handle=NULL;
//        }
//
//    }
//
//    void connect();
//    void direct_connect();
//    void disconnect();
//    void reset();
//    void unpowered();
//    void eject();
//
//
//    READERSTATE* get_readerstate();
//    boostpy::object transceive(boostpy::object const& ob);
//    boostpy::long_ get_transmit_count();
//    boostpy::object direct_control(boostpy::long_ ctl, boostpy::object const& ob);
//    // Inline section
//    inline boostpy::object get_max_clk(){
//        return get_attribute(SCARD_ATTR_MAX_CLK, "i");
//    }
//
//
//    inline boostpy::object get_max_datarate()
//    {
//        return get_attribute(SCARD_ATTR_MAX_DATA_RATE, "i");
//    }
//
//    inline boostpy::object get_atr() {
//        return get_attribute(SCARD_ATTR_ATR_STRING, "y#");
//    }
//
//    inline boostpy::object get_max_ifsd(){
//        return get_attribute(SCARD_ATTR_MAX_IFSD,"i");
//    }
//
//
//    inline void set_name(std::string szname){ 
//        m_szname = szname; 
//    }
//    
//    inline std::string& get_name(void){ 
//        return m_szname; 
//    }
//    
//    inline boostpy::str get_pythonname(void) {
//        return boostpy::str(m_szname.c_str()); 
//    }
//private:
//    boostpy::object get_attribute(DWORD attrid,LPCTSTR strpytype);
//    std::string m_szname;
//    /* sccontext handle */
//    SCARDHANDLE m_handle;
//    sccontext* m_psccontext;
//    DWORD m_prototype;
//    SCARD_IO_REQUEST m_io_request;
//    ubyte_t m_rxbuffer[RXBUFFERSIZE];
//    READERSTATE m_readerstate;
//};

//#include "connector.h"
//class sccontext
//{
//    typedef std::vector<connector*> connectorlist_t;
//public:
//    static const char* class_doc;
//
//public:
//    sccontext();
//    ~sccontext();
//
//    /* cara lebih panjang tetapi disisi python lebih "elegan" */
//    boostpy::list get_list_readers();
//    connector* get_connector(boostpy::long_ idx);
//    connector* get_connector_byname(boostpy::str szname);
//    inline SCARDCONTEXT get_handler(){ return m_ctxhandle; }
//    boostpy::list get_status_change(boostpy::long_ tmout);
//
//private:
//    std::vector<connector*> m_connectorlist;
//    SCARDCONTEXT	m_ctxhandle;
//    static SCARD_READERSTATE s_readerstate[MAXIMUM_SMARTCARD_READERS];
//
////    static sccontext s_sccontext;
//};

class scexception
{
public:
    static void throw_systemerror(const char* message, LONG errorcode);
    static void throw_systemerror(const char* message);
};

