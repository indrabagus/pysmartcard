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

    inline boostpy::object get_current_ebc_enc() {
        return get_int_attribute(SCARD_ATTR_CURRENT_EBC_ENCODING, 4);
    }

    inline boostpy::object get_current_clk_conv() {
        return get_int_attribute(SCARD_ATTR_CURRENT_F, 4);
    }

    inline boostpy::object get_current_ifsc() {
        return get_int_attribute(SCARD_ATTR_CURRENT_IFSC, 4);
    }

    inline boostpy::object get_current_ifsd() {
        return get_int_attribute(SCARD_ATTR_CURRENT_IFSD, 4);
    }

    inline boostpy::object get_current_guardtime() {
        return get_int_attribute(SCARD_ATTR_CURRENT_N, 4);
    }

    inline boostpy::object get_current_prototype() {
        return get_int_attribute(SCARD_ATTR_CURRENT_PROTOCOL_TYPE, 4);
    }

    inline boostpy::object get_current_workwaiting() {
        return get_int_attribute(SCARD_ATTR_CURRENT_W, 4);
    }

    inline boostpy::object get_default_clockrate() {
        return get_int_attribute(SCARD_ATTR_DEFAULT_CLK, 4);
    }

    inline boostpy::object get_default_datarate() {
        return get_int_attribute(SCARD_ATTR_DEFAULT_DATA_RATE, 4);
    }

    inline boostpy::object get_reader_dispname() {
        return get_attribute(SCARD_ATTR_DEVICE_FRIENDLY_NAME, "z#");
    }

    inline boostpy::object get_reader_sysname() {
        return get_attribute(SCARD_ATTR_DEVICE_SYSTEM_NAME, "z#");
    }

    inline boostpy::object get_reader_devunit() {
        return get_attribute(SCARD_ATTR_DEVICE_UNIT, "z#");
    }

    inline boostpy::object get_icc_ifacestatus() {
        return get_int_attribute(SCARD_ATTR_ICC_INTERFACE_STATUS, 1);
    }

    inline boostpy::object get_icc_presence() {
        return get_int_attribute(SCARD_ATTR_ICC_PRESENCE, 1);
    }

    inline boostpy::object get_icc_type_per_atr() {
        return get_int_attribute(SCARD_ATTR_ICC_TYPE_PER_ATR, 1);
    }

    inline boostpy::object get_max_clk() {
        return get_int_attribute(SCARD_ATTR_MAX_CLK, 4);
    }

    inline boostpy::object get_max_datarate()
    {
        return get_int_attribute(SCARD_ATTR_MAX_DATA_RATE, 4);
    }

    inline boostpy::object get_max_ifsd() {
        return get_int_attribute(SCARD_ATTR_MAX_IFSD, 2);
    }

    inline boostpy::object get_power_mgmt_support() {
        return get_int_attribute(SCARD_ATTR_POWER_MGMT_SUPPORT, 1);
    }

    inline boostpy::object get_protocol_types() {
        return get_int_attribute(SCARD_ATTR_PROTOCOL_TYPES, 4);
    }

    inline boostpy::object get_vendor_serialno() {
        return get_attribute(SCARD_ATTR_VENDOR_IFD_SERIAL_NO, "y#");
    }

    inline boostpy::object get_vendor_devicetype() {
        return get_attribute(SCARD_ATTR_VENDOR_IFD_TYPE, "y#");
    }

    inline boostpy::object get_vendor_deviceversion() {
        return get_int_attribute(SCARD_ATTR_VENDOR_IFD_TYPE, 4);
    }

    inline boostpy::object get_vendor_name() {
        return get_attribute(SCARD_ATTR_VENDOR_NAME, "y#");
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