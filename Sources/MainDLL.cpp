/*
    SMART Card Reader Module for Python 
    Writer: Indra Bagus <indra@xirkachipset.com>
*/

#include "xstsmartcard.h"

using namespace boost::python;
#define MAJOR_VERSION   2
#define MINOR_VERSION   2
#define RELEASE_NUMBER  4
#define BUILD_NUMBER    1

static boost::scoped_ptr<sccontext> s_pcontext;

static const char* docscontext = "object representasi dari smart card context";

boost::python::str about()
{
    return boost::python::str("Smart Card Python Module by Indra Bagus <indra.bagus@gmail.com> ");
}

boost::python::tuple version()
{
    return boost::python::make_tuple(boost::python::long_(MAJOR_VERSION),
                                     boost::python::long_(MINOR_VERSION),
                                     boost::python::long_(RELEASE_NUMBER),
                                     boost::python::long_(BUILD_NUMBER));
}


sccontext* get_context()
{
    if(s_pcontext.get() == 0)
    {
        s_pcontext.reset(new sccontext);
    }
    return s_pcontext.get();
}


BOOST_PYTHON_MODULE(scard)
{

    def("version",&version);
    def("about",&about);
    /* harus ditambahkan jika ingin setiap converter vector<std::string>->boost::python::list berhasil */
    class_<stringlist>("stringlist")
        .def(vector_indexing_suite<stringlist>());
    /* idem dengan atas */
    class_<intvect_t>("intlist")
        .def(vector_indexing_suite<intvect_t>());

    class_<ubytevect_t>("bytelist")
        .def(vector_indexing_suite<ubytevect_t>());

    def("context",&get_context,return_value_policy<reference_existing_object>());
    class_<sccontext>("sccontext",docscontext,boost::python::no_init)
        .def("list_readers",&sccontext::get_list_readers,return_value_policy<return_by_value>())
        .def("connector",&sccontext::get_connector,return_value_policy<reference_existing_object>());

    class_<connector>("connector","object representasi smart card connector",boost::python::no_init)
        .def("event",&connector::get_current_event)
        .def("name", &connector::get_pythonname)
        .def("connect",&connector::connect)
        .def("direct_connect",&connector::direct_connect)
        .def("disconnect",&connector::disconnect)
        .def("control", &connector::direct_control, return_value_policy<return_by_value>())
        .def("transceive",&connector::transceive,return_value_policy<return_by_value>());

    scope().attr("SCARD_STATE_UNAWARE")     = SCARD_STATE_UNAWARE;
    scope().attr("SCARD_STATE_IGNORE")      = SCARD_STATE_IGNORE;
    scope().attr("SCARD_STATE_CHANGED")     = SCARD_STATE_CHANGED;
    scope().attr("SCARD_STATE_UNKNOWN")     = SCARD_STATE_UNKNOWN;
    scope().attr("SCARD_STATE_UNAVAILABLE") = SCARD_STATE_UNAVAILABLE;
    scope().attr("SCARD_STATE_EMPTY")       = SCARD_STATE_EMPTY;
    scope().attr("SCARD_STATE_PRESENT")     = SCARD_STATE_PRESENT;
    scope().attr("SCARD_STATE_ATRMATCH")    = SCARD_STATE_ATRMATCH;
    scope().attr("SCARD_STATE_EXCLUSIVE")   = SCARD_STATE_EXCLUSIVE;
    scope().attr("SCARD_STATE_INUSE")       = SCARD_STATE_INUSE;
    scope().attr("SCARD_STATE_MUTE")        = SCARD_STATE_MUTE;
    scope().attr("SCARD_STATE_UNPOWERED")   = SCARD_STATE_UNPOWERED;

    /* SCARD CTL */
    scope().attr("IOCTL_SMARTCARD_POWER")           = IOCTL_SMARTCARD_POWER;
    scope().attr("IOCTL_SMARTCARD_GET_ATTRIBUTE")   = IOCTL_SMARTCARD_GET_ATTRIBUTE;
    scope().attr("IOCTL_SMARTCARD_SET_ATTRIBUTE")   = IOCTL_SMARTCARD_SET_ATTRIBUTE;
    scope().attr("IOCTL_SMARTCARD_CONFISCATE")      = IOCTL_SMARTCARD_CONFISCATE;
    scope().attr("IOCTL_SMARTCARD_TRANSMIT")        = IOCTL_SMARTCARD_TRANSMIT;
    scope().attr("IOCTL_SMARTCARD_EJECT")           = IOCTL_SMARTCARD_EJECT;
    scope().attr("IOCTL_SMARTCARD_SWALLOW")         = IOCTL_SMARTCARD_SWALLOW;
    scope().attr("IOCTL_SMARTCARD_IS_PRESENT")      = IOCTL_SMARTCARD_IS_PRESENT;
    scope().attr("IOCTL_SMARTCARD_IS_ABSENT")       = IOCTL_SMARTCARD_IS_ABSENT;
    scope().attr("IOCTL_SMARTCARD_SET_PROTOCOL")    = IOCTL_SMARTCARD_SET_PROTOCOL;
    scope().attr("IOCTL_SMARTCARD_GET_STATE")       = IOCTL_SMARTCARD_GET_STATE;
    scope().attr("IOCTL_SMARTCARD_GET_LAST_ERROR")  = IOCTL_SMARTCARD_GET_LAST_ERROR;
    scope().attr("IOCTL_SMARTCARD_GET_PERF_CNTR")   = IOCTL_SMARTCARD_GET_PERF_CNTR;
    /* IOCTL ACR122*/
    scope().attr("IOCTL_CCID_ESCAPE_SCARD_CTL_CODE")    = IOCTL_CCID_ESCAPE_SCARD_CTL_CODE;
    scope().attr("IOCTL_SMARTCARD_ACR128_ESCAPE_COMMAND") = IOCTL_SMARTCARD_ACR128_ESCAPE_COMMAND;
}

