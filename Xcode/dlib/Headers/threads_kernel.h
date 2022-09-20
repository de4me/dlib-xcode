// Copyright (C) 2006  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_THREADs_KERNEL_
#define DLIB_THREADs_KERNEL_

#include <dlib/platform.h>

#ifdef WIN32
#include <dlib/threads_windows.h>
#endif

#ifndef WIN32
#include <dlib/threads_posix.h>
#endif

#endif // DLIB_THREADs_KERNEL_


