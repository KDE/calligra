/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klar�vdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart licenses may use this file in
 ** accordance with the KDChart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef __KDCHARTGLOBAL_H__
#define __KDCHARTGLOBAL_H__

#include <qglobal.h>

#include <float.h>

#if defined(unix) || defined(__unix__) || defined(Q_WS_MAC) || defined(Q_WS_LINUX)
#include <limits.h>
#else
#define MINDOUBLE DBL_MIN
#define MAXDOUBLE DBL_MAX
#endif

#if defined(_MSC_VER)
#pragma warning(disable: 4251)
#endif
#include <kdchart_export.h>


/* \file KDChartGlobal.h
   \brief Definition of global macros.
   */



#if defined(_MSC_VER)
#ifndef QT_NO_STL
#define QT_NO_STL // Avoid a name clash
#endif
#endif



// important: make the old format of QT_VERSION comparable
//            to the new format that is used since Qt 3.0.0
#if QT_VERSION == 141
#define COMPAT_QT_VERSION 0x010401
#endif
#if QT_VERSION == 142
#define COMPAT_QT_VERSION 0x010402
#endif
#if QT_VERSION == 143
#define COMPAT_QT_VERSION 0x010403
#endif
#if QT_VERSION == 144
#define COMPAT_QT_VERSION 0x010404
#endif
#if QT_VERSION == 145
#define COMPAT_QT_VERSION 0x010405
#endif
#if QT_VERSION == 200
#define COMPAT_QT_VERSION 0x020000
#endif
#if QT_VERSION == 201
#define COMPAT_QT_VERSION 0x020001
#endif
#if QT_VERSION == 202
#define COMPAT_QT_VERSION 0x020002
#endif
#if QT_VERSION == 210
#define COMPAT_QT_VERSION 0x020100
#endif
#if QT_VERSION == 211
#define COMPAT_QT_VERSION 0x020101
#endif
#if QT_VERSION == 220
#define COMPAT_QT_VERSION 0x020200
#endif
#if QT_VERSION == 221
#define COMPAT_QT_VERSION 0x020201
#endif
#if QT_VERSION == 222
#define COMPAT_QT_VERSION 0x020202
#endif
#if QT_VERSION == 223
#define COMPAT_QT_VERSION 0x020203
#endif
#if QT_VERSION == 224
#define COMPAT_QT_VERSION 0x020204
#endif
#if QT_VERSION == 230
#define COMPAT_QT_VERSION 0x020300
#endif
#if QT_VERSION == 231
#define COMPAT_QT_VERSION 0x020301
#endif
#if QT_VERSION == 232
#define COMPAT_QT_VERSION 0x020302
#if defined(_MSC_VER)
#pragma message( "using Qt 2.3.2" )
#endif
#endif
#if QT_VERSION == 300
#define COMPAT_QT_VERSION 0x030000
#endif
#if QT_VERSION == 301
#define COMPAT_QT_VERSION 0x030001
#endif
#if QT_VERSION == 302
#define COMPAT_QT_VERSION 0x030002
#endif
#if QT_VERSION == 303
#define COMPAT_QT_VERSION 0x030003
#endif
#if QT_VERSION == 304
#define COMPAT_QT_VERSION 0x030004
#endif
#if QT_VERSION == 305
#define COMPAT_QT_VERSION 0x030005
#endif
#if QT_VERSION == 310
#define COMPAT_QT_VERSION 0x030100
#endif
// --- new format since Qt 3.0.0 or since 3.0.4 ???
#ifndef COMPAT_QT_VERSION
#if defined(_MSC_VER)
#pragma message( "using Qt > 2.3.2" )
#endif
#define COMPAT_QT_VERSION QT_VERSION
#endif

// since Qt 3.1 they do /not/ include limits.h or climits on windows anymore
// so we must include that manually
#if COMPAT_QT_VERSION > 0x030099
#if defined(_MSC_VER)
#include <climits>
#endif
#endif


#if COMPAT_QT_VERSION < 0x030000

#if !defined(Q_ASSERT)
#if defined(QT_CHECK_STATE)
#if defined(QT_FATAL_ASSERT)
#define Q_ASSERT(x)  ((x) ? (void)0 : qFatal("ASSERT: \"%s\" in %s (%d)",#x,__FILE__,__LINE__))
#else
#define Q_ASSERT(x)  ((x) ? (void)0 : qWarning("ASSERT: \"%s\" in %s (%d)",#x,__FILE__,__LINE__))
#endif
#else
#define Q_ASSERT(x)
#endif
#endif

#if !defined(QT_NO_COMPAT)
// source compatibility with Qt 2.x
#ifndef Q_OS_TEMP
#  if !defined(ASSERT)
#    define ASSERT(x) Q_ASSERT(x)
#  endif
#endif // Q_OS_TEMP
#endif // QT_NO_COMPAT

#define QPtrList QList
#define QPtrListIterator QListIterator
#define QPtrVector QVector
#define QMemArray QArray

#include <qlist.h>
#include <qvector.h>
#include <qarray.h>

#else

#include <qptrlist.h>
#include <qptrvector.h>
#include <qmemarray.h>

#endif
// end of #if COMPAT_QT_VERSION < 0x030000



#endif
