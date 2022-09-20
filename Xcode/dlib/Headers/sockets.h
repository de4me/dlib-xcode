// Copyright (C) 2003  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_SOCKETs_
#define DLIB_SOCKETs_

#include <dlib/platform.h>


#ifdef WIN32
#include <dlib/sockets_windows.h>
#endif

#ifndef WIN32
#include <dlib/sockets_posix.h>
#endif

#include <dlib/sockets_extensions.h>

#endif // DLIB_SOCKETs_

