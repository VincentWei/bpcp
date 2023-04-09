/**
 * @file purc-version.h
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/07/02
 * @brief The version of PurC.
 *
 * Copyright (C) 2021 FMSoft <https://www.fmsoft.cn>
 *
 * This file is a part of PurC (short for Purring Cat), an HVML interpreter.
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

#ifndef PURC_PURC_VERSION_H
#define PURC_PURC_VERSION_H

#include "purc-macros.h"

/**
 * PURC_VERSION_MAJOR:
 *
 * Like purc_get_major_version(), but from the headers used at
 * application compile time, rather than from the library linked
 * against at application run time.
 *
 * Since 0.0.1
 */
#define PURC_VERSION_MAJOR 0

/**
 * PURC_VERSION_MINOR:
 *
 * Like purc_get_minor_version(), but from the headers used at
 * application compile time, rather than from the library linked
 * against at application run time.
 *
 * Since 0.0.1
 */
#define PURC_VERSION_MINOR 9

/**
 * PURC_VERSION_MICRO:
 *
 * Like purc_get_micro_version(), but from the headers used at
 * application compile time, rather than from the library linked
 * against at application run time.
 *
 * Since 0.0.1
 */
#define PURC_VERSION_MICRO 8

#define PURC_VERSION_STRING "0.9.8"

#define PURC_API_VERSION_STRING "0.9"

/**
 * PURC_CHECK_VERSION:
 *
 * @major: major version (e.g. 1 for version 1.2.5)
 * @minor: minor version (e.g. 2 for version 1.2.5)
 * @micro: micro version (e.g. 5 for version 1.2.5)
 *
 * Returns: %TRUE if the version of the PurC header files
 * is the same as or newer than the passed-in version.
 *
 * Since 0.0.1
 */
#define PURC_CHECK_VERSION(major, minor, micro) \
    (PURC_VERSION_MAJOR > (major) || \
    (PURC_VERSION_MAJOR == (major) && PURC_VERSION_MINOR > (minor)) || \
    (PURC_VERSION_MAJOR == (major) && PURC_VERSION_MINOR == (minor) && \
     PURC_VERSION_MICRO >= (micro)))

PCA_EXTERN_C_BEGIN

/**
 * purc_get_major_version:
 *
 * Returns: The major version of PurC.
 *
 * Since 0.0.1
 */
PCA_INLINE int
purc_get_major_version(void)
{
    return PURC_VERSION_MAJOR;
}

/**
 * purc_get_minor_version:
 *
 * Returns: The minor version of PurC.
 *
 * Since 0.0.1
 */
PCA_INLINE int
purc_get_minor_version(void)
{
    return PURC_VERSION_MINOR;
}

/**
 * purc_get_micro_version:
 *
 * Returns: The micro version of PurC.
 *
 * Since 0.0.1
 */
PCA_INLINE int
purc_get_micro_version(void)
{
    return PURC_VERSION_MICRO;
}

/**
 * purc_get_versions:
 *
 * @major: the buffer for major version (nullable).
 * @minor: the buffer for minor version (nullable).
 * @micro: the buffer for micro version (nullable).
 *
 * Returns: none.
 *
 * Since 0.0.1
 */
PCA_INLINE void
purc_get_versions (int *major, int *minor, int *micro) {
    if (major) *major = PURC_VERSION_MAJOR;
    if (minor) *minor = PURC_VERSION_MINOR;
    if (micro) *micro = PURC_VERSION_MICRO;
}

/**
 * purc_version_string:
 *
 * Returns: the pointer to the PurC version string (e.g. "1.2.3" for version 1.2.3).
 *
 * Since 0.0.1
 */
PCA_INLINE const char *
purc_get_version_string (void) {
    return PURC_VERSION_STRING;
}

/**
 * purc_api_version_string:
 *
 * Returns: the pointer to the PurC API version string (e.g. "1.2" for version 1.2).
 *
 * Since 0.0.1
 */
PCA_INLINE const char *
purc_get_api_version_string (void) {
    return PURC_API_VERSION_STRING;
}

PCA_EXTERN_C_END

#define HVML_SPEC_VERSION               "1.0"
#define HVML_SPEC_RELEASE               "硕鼠"

#define HVML_PREDEF_VARS_SPEC_VERSION   "1.0"
#define HVML_PREDEF_VARS_SPEC_RELEASE   "硕鼠"

#define HVML_INTRPR_NAME                "PurC"
#define HVML_INTRPR_VERSION             PURC_VERSION_STRING
#define HVML_INTRPR_RELEASE             "立春"

#endif /* not defined PURC_PURC_VERSION_H */

