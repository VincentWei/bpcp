/*
 * @file foo.c
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/18
 * @brief The implementation of foo module.
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

#include "foobar.h"

#include "private/instance.h"
#include "private/foo.h"

#include "context.h"

#include <stdlib.h>

bool fbfoo_init(foobar_instance_t inst)
{
    UNUSED_PARAM(inst);

    inst->foo_ctxt = calloc(sizeof(struct fbfoo_context), 1);
    if (inst->foo_ctxt)
        return true;

    return false;
}

bool fbfoo_term(foobar_instance_t inst)
{
    if (inst == NULL || inst->foo_ctxt == NULL)
        return false;

    free(inst->foo_ctxt);
    inst->foo_ctxt = NULL;
    return true;
}

