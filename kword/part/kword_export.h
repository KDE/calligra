/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <koffice_export.h>

#ifndef _KWORD_EXPORT_H
#define _KWORD_EXPORT_H

#ifdef COMPILING_TESTS
# ifdef Q_WS_WIN
#   ifdef MAKE_KWORD_TEST_LIB
#       define KWORD_TEST_EXPORT KDE_EXPORT
#   elif KDE_MAKE_LIB
#       define KWORD_TEST_EXPORT KDE_IMPORT
#   else
#       define KWORD_TEST_EXPORT
#   endif
# else // not windows
#   define KWORD_TEST_EXPORT KDE_EXPORT
# endif
#else // not compiling tests
#   define KWORD_TEST_EXPORT
#endif

#endif
