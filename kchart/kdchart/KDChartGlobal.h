/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

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

#endif
// end of #if QT_VERSION < 300



#endif
