/*
 * @file utils.h
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/18
 * @brief The internal utility interfaces.
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

#ifndef UTILS_H
#define UTILS_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX(x,y) ((x > y) ? x : y)

#if !HAVE(VASPRINTF)
int vasprintf(char **buf, const char *fmt, va_list ap)
# if COMPILER(GCC)
    __attribute__ ((format (gnu_printf, 2, 0)))
# endif
;
#endif

#ifdef __cplusplus
}
#endif

#endif /* not defined UTILS_H */

