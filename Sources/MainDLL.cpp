/*
    SMART Card Reader Module for Python 
    Writer: Indra Bagus <indra@xirkachipset.com>
*/

#include "stdafx.h"
#include "sccontext.h"
#include "connector.h"
#include "version.h"

//using namespace boost::python;
namespace boostpy = boost::python;

static boost::scoped_ptr<sccontext> s_psccontext;

                                 
                                 
static const char* docsconnector = "The connector manager that will handle data transaction "
                                   "to the smart card or to reader itself";                                 


boostpy::str about()
{
    return boost::python::str("Xirka Smart Card Python Extension Module "
                              "by Indra Bagus <indra@xirkachipset.com>");
}

boostpy::tuple version()
{
    return boost::python::make_tuple(boost::python::long_(MAJOR_VERSION),
                                     boost::python::long_(MINOR_VERSION),
                                     boost::python::long_(RELEASE_NUMBER),
                                     boost::python::long_(BUILD_NUMBER));
}


sccontext* get_sccontext()
{
    if(s_psccontext.get() == 0)
    {
        s_psccontext.reset(new sccontext);
    }
    return s_psccontext.get();
}

BOOST_PYTHON_MODULE(_scard)
{

    boostpy::docstring_options doc_options(true, false, false);
    boostpy::def("version",&version,"Get version of this module");
    boostpy::def("about", &about, "About the writer of this module");
    boostpy::def("context", &get_sccontext, boostpy::return_value_policy<boostpy::reference_existing_object>(),
        "Get the sccontext object of Windows Smart Card API");

    /* harus ditambahkan jika ingin setiap converter stringlist(vector<std::string>) 
      ke boost::python::list berhasil */
    boostpy::class_<stringlist>("stringlist")
        .def(boostpy::vector_indexing_suite<stringlist>());
    /* converter intvect_t(vector<int>) ke boost::python::list */
    boostpy::class_<intvect_t>("intlist")
        .def(boostpy::vector_indexing_suite<intvect_t>());
    /* converter ubytevect_t (vector<ubyte_t>) ke boost::python::list */
    boostpy::class_<ubytevect_t>("bytelist")
        .def(boostpy::vector_indexing_suite<ubytevect_t>());
    boostpy::class_<READERSTATE>("READERSTATE", "READERSTATE documentation here")
        .def_readwrite("currentstate", &READERSTATE::current_state)
        .def_readwrite("eventstate", &READERSTATE::event_state);
    boostpy::class_<statelist_t>("statelist")
        .def(boostpy::vector_indexing_suite<statelist_t>());

    boostpy::class_<sccontext>("sccontext", sccontext::class_doc, boostpy::no_init)
        .def("list_readers", &sccontext::get_list_readers, 
            boostpy::return_value_policy<boostpy::return_by_value>())
        .def("connector", &sccontext::get_connector, 
            boostpy::return_value_policy<boostpy::reference_existing_object>())
        .def("connector_byname", &sccontext::get_connector_byname, 
            boostpy::return_value_policy<boostpy::reference_existing_object>())
        .def("get_status_change",&sccontext::get_status_change,
            boostpy::return_value_policy<boostpy::return_by_value>());

    boostpy::class_<connector>("connector", docsconnector, boostpy::no_init)
        .def("name", &connector::get_pythonname)
        .def("connect", &connector::connect)
        .def("direct_connect", &connector::direct_connect)
        .def("disconnect", &connector::disconnect)
        .def("reset",&connector::reset)
        .def("eject",&connector::eject)
        .def("unpowered",&connector::unpowered)
        .def("control", &connector::direct_control, 
            boostpy::return_value_policy<boostpy::return_by_value>())
        .def("transceive", &connector::transceive, 
            boostpy::return_value_policy<boostpy::return_by_value>())
        .def("atr", &connector::get_atr, 
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("channel_id", &connector::get_channelid,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("characteristics", &connector::get_characteristics,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_bwt", &connector::get_current_bwt,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_clk", &connector::get_current_clock,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_cwt", &connector::get_current_cwt,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_d", &connector::get_current_d,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_ebc_enc", &connector::get_current_ebc_enc,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_clk_conv", &connector::get_current_clk_conv,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_ifsc", &connector::get_current_ifsc,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_ifsd", &connector::get_current_ifsd,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_guardtime", &connector::get_current_guardtime,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_prototype", &connector::get_current_prototype,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("current_workwaiting", &connector::get_current_workwaiting,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("default_clkrate", &connector::get_default_clockrate,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("default_datarate", &connector::get_default_datarate,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("device_dispname", &connector::get_reader_dispname,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("device_sysname", &connector::get_reader_sysname,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("device_unit", &connector::get_reader_devunit,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_atr)
        .def("icc_interface_status", &connector::get_icc_ifacestatus,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_icc_ifacestatus)
        .def("icc_presence", &connector::get_icc_presence,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_icc_presence)
        .def("icc_type_per_atr", &connector::get_icc_type_per_atr,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_icc_type_per_atr)
        .def("max_clock", &connector::get_max_clk,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_max_clk)
        .def("max_datarate", &connector::get_max_datarate, 
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_max_datarate)
        .def("max_ifsd", &connector::get_max_ifsd,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_max_ifsd)
        .def("power_mgmt_support", &connector::get_power_mgmt_support,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_max_ifsd)
        .def("protocoltype", &connector::get_protocol_types,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_max_ifsd)
        .def("vendor_serialno", &connector::get_vendor_serialno,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_max_ifsd)
        .def("vendor_devtype", &connector::get_vendor_devicetype,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_max_ifsd)
        .def("vendor_device_ver", &connector::get_vendor_deviceversion,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_max_ifsd)
        .def("vendor_name", &connector::get_vendor_name,
            boostpy::return_value_policy<boostpy::return_by_value>(),
            connector::doc_max_ifsd)
        .def("readerstate", &connector::get_readerstate, 
            boostpy::return_value_policy<boostpy::reference_existing_object>());
    
    boostpy::scope _this;
    _this.attr("__doc__") =
        "A Xirka Smart Card Python module's extension\n"
        "used for testing various smart card test's script";

    _this.attr("__author__") = "Indra Bagus <indra.bagus@gmail.com>";
    _this.attr("__version__") = VERSION_STR();


    boostpy::scope().attr("SCARD_STATE_UNAWARE") = SCARD_STATE_UNAWARE;
    boostpy::scope().attr("SCARD_STATE_IGNORE") = SCARD_STATE_IGNORE;
    boostpy::scope().attr("SCARD_STATE_CHANGED") = SCARD_STATE_CHANGED;
    boostpy::scope().attr("SCARD_STATE_UNKNOWN") = SCARD_STATE_UNKNOWN;
    boostpy::scope().attr("SCARD_STATE_UNAVAILABLE") = SCARD_STATE_UNAVAILABLE;
    boostpy::scope().attr("SCARD_STATE_EMPTY")       = SCARD_STATE_EMPTY;
    boostpy::scope().attr("SCARD_STATE_PRESENT")     = SCARD_STATE_PRESENT;
    boostpy::scope().attr("SCARD_STATE_ATRMATCH")    = SCARD_STATE_ATRMATCH;
    boostpy::scope().attr("SCARD_STATE_EXCLUSIVE")   = SCARD_STATE_EXCLUSIVE;
    boostpy::scope().attr("SCARD_STATE_INUSE")       = SCARD_STATE_INUSE;
    boostpy::scope().attr("SCARD_STATE_MUTE")        = SCARD_STATE_MUTE;
    boostpy::scope().attr("SCARD_STATE_UNPOWERED")   = SCARD_STATE_UNPOWERED;

    /* SCARD CTL */
    boostpy::scope().attr("IOCTL_SMARTCARD_POWER")           = IOCTL_SMARTCARD_POWER;
    boostpy::scope().attr("IOCTL_SMARTCARD_GET_ATTRIBUTE")   = IOCTL_SMARTCARD_GET_ATTRIBUTE;
    boostpy::scope().attr("IOCTL_SMARTCARD_SET_ATTRIBUTE")   = IOCTL_SMARTCARD_SET_ATTRIBUTE;
    boostpy::scope().attr("IOCTL_SMARTCARD_CONFISCATE")      = IOCTL_SMARTCARD_CONFISCATE;
    boostpy::scope().attr("IOCTL_SMARTCARD_TRANSMIT")        = IOCTL_SMARTCARD_TRANSMIT;
    boostpy::scope().attr("IOCTL_SMARTCARD_EJECT")           = IOCTL_SMARTCARD_EJECT;
    boostpy::scope().attr("IOCTL_SMARTCARD_SWALLOW")         = IOCTL_SMARTCARD_SWALLOW;
    boostpy::scope().attr("IOCTL_SMARTCARD_IS_PRESENT")      = IOCTL_SMARTCARD_IS_PRESENT;
    boostpy::scope().attr("IOCTL_SMARTCARD_IS_ABSENT")       = IOCTL_SMARTCARD_IS_ABSENT;
    boostpy::scope().attr("IOCTL_SMARTCARD_SET_PROTOCOL")    = IOCTL_SMARTCARD_SET_PROTOCOL;
    boostpy::scope().attr("IOCTL_SMARTCARD_GET_STATE")       = IOCTL_SMARTCARD_GET_STATE;
    boostpy::scope().attr("IOCTL_SMARTCARD_GET_LAST_ERROR")  = IOCTL_SMARTCARD_GET_LAST_ERROR;
    boostpy::scope().attr("IOCTL_SMARTCARD_GET_PERF_CNTR")   = IOCTL_SMARTCARD_GET_PERF_CNTR;
    /* IOCTL ACR122*/
    boostpy::scope().attr("IOCTL_CCID_ESCAPE_SCARD_CTL_CODE") = IOCTL_CCID_ESCAPE_SCARD_CTL_CODE;
    boostpy::scope().attr("IOCTL_SMARTCARD_ACR128_ESCAPE_COMMAND") = IOCTL_SMARTCARD_ACR128_ESCAPE_COMMAND;
}

