// Copyright (C) 2003  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_CONFIG_READEr_
#define DLIB_CONFIG_READEr_

#include <dlib/config_reader_kernel_1.h>
#include <dlib/map.h>
#include <dlib/tokenizer.h>
#include <dlib/get_option.h>

#include <dlib/algs.h>
#include <dlib/is_kind.h>


namespace dlib
{

    typedef config_reader_kernel_1<
        map<std::string,std::string>::kernel_1b,
        map<std::string,void*>::kernel_1b,
        tokenizer::kernel_1a
        > config_reader;

    template <> struct is_config_reader<config_reader> { const static bool value = true; };

#ifndef DLIB_ISO_CPP_ONLY
    typedef config_reader_thread_safe_1<
        config_reader,
        map<std::string,void*>::kernel_1b
        > config_reader_thread_safe;

    template <> struct is_config_reader<config_reader_thread_safe> { const static bool value = true; };
#endif // DLIB_ISO_CPP_ONLY


}

#endif // DLIB_CONFIG_READEr_

