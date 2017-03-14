#include "stdafx.h"
#include "sccontext.h"
#include "scexception.h"
/*
========================================================================================================
sccontext Class Implementation
========================================================================================================
*/

const char* sccontext::class_doc = "The smart card sccontext management that mostly used "
"to create connector and readers enumeration";

SCARD_READERSTATE sccontext::s_readerstate[MAXIMUM_SMARTCARD_READERS];

sccontext::sccontext()
{
    long ret = ::SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_ctxhandle);

    if (ret != SCARD_S_SUCCESS)
        scexception::throw_systemerror("Failed to established smart card context", ret);

    LPTSTR prespbuffer;
    LPTSTR szreader;
    DWORD resplength = SCARD_AUTOALLOCATE;
    ret = ::SCardListReaders(m_ctxhandle, 0, (LPTSTR)&prespbuffer, &resplength);

    /* I'll let python user know that list reader will returned empy list */
    if (ret != SCARD_S_SUCCESS) {
        return;
    }


    szreader = prespbuffer;
    while (*szreader != '\0')
    {
        std::string readername(szreader);
        m_connectorlist.push_back(new connector(readername, this));
        szreader = szreader + strlen((char*)szreader) + 1;
    }
    ::SCardFreeMemory(m_ctxhandle, prespbuffer);

    // check the initial status/event
    for (std::size_t i = 0; i < m_connectorlist.size(); ++i)
    {
        s_readerstate[i].szReader = (LPCTSTR)m_connectorlist[i]->get_name().c_str();
        s_readerstate[i].dwCurrentState = SCARD_STATE_UNAWARE;
        s_readerstate[i].dwEventState = SCARD_STATE_UNKNOWN;
    }
    ret = SCardGetStatusChange(this->m_ctxhandle, INFINITE, s_readerstate, m_connectorlist.size());

}


sccontext::~sccontext()
{
    std::vector<connector*>::iterator itercon = m_connectorlist.begin();
    while (itercon != m_connectorlist.end())
    {
        delete *itercon;
        ++itercon;
    }
    ::SCardReleaseContext(m_ctxhandle);
}

connector* sccontext::get_connector(boostpy::long_ idx)
{
    if (m_connectorlist.empty()) {
        scexception::throw_systemerror("No connector detected");
    }

    size_t val = ::PyLong_AsSize_t(idx.ptr());
    if (val >= m_connectorlist.size())
    {
        boostpy::throw_error_already_set();
    }
    return m_connectorlist[val];
}

connector* sccontext::get_connector_byname(boostpy::str szname)
{
    std::string stdstrname = boostpy::extract<const char*>(szname);
    connectorlist_t::iterator iter = m_connectorlist.begin();
    while (iter != m_connectorlist.end())
    {
        if ((*iter)->get_name() == stdstrname)
            return (*iter);
    }
    scexception::throw_systemerror("Connector's name unrecognize");
}


boostpy::list sccontext::get_list_readers()
{
    std::vector<std::string> strlist;
    std::vector<connector*>::iterator itercon = m_connectorlist.begin();
    while (itercon != m_connectorlist.end())
    {
        strlist.push_back((*itercon)->get_name());
        ++itercon;
    }
    boostpy::object get_iter = boostpy::iterator<std::vector<std::string>>();
    boostpy::object iter = get_iter(strlist);
    return boostpy::list(iter);
}

boostpy::list sccontext::get_status_change(boostpy::long_ tmout)
{
    statelist_t lists;
    //DWORD dwStateMask = ~(SCARD_STATE_INUSE |
    //    SCARD_STATE_EXCLUSIVE |
    //    SCARD_STATE_UNAWARE |
    //    SCARD_STATE_IGNORE |
    //    SCARD_STATE_CHANGED);

    for (int i = 0; i < m_connectorlist.size(); ++i) {
        s_readerstate[i].dwCurrentState = s_readerstate[i].dwEventState;
    }

    DWORD tmval = boostpy::extract<DWORD>(tmout);
    LONG lret = ::SCardGetStatusChange(this->m_ctxhandle,
        INFINITE,
        s_readerstate,
        m_connectorlist.size());
    if (lret != SCARD_S_SUCCESS)
    {
        scexception::throw_systemerror("Error to get status change", lret);
    }

    for (std::size_t i = 0; i < m_connectorlist.size(); ++i)
    {
        READERSTATE state;
        state.current_state = s_readerstate[i].dwCurrentState;
        state.event_state = s_readerstate[i].dwEventState;
        lists.push_back(state);
    }
    boostpy::object get_iter = boostpy::iterator<statelist_t>();
    boostpy::object iter = get_iter(lists);
    return boostpy::list(iter);
}
