#ifndef CONNECTOR_H
#define CONNECTOR_H

#pragma once


#define RXBUFFERSIZE    260
#define IOCTL_CCID_ESCAPE_SCARD_CTL_CODE        SCARD_CTL_CODE(3500)
#define IOCTL_SMARTCARD_ACR128_ESCAPE_COMMAND   SCARD_CTL_CODE(2079)

struct READERSTATE
{
    unsigned int current_state;
    unsigned int event_state;

    bool operator == (const struct READERSTATE& rhs)
    {
        if ((current_state == rhs.current_state) && (event_state == rhs.event_state))
            return true;

        return false;
    }
};

typedef unsigned char ubyte_t;
typedef std::vector<std::string> stringlist;
typedef std::vector<int> intvect_t;
typedef std::vector<ubyte_t> ubytevect_t;
typedef std::vector<struct READERSTATE> statelist_t;

class sccontext;


class connector
{
public:
    static const char* doc_icc_ifacestatus;
    static const char* doc_icc_presence;
    static const char* doc_icc_type_per_atr;
    static const char* doc_atr;
    static const char* doc_max_datarate;
    static const char* doc_max_clk;
    static const char* doc_max_ifsd;
public:
    connector() :m_handle(NULL) {
        /* ctor */
    }

    explicit connector(std::string name, sccontext* pctx) :m_handle(NULL)
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
            m_handle = NULL;
        }

    }

    void connect();
    void direct_connect();
    void disconnect();
    void reset();
    void unpowered();
    void eject();
    READERSTATE* get_readerstate();
    boostpy::object transceive(boostpy::object const& ob);
    boostpy::long_ get_transmit_count();
    boostpy::object direct_control(boostpy::long_ ctl, boostpy::object const& ob);
    // Inline section
    inline boostpy::object get_atr() {
        return get_attribute(SCARD_ATTR_ATR_STRING, "y#");
    }

    inline boostpy::object get_channelid() {
        return get_int_attribute(SCARD_ATTR_CHANNEL_ID, 4);
    }

    inline boostpy::object get_characteristics() {
        return get_int_attribute(SCARD_ATTR_CHARACTERISTICS, 4);
    }

    inline boostpy::object get_current_bwt() {
        return get_int_attribute(SCARD_ATTR_CURRENT_BWT, 4);
    }

    inline boostpy::object get_current_clock() {
        return get_int_attribute(SCARD_ATTR_CURRENT_CLK, 4);
    }

    inline boostpy::object get_current_cwt() {
        return get_int_attribute(SCARD_ATTR_CURRENT_CWT, 4);
    }

    inline boostpy::object get_current_d() {
        return get_int_attribute(SCARD_ATTR_CURRENT_D, 4);
    }
    inline boostpy::object get_max_clk() {
        return get_int_attribute(SCARD_ATTR_MAX_CLK, 4);
    }


    inline boostpy::object get_max_datarate()
    {
        return get_int_attribute(SCARD_ATTR_MAX_DATA_RATE, 2);
    }

    inline boostpy::object get_max_ifsd() {
        return get_int_attribute(SCARD_ATTR_MAX_IFSD, 2);
    }

    inline boostpy::object get_icc_type_per_atr() {
        return get_int_attribute(SCARD_ATTR_ICC_TYPE_PER_ATR, 1);
    }

    inline boostpy::object get_icc_presence() {
        return get_int_attribute(SCARD_ATTR_ICC_PRESENCE, 1);
    }

    inline boostpy::object get_icc_ifacestatus() {
        return get_int_attribute(SCARD_ATTR_ICC_INTERFACE_STATUS, 1);
    }

    inline void set_name(std::string szname) {
        m_szname = szname;
    }

    inline std::string& get_name(void) {
        return m_szname;
    }

    inline boostpy::str get_pythonname(void) {
        return boostpy::str(m_szname.c_str());
    }
private:
    boostpy::object get_attribute(DWORD attrid, LPCTSTR strpytype);
    boostpy::object get_int_attribute(DWORD attrid,int explen);
    std::string m_szname;
    /* sccontext handle */
    SCARDHANDLE m_handle;
    sccontext* m_psccontext;
    DWORD m_prototype;
    SCARD_IO_REQUEST m_io_request;
    ubyte_t m_rxbuffer[RXBUFFERSIZE];
    READERSTATE m_readerstate;
};


#endif