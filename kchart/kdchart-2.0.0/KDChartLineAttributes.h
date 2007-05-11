/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#ifndef KDCHARTLINEATTRIBUTES_H
#define KDCHARTLINEATTRIBUTES_H

#include <QMetaType>
#include "KDChartGlobal.h"

namespace KDChart {

class KDCHART_EXPORT LineAttributes
{
public:
    /**
      \brief MissingValuesPolicy specifies how a missing value will be shown in a line diagram.

      Missing value is assumed if the data cell contains a QVariant that can not be
      interpreted as a double.

      \li \c MissingValuesAreBridged the default: No markers will be shown for missing values
      but the line will be bridged if there is at least one valid cell before and after
      the missing value(s), otherwise the segment will be hidden.
      \li \c MissingValuesHideSegments Line segments starting with a missing value will
      not be shown, and no markers will be shown for missing values, so this will look like
      a piece of the line is missing.
      \li \c MissingValuesShownAsZero Missing value(s) will be treated like normal zero values,
      and markers will shown for them too, so there will be no visible difference between a
      zero value and a missing value.
      \li \c MissingValuesPolicyIgnored (internal value, do not use)

      */
    enum MissingValuesPolicy {
        MissingValuesAreBridged,
        MissingValuesHideSegments,
        MissingValuesShownAsZero,
        MissingValuesPolicyIgnored };

    LineAttributes();
    LineAttributes( const LineAttributes& );
    LineAttributes &operator= ( const LineAttributes& );

    ~LineAttributes();

    /* line chart and area chart - all types */
    void setMissingValuesPolicy( MissingValuesPolicy policy );
    MissingValuesPolicy missingValuesPolicy() const;

    /* area chart - all types */
    void setDisplayArea( bool display );
    bool displayArea() const;
    /*allows viewing the covered areas*/
    void setTransparency( uint alpha );
    uint transparency() const;

    bool operator==( const LineAttributes& ) const;
    inline bool operator!=( const LineAttributes& other ) const { return !operator==(other); }

private:
    KDCHART_DECLARE_PRIVATE_BASE_VALUE( LineAttributes )
}; // End of class GridAttributes

}

#if !defined(QT_NO_DEBUG_STREAM)
KDCHART_EXPORT QDebug operator<<(QDebug, const KDChart::LineAttributes& );
#endif /* QT_NO_DEBUG_STREAM */

KDCHART_DECLARE_SWAP_SPECIALISATION( KDChart::LineAttributes )
Q_DECLARE_METATYPE( KDChart::LineAttributes )
Q_DECLARE_TYPEINFO( KDChart::LineAttributes, Q_MOVABLE_TYPE );


#endif // KDCHARTLINEATTRIBUTES_H
