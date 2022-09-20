// Copyright (C) 2003  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_DIR_NAv_
#define DLIB_DIR_NAv_


#include <dlib/platform.h>


#ifdef WIN32
#include <dlib/dir_nav_windows.h>
#endif

#ifndef WIN32
#include <dlib/dir_nav_posix.h>
#endif

#include <dlib/dir_nav_extensions.h>

#endif // DLIB_DIR_NAv_

