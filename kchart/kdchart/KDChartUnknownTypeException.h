/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTUNKNOWNTYPEEXCEPTION_H__
#define __KDCHARTUNKNOWNTYPEEXCEPTION_H__

#ifdef USE_EXCEPTIONS

#include <qstring.h>

/**
   This exception is thrown when it is attempted to create a chart of
   non-registered user-defined type.
*/
class KDChartUnknownTypeException
{
public:
KDChartUnknownTypeException( const QString& type ) :
    _type( type )
    {}

    QString type() const
    {
        return _type;
    }

private:
    QString _type;
};

#endif

#endif
