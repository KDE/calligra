/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#ifndef KDCHART_EXPORT_H
#define KDCHART_EXPORT_H

#include <qglobal.h>

#ifdef Q_OS_WIN
# ifdef KDCHART_NODLL
#  undef KDCHART_MAKEDLL
# elif defined( KDCHART_MAKEDLL )
#  ifdef KDCHART_BUILD_KDCHART_LIB
#   define KDCHART_EXPORT Q_DECL_EXPORT
#  else
#   define KDCHART_EXPORT Q_DECL_IMPORT
#  endif
#  ifdef UITOOLS_BUILD_UITOOLS_LIB
#   define UITOOLS_EXPORT Q_DECL_EXPORT
#  else
#   define UITOOLS_EXPORT Q_DECL_IMPORT
#  endif
#  ifdef KDCHART_BUILD_KDCHART_COMPAT_LIB
#   define KDCHART_COMPAT_EXPORT Q_DECL_EXPORT
#  else
#   define KDCHART_COMPAT_EXPORT Q_DECL_IMPORT
#  endif
# else
#  define KDCHART_EXPORT Q_DECL_IMPORT
#  define UITOOLS_EXPORT Q_DECL_IMPORT
#  define KDCHART_COMPAT_EXPORT Q_DECL_IMPORT
# endif
#else
# undef KDCHART_MAKEDLL
#endif

#ifndef KDCHART_EXPORT
# ifdef QT_SHARED
#  define KDCHART_EXPORT Q_DECL_EXPORT
#  define UITOOLS_EXPORT Q_DECL_EXPORT
#  define KDCHART_COMPAT_EXPORT Q_DECL_EXPORT
# else
#  define KDCHART_EXPORT
#  define KDCHART_COMPAT_EXPORT Q_DECL_EXPORT
# endif
#endif

#endif // KDCHART_EXPORT_H
