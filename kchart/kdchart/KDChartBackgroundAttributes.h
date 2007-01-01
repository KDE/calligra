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

#ifndef KDCHARTBACKGROUNDATTRIBUTES_H
#define KDCHARTBACKGROUNDATTRIBUTES_H

#include <QDebug>
#include <QMetaType>
#include <QBrush>
#include "KDChartGlobal.h"

namespace KDChart {

class KDCHART_EXPORT BackgroundAttributes
{
public:
    BackgroundAttributes();
    BackgroundAttributes( const BackgroundAttributes& );
    BackgroundAttributes &operator= ( const BackgroundAttributes& );

    ~BackgroundAttributes();

    enum BackgroundPixmapMode { BackgroundPixmapModeNone,
                                BackgroundPixmapModeCentered,
                                BackgroundPixmapModeScaled,
                                BackgroundPixmapModeStretched };

    void setVisible( bool visible );
    bool isVisible() const;

    void setBrush( const QBrush &brush );
    QBrush brush() const;

    void setPixmapMode( BackgroundPixmapMode mode );
    BackgroundPixmapMode pixmapMode() const;

    void setPixmap( const QPixmap &backPixmap );
    QPixmap pixmap() const;

    bool operator==( const BackgroundAttributes& ) const;
    inline bool operator!=( const BackgroundAttributes& other ) const { return !operator==(other); }

private:
    KDCHART_DECLARE_PRIVATE_BASE_VALUE( BackgroundAttributes )
}; // End of class BackgroundAttributes

}

#if !defined(QT_NO_DEBUG_STREAM)
KDCHART_EXPORT QDebug operator<<(QDebug, const KDChart::BackgroundAttributes& );
#endif /* QT_NO_DEBUG_STREAM */

Q_DECLARE_METATYPE( KDChart::BackgroundAttributes )
Q_DECLARE_TYPEINFO( KDChart::BackgroundAttributes, Q_MOVABLE_TYPE );
KDCHART_DECLARE_SWAP_SPECIALISATION( KDChart::BackgroundAttributes )

#endif // KDCHARTBACKGROUNDATTRIBUTES_H
