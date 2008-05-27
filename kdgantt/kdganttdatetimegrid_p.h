/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Gantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Gantt licenses may use this file in
 ** accordance with the KD Gantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdgantt for
 **   information about KD Gantt Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef KDGANTTDATETIMEGRID_P_H
#define KDGANTTDATETIMEGRID_P_H

#include "kdganttdatetimegrid.h"
#include "kdganttabstractgrid_p.h"

#include <QDateTime>

namespace KDGantt {
    class DateTimeGrid::Private : public AbstractGrid::Private {
    public:
        Private() : startDateTime( QDateTime::currentDateTime().addDays( -3 ) ),
                    dayWidth( 100. ), scale(ScaleAuto), weekStart( Qt::Monday ),
                    freeDays( QSet<Qt::DayOfWeek>() << Qt::Saturday << Qt::Sunday ),
                    rowSeparators( false ) {}

        qreal dateTimeToChartX( const QDateTime& dt ) const;
        QDateTime chartXtoDateTime( qreal x ) const;

        QDateTime startDateTime;
        QDateTime endDateTime;
        qreal dayWidth;
	Scale scale;
        Qt::DayOfWeek weekStart;
        QSet<Qt::DayOfWeek> freeDays;
        bool rowSeparators;
    };

    inline DateTimeGrid::DateTimeGrid( DateTimeGrid::Private* d ) : AbstractGrid( d ) {}

    inline DateTimeGrid::Private* DateTimeGrid::d_func() {
        return static_cast<Private*>( AbstractGrid::d_func() );
    }
    inline const DateTimeGrid::Private* DateTimeGrid::d_func() const {
        return static_cast<const Private*>( AbstractGrid::d_func() );
    }
}

#endif /* KDGANTTDATETIMEGRID_P_H */

