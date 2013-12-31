#define BOOST_PYTHON_STATIC_LIB
//
#include <boost/python.hpp>
using namespace boost::python;
struct context
{
    context(){}
    const char* about(){return "Hello this is Smart Card Context";} 
};


context* createcontext(){ return new context();}
void destroycontext(context* ctx){ delete ctx; }

BOOST_PYTHON_MODULE(scard)
{
    def("createcontext",createcontext);
    def("destroycontext",destroycontext);
    class_<context>("context").def("about",&context::about);
}

