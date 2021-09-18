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

foobar_instance_t foobar_init(const char* app_name, const char* runner_name,
        const foobar_instance_extra_info* extra_info)
{
    UNUSED_PARAM(app_name);
    UNUSED_PARAM(runner_name);
    UNUSED_PARAM(extra_info);

    // TODO: initialize modules here.
    fbfoo_init();
    fbbar_init();

    return NULL;
}

bool foobar_cleanup(foobar_instance_t inst)
{
    if (inst == NULL)
        return false;

    // TODO: terminate modules here.
    fbfoo_term();
    fbbar_term();
    return true;
}
