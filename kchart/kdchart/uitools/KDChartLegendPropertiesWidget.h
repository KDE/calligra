/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
