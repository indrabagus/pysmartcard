#define BOOST_PYTHON_STATIC_LIB
//
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>

using namespace boost::python;
typedef std::vector<std::string> stringlist;



struct connector
{
    connector(){}
    explicit connector(std::string name)
    {
        m_szname = name;
    }
    
    int get_status_change()
    {
        return 0;
    }

    unsigned short transceive(object const& ob)
    {
        stl_input_iterator<unsigned char> begin(ob), end;
        std::vector<unsigned char> vs(begin,end);
        return vs.size();
    }

private:
    std::string m_szname;

};


struct context
{
    context(){}
    stringlist enumreaders()
    {
        stringlist retval;
        retval.push_back(std::string("BOOST"));
        retval.push_back(std::string("PYTHON"));
        return retval;
    }
    /* cara lebih panjang tetapi disisi python lebih "elegan" */
    boost::python::list get_list_readers()
    {
        std::vector<std::string> strlist;
        strlist.push_back(std::string("CPP"));
        strlist.push_back(std::string("BOOST"));
        strlist.push_back(std::string("PYTHON"));
        boost::python::object get_iter=boost::python::iterator<std::vector<std::string>>();
        boost::python::object iter = get_iter(strlist);
        return boost::python::list(iter);
    }

    connector* get_connector(boost::python::str name)
    {
        return new connector(std::string(extract<const char*>(name)));
    }

    const char* about(){
        return "Hello this is Smart Card Context";
    } 
};



context* createcontext(){ return new context();}
void destroycontext(context* ctx){ delete ctx; }

BOOST_PYTHON_MODULE(scard)
{
    class_<stringlist>("stringlist")
        .def(vector_indexing_suite<stringlist>());

    def("createcontext",createcontext,return_value_policy<manage_new_object>());
    class_<context>("context")
        .def("get_list_readers",&context::get_list_readers,return_value_policy<return_by_value>())
        .def("enumreaders",&context::enumreaders)
        .def("about",&context::about);

    class_<connector>("connector")
        .def("get_status_change",&connector::get_status_change)
        .def("transceive",&connector::transceive);
}

