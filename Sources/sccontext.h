#ifndef CONTEXT_H
#define CONTEXT_H
#pragma once


#include "connector.h"
class sccontext
{
    typedef std::vector<connector*> connectorlist_t;
public:
    static const char* class_doc;

public:
    sccontext();
    ~sccontext();

    /* cara lebih panjang tetapi disisi python lebih "elegan" */
    boostpy::list get_list_readers();
    connector* get_connector(boostpy::long_ idx);
    connector* get_connector_byname(boostpy::str szname);
    inline SCARDCONTEXT get_handler() { return m_ctxhandle; }
    boostpy::list get_status_change(boostpy::long_ tmout);

private:
    std::vector<connector*> m_connectorlist;
    SCARDCONTEXT	m_ctxhandle;
    static SCARD_READERSTATE s_readerstate[MAXIMUM_SMARTCARD_READERS];

    //    static sccontext s_sccontext;
};


#endif
