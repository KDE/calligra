/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2007 Klarälvdalens Datakonsult AB.  All rights reserved.
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

#include <limits>

#include <QChar>
#include <QTextStream>

#include "TernaryPoint.h"
#include "TernaryConstants.h"

TernaryPoint::TernaryPoint()
    : m_a( -1.0 )
    , m_b( -1.0 )
{
    Q_ASSERT( !isValid() );
}

TernaryPoint::TernaryPoint( double a, double b )
    : m_a( -1.0 )
    , m_b( -1.0 )
{
    set( a, b );
}

void TernaryPoint::set( double a, double b )
{
    if ( a >= 0.0 && a <= 1.0
         && b >= 0.0 && b <= 1.0
         && 1.0 - a - b >= -2.0 * std::numeric_limits<double>::epsilon() ) {
        m_a = a;
        m_b = b;
        Q_ASSERT( isValid() ); // more a test for isValid
    } else {
        m_a = -1.0;
        m_b = -1.0;
        Q_ASSERT( ! isValid() );
    }
}

bool TernaryPoint::isValid() const
{
    return
        m_a >= 0.0 && m_a <= 1.0
        && m_b >= 0.0 && m_b <= 1.0
        && 1.0 - m_a + m_b >= - std::numeric_limits<double>::epsilon();
}

QDebug operator<<( QDebug stream, const TernaryPoint& point )
{
    QString string;
    QTextStream text( &string );
    text << "[TernaryPoint: ";
    if ( point.isValid() ) {
        text.setFieldWidth( 2 );
        text.setPadChar( QLatin1Char( '0' ) );
        text << ( int ) ( point.a() * 100.0 ) << "%|"
             << ( int ) ( point.b() * 100.0 ) << "%|"
             << ( int ) ( point.c() * 100.0 ) << "%]";
    } else {
        text << "a=" << point.a() << " - b=" << point.b() << " - INVALID]";
    }
    stream << string;
    return stream;
}

QPointF translate( const TernaryPoint& point )
{
    if ( point.isValid() ) {
        // the position is calculated by
        // - first moving along the B-C line to the function that b
        //   selects
        // - then traversing the selected function until we meet with
        //   the function that A selects (which is a parallel of the B-C
        //   line)
        QPointF bPosition( 1.0 - point.b(), 0.0 );
        QPointF aPosition( point.a() * AxisVector_C_A );
        QPointF result( bPosition + aPosition );
        return result;
    } else {
        qWarning() << "TernaryPoint::translate(TernaryPoint): cannot translate invalid ternary points:"
                   << point;
        return QPointF();
    }
}
