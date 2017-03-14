#ifndef STDAFX_H
#define STDAFX_H

#pragma once
/* we use static library */
#ifndef BOOST_PYTHON_STATIC_LIB
#define BOOST_PYTHON_STATIC_LIB
#endif

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/smart_ptr.hpp>
#include <vector>
#include <WinSCard.h>

namespace boostpy = boost::python;


#endif