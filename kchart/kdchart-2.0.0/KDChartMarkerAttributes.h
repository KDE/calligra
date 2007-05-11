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

#ifndef KDCHARTMARKERATTRIBUTES_H
#define KDCHARTMARKERATTRIBUTES_H

#include <QMetaType>
#include "KDChartGlobal.h"

class QColor;
class QSizeF;
class QPen;
class QDebug;
template <typename T, typename K> class QMap;

namespace KDChart {

    class KDCHART_EXPORT MarkerAttributes
    {
    public:
        MarkerAttributes();
        MarkerAttributes( const MarkerAttributes& );
        MarkerAttributes &operator= ( const MarkerAttributes& );

        ~MarkerAttributes();

        enum MarkerStyle { MarkerCircle  = 0,
                           MarkerSquare  = 1,
                           MarkerDiamond = 2,
                           Marker1Pixel  = 3,
                           Marker4Pixels = 4,
                           MarkerRing    = 5,
                           MarkerCross   = 6,
                           MarkerFastCross = 7 };

        void setVisible( bool visible );
        bool isVisible() const;

        typedef QMap<uint, MarkerStyle> MarkerStylesMap;
        void setMarkerStylesMap( const MarkerStylesMap & map );
        MarkerStylesMap markerStylesMap() const;

        void setMarkerStyle( MarkerStyle style );
        MarkerStyle markerStyle() const;

        void setMarkerSize( const QSizeF& size );
        QSizeF markerSize() const;

        void setMarkerColor( const QColor& color );
        QColor markerColor() const;

        void setPen( const QPen& pen );
        QPen pen() const;


        bool operator==( const MarkerAttributes& ) const;
        bool operator!=( const MarkerAttributes& ) const;

    private:
        KDCHART_DECLARE_PRIVATE_BASE_VALUE( MarkerAttributes )
    }; // End of class MarkerAttributes

    inline bool MarkerAttributes::operator!=( const MarkerAttributes & other ) const { return !operator==( other ); }
}

KDCHART_DECLARE_SWAP_SPECIALISATION( KDChart::MarkerAttributes )
Q_DECLARE_TYPEINFO( KDChart::MarkerAttributes, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( KDChart::MarkerAttributes )

#ifndef QT_NO_DEBUG_STREAM
KDCHART_EXPORT QDebug operator<<( QDebug, const KDChart::MarkerAttributes & );
#endif

#endif // KDCHARTMARKERATTRIBUTES_H
