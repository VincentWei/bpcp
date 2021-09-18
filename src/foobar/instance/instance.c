/*
 * @file instance.c
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/18
 * @brief The instance of FooBar.
 *
 * Copyright (C) 2021 FMSoft <https://www.fmsoft.cn>
 *
 * This file is a part of FooBar (short for Purring Cat), an HVML interpreter.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "foobar.h"

#include "private/instance.h"
#include "private/foo.h"
#include "private/bar.h"

#include <stdlib.h>

foobar_instance_t foobar_init(const char* app_name, const char* runner_name,
        const foobar_instance_extra_info* extra_info)
{
    foobar_instance_t inst;

    UNUSED_PARAM(app_name);
    UNUSED_PARAM(runner_name);
    UNUSED_PARAM(extra_info);

    if ((inst = calloc(sizeof(struct foobar_instance), 1)) == NULL)
        goto error;

    // TODO: initialize modules here.
    fbfoo_init(inst);
    fbbar_init(inst);

error:
    return inst;
}

bool foobar_cleanup(foobar_instance_t inst)
{
    if (inst == NULL)
        return false;

    // TODO: terminate modules here.
    fbfoo_term(inst);
    fbbar_term(inst);

    return true;
}
