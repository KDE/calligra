/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
** Copyright (C) 2001-2002 Klarälvdalens Datakonsult AB.  All rights reserved.
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
** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
**   information about KDChart Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/
#ifndef __KDCHARTGLOBAL_H__
#define __KDCHARTGLOBAL_H__

#include <qglobal.h>


/** \file KDChartGlobal.h
    \brief Definition of global macros.
*/



#if QT_VERSION < 300

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
// end of #if QT_VERSION < 300



#endif
