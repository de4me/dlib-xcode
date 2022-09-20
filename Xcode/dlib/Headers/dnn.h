// Copyright (C) 2015  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_DNn_
#define DLIB_DNn_

// DNN module uses template-based network declaration that leads to very long
// type names. Visual Studio will produce Warning C4503 in such cases
#ifdef _MSC_VER
#   pragma warning( disable: 4503 )
#endif

#include <dlib/tensor.h>
#include <dlib/input.h>

// Problem:    Visual Studio's vcpkgsrv.exe constantly uses a single CPU core,
//             apparently never finishing whatever it's trying to do. Moreover,
//             this issue prevents some operations like switching from Debug to
//             Release (and vice versa) in the IDE. (Your mileage may vary.)
// Workaround: Keep manually killing the vcpkgsrv.exe process.
// Solution:   Disable IntelliSense for some files. Which files? Unfortunately
//             this seems to be a trial-and-error process.
#ifndef __INTELLISENSE__
#include <dlib/layers.h>
#endif // __INTELLISENSE__

#include <dlib/loss.h>
#include <dlib/core.h>
#include <dlib/solvers.h>
#include <dlib/trainer.h>
#include <dlib/cpu_dlib.h>
#include <dlib/tensor_tools.h>
#include <dlib/utilities.h>
#include <dlib/validation.h>
#include <dlib/visitors.h>

#endif // DLIB_DNn_


