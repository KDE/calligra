/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/
#ifndef __KDCHART_LEGEND_PROPERTIES_WIDGET_H__
#define __KDCHART_LEGEND_PROPERTIES_WIDGET_H__

#include <QWidget>

#include "kdchart_export.h"
#include "ui_KDChartLegendPropertiesWidget.h"
#include "KDChartGlobal.h"

namespace KDChart {

    class Legend;

    /**
     * \class LegendPropertiesWidget KDChartLegendPropertiesWidget.h
     * \brief
     *
     *
     */
    class UITOOLS_EXPORT LegendPropertiesWidget
    : public QWidget, private Ui::KDChartLegendPropertiesWidget
    {
        Q_OBJECT

        Q_DISABLE_COPY( LegendPropertiesWidget )
        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC_QWIDGET( LegendPropertiesWidget )

    public:
        LegendPropertiesWidget( QWidget* parent = 0 );

        ~LegendPropertiesWidget();

        void setLegend( Legend* legend );
        void setInstantApply( bool value );
        void readFromLegend( const Legend * legend );
        void writeToLegend( Legend * legend );

    protected Q_SLOTS:
        void slotPositionChanged( int idx );
        void slotOrientationChanged( bool toggled );
        void slotAlignmentChanged( bool toggled );
        void slotTitleTextChanged( const QString& text );
        void slotShowLineChanged( int state );

    Q_SIGNALS:
        // emitted whenever a value in the dialog changes
        void changed();
    };
}

#endif // KDChart_LEGEND_PROPERTIES_Widget_H
