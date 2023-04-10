/**
 * @file foobar.h
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/18
 * @brief The main header file of FooBar.
 *
 * Copyright (C) 2021 FMSoft <https://www.fmsoft.cn>
 *
 * This file is a part of FooBar, which contains the examples of my course:
 * _the Best Practices of C Language_.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef FOOBAR_FOOBAR_H
#define FOOBAR_FOOBAR_H

#include "foobar_macros.h"
#include "foobar_version.h"
#include "foobar_features.h"

struct foobar_instance;
typedef struct foobar_instance *foobar_instance_t;

typedef struct foobar_instance_extra_info {
    int foo;
    int bar;
} foobar_instance_extra_info;

FOOBAR_EXTERN_C_BEGIN

/**
 * foobar_init:
 *
 * @app_name: a pointer to the string contains the app name.
 *      If this argument is null, the executable program name of the command
 *      line will be used for the app name.
 * @runner_name: a pointer to the string contains the runner name.
 *      If this argument is null, `unknown` will be used for the runner name.
 * @extra_info: a pointer (nullable) to the extra information for
 *      the new FooBar instance.
 *
 * Initializes a new FooBar instance for the current thread.
 *
 * Returns: A new FooBar instance; NULL on error.
 *
 * Since 0.0.1
 */
FOOBAR_EXPORT foobar_instance_t foobar_init(const char* app_name,
        const char* runner_name,
        const foobar_instance_extra_info* extra_info);

/**
 * foobar_term:
 *
 * Cleans up the FooBar instance attached to the current thread.
 *
 * Returns: @true for success; @false for bad FooBar instance.
 *
 * Since 0.0.1
 */
FOOBAR_EXPORT bool foobar_term(foobar_instance_t inst);

FOOBAR_EXTERN_C_END

#endif /* not defined FOOBAR_FOOBAR_H */
