/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef KOMSOOXML_EXPORT_H
#define KOMSOOXML_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef KOMSOOXML_EXPORT
# if defined(MAKE_KOMSOOXML_LIB)
/* We are building this library */
#  define KOMSOOXML_EXPORT KDE_EXPORT
# else
/* We are using this library */
#  define KOMSOOXML_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KOMSOOXML_EXPORT_DEPRECATED
#  define KOMSOOXML_EXPORT_DEPRECATED KDE_DEPRECATED KOMSOOXML_EXPORT
# endif

#endif
