
//
#include "xstsmartcard.h"

using namespace boost::python;




boost::python::str about()
{
    return boost::python::str("Smart Card Python Module ( Writer: Indra Bagus <indra@xirkachipset.com> )");
}


BOOST_PYTHON_MODULE(scard)
{
    enum_<returnvalue>("RETURNVALUE")
        .value("SUCCESS",SUCCESS)
        .value("INTERNAL_ERROR",INTERNAL_ERROR)
        .value("CANCELLED",CANCELLED);


    def("about",&about);
    /* harus ditambahkan jika ingin setiap converter vector<std::string>->boost::python::list berhasil */
    class_<stringlist>("stringlist")
        .def(vector_indexing_suite<stringlist>());
    /* idem dengan atas */
    class_<intvect_t>("intlist")
        .def(vector_indexing_suite<intvect_t>());

    class_<ubytevect_t>("bytelist")
        .def(vector_indexing_suite<ubytevect_t>());

    def("context",&context::get_context,return_value_policy<reference_existing_object>());
    class_<context>("context")
        .def("get_list_readers",&context::get_list_readers,return_value_policy<return_by_value>())
        .def("get_connector",&context::get_connector,return_value_policy<reference_existing_object>());

    class_<connector>("connector")
        .def("get_status_change",&connector::get_status_change)
        .def("get_name",&connector::get_pythonstring)
        .def("transceive",&connector::transceive,return_value_policy<return_by_value>());
}

