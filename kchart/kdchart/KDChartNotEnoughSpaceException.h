/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTNOTENOUGHSPACEEXCEPTION_H__
#define __KDCHARTNOTENOUGHSPACEEXCEPTION_H__

#ifdef USE_EXCEPTIONS

#include <qstring.h>

/**
   This exception is thrown when there is not enough space for
   drawing all selected parts of a chart.
*/
class KDChartNotEnoughSpaceException
{
public:
    KDChartNotEnoughSpaceException();
};

#endif

#endif
