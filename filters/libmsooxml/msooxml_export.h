/*
 * This file is part of Office 2007 Filters for KOffice
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

#ifndef MSOOXML_EXPORT_H
#define MSOOXML_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef MSOOXML_EXPORT
# if defined(MAKE_MSOOXML_LIB)
/* We are building this library */
#  define MSOOXML_EXPORT KDE_EXPORT
# else
/* We are using this library */
#  define MSOOXML_EXPORT KDE_IMPORT
# endif
#endif

# ifndef MSOOXML_EXPORT_DEPRECATED
#  define MSOOXML_EXPORT_DEPRECATED KDE_DEPRECATED MSOOXML_EXPORT
# endif

#endif
