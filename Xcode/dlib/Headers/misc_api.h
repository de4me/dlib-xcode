// Copyright (C) 2004  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.

#ifndef DLIB_MISC_APi_
#define DLIB_MISC_APi_

#include <dlib/platform.h>

#ifdef WIN32
#include <dlib/misc_api_windows.h>
#endif

#ifndef WIN32
#include <dlib/misc_api_posix.h>
#endif

#include <dlib/misc_api_shared.h>

#endif // DLIB_MISC_APi_

