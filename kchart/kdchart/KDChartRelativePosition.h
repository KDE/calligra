/* -*- Mode: C++ -*-
  KDChart - a multi-platform charting engine
  */

/****************************************************************************
** Copyright (C) 2005-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
**   information about KD Chart Commercial License Agreements.
**
** Contact info@kdab.net if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/

#ifndef KDCHARTREALTIVEPOSITION_H
#define KDCHARTREALTIVEPOSITION_H

#include <QDebug>
#include <QMetaType>
#include <Qt>
#include "KDChartGlobal.h"

namespace KDChart {

    class AbstractArea;
    class Position;
    class Measure;

/**
  \class RelativePosition KDChartRelativePosition.h
  \brief Defines relative position information: reference area, position
  in this area, horizontal / vertical padding, and rotating.

  \Note Using RelativePosition you can specify the relative parts
  of some position information, and you can specify the absolute parts:
  the reference area, and the position in this area.
  To get an absolute position, you will need to declare both, the relative
  and the absolute parts, otherwise the specification is incomplete and
  KD Chart will ignore it.
  */
class KDCHART_EXPORT RelativePosition
{
public:
    RelativePosition();
    RelativePosition( const RelativePosition& );

    RelativePosition & operator=( const RelativePosition & other );

    ~RelativePosition();

    void setReferenceArea( AbstractArea* area );
    AbstractArea* referenceArea() const;

    void setReferencePosition( Position position );
    Position referencePosition() const;

    void setAlignment( Qt::Alignment flags );
    Qt::Alignment alignment() const;

    void setHorizontalPadding( const Measure& padding );
    Measure horizontalPadding() const;

    void setVerticalPadding( const Measure& padding );
    Measure verticalPadding() const;

    void setRotation( qreal rot );
    qreal rotation() const;

    bool operator==( const RelativePosition& ) const;
    bool operator!=( const RelativePosition & other ) const;

private:
    KDCHART_DECLARE_PRIVATE_BASE_VALUE( RelativePosition )
};

inline bool RelativePosition::operator!=( const RelativePosition & other ) const { return !operator==( other ); }
}

KDCHART_DECLARE_SWAP_SPECIALISATION( KDChart::RelativePosition )

Q_DECLARE_TYPEINFO( KDChart::RelativePosition, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( KDChart::RelativePosition )

#if !defined(QT_NO_DEBUG_STREAM)
KDCHART_EXPORT QDebug operator<<(QDebug, const KDChart::RelativePosition& );
#endif /* QT_NO_DEBUG_STREAM */


#endif // KDCHARTREALTIVEPOSITION_H
