/*
*libarary fails to build, with this and ALL_GUI included
*I temporarily modified both files to allow compilation of the dlib library
*/


// Copyright (C) 2006  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_ALL_CONSOLe_
#define DLIB_ALL_CONSOLe_
#define DLIB_ALL_GUi_ //define the other included files
#include "all/source.cpp" //quick fix
//#error "This file has been replaced.  Instead you should add dlib/all/source.cpp to your project"

#endif // DLIB_ALL_CONSOLe_

