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

#ifndef KDCHARTPOSITION_H
#define KDCHARTPOSITION_H

#include <QDebug>
#include <Qt>
#include <QMetaType>
#include <QCoreApplication>
#include "KDChartGlobal.h"
#include "KDChartEnums.h"

class QStringList;
class QByteArray;
template <typename T> class QList;

namespace KDChart {

/**
 * \class Position KDChartPosition.h
 * \brief Defines a position, using compass terminology.
 *
 * Using KDChartPosition you can specify one of nine
 * pre-defined, logical points (see the \c static \c const getter
 * methods below), in a similar way, as you would use a
 * compass to navigate on a map.
 *
 * \Note Often you will declare a \c Position together with the
 * RelativePosition class, to specify a logical point,
 * which then will be used to layout your chart at runtime,
 * e.g. for specifying the location of a floating Legend box.
 *
 * For comparing a Position's value with a switch() statement,
 * you can use numeric values defined in KDChartEnums, like this:
\verbatim
switch( yourPosition().value() ) {
    case KDChartEnums::PositionNorthWest:
        // your code ...
        break;
    case KDChartEnums::PositionNorth:
        // your code ...
        break;
}
\endverbatim
 * \sa RelativePosition, KDChartEnums::PositionValue
 */

class KDCHART_EXPORT Position
{
    Q_DECLARE_TR_FUNCTIONS( Position )
    Position( int value );
public:
    Position();
    Position( KDChartEnums::PositionValue value ); // intentionally non-explicit

    KDChartEnums::PositionValue value() const;

    const char * name() const;
    QString printableName() const;

    bool isWestSide() const;
    bool isNorthSide() const;
    bool isEastSide() const;
    bool isSouthSide() const;

    bool isCorner() const;
    bool isPole() const;

    static const Position& Unknown;
    static const Position& Center;
    static const Position& NorthWest;
    static const Position& North;
    static const Position& NorthEast;
    static const Position& East;
    static const Position& SouthEast;
    static const Position& South;
    static const Position& SouthWest;
    static const Position& West;

    enum Option { IncludeCenter=0, ExcludeCenter=1 };
    Q_DECLARE_FLAGS( Options, Option )

    static QList<QByteArray> names( Options options=IncludeCenter );
    static QStringList printableNames( Options options=IncludeCenter );

    static Position fromName(const char * name);
    static Position fromName(const QByteArray & name);

    bool operator==( const Position& ) const;
    bool operator==( int ) const;
    bool operator!=( const Position& ) const;
    bool operator!=( int ) const;

private:
    int m_value;
}; // End of class Position

inline bool Position::operator!=( const Position & other ) const { return !operator==( other ); }
inline bool Position::operator!=( int other ) const { return !operator==( other ); }

}

Q_DECLARE_TYPEINFO( KDChart::Position, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( KDChart::Position )
Q_DECLARE_OPERATORS_FOR_FLAGS( KDChart::Position::Options )

#if !defined(QT_NO_DEBUG_STREAM)
KDCHART_EXPORT QDebug operator<<(QDebug, const KDChart::Position& );
#endif /* QT_NO_DEBUG_STREAM */

#endif // KDCHARTPOSITION_H
