/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2006 David Faure <faure@kde.org>
    SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RDF_EXPORT_H
#define RDF_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

/* We use _WIN32/_WIN64 instead of Q_OS_WIN so that this header can be used from C files too */
#if defined _WIN32 || defined _WIN64

#ifndef KORDF_EXPORT
#if defined(MAKE_KORDF_LIB)
/* We are building this library */
#define KORDF_EXPORT KDE_EXPORT
#else
/* We are using this library */
#define KORDF_EXPORT KDE_IMPORT
#endif
#endif

#else /* UNIX */

#define KORDF_EXPORT KDE_EXPORT

#endif

/* Now the same for KORDF_TEST_EXPORT, if compiling with unit tests enabled */

#ifdef COMPILING_TESTS
#if defined _WIN32 || defined _WIN64
#if defined(MAKE_KORDF_LIB)
#define KORDF_TEST_EXPORT KDE_EXPORT
#else
#define KORDF_TEST_EXPORT KDE_IMPORT
#endif
#else /* not windows */
#define KORDF_TEST_EXPORT KDE_EXPORT
#endif
#else /* not compiling tests */
#define KORDF_TEST_EXPORT
#endif

#endif
