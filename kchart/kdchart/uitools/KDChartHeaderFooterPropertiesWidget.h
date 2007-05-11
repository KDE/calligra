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
#ifndef __KDCHART_HEADERFOOTER_PROPERTIES_WIDGET_H__
#define __KDCHART_HEADERFOOTER_PROPERTIES_WIDGET_H__

#include <QWidget>

#include "kdchart_export.h"
#include "ui_KDChartHeaderFooterPropertiesWidget.h"
#include "KDChartGlobal.h"

namespace KDChart {

    class HeaderFooter;

    /**
     * \class HeaderFooterPropertiesWidget KDChartHeaderFooterPropertiesWidget.h
     * \brief
     *
     *
     */

    class UITOOLS_EXPORT HeaderFooterPropertiesWidget
       : public QWidget, private Ui::KDChartHeaderFooterPropertiesWidget
    {
        Q_OBJECT

        Q_DISABLE_COPY( HeaderFooterPropertiesWidget )
        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC_QWIDGET( HeaderFooterPropertiesWidget )

    public:
        /**
         * Default Constructor
         *
         * Creates a new widget with all data initialized empty.
         *
         * \param parent the widget parent; passed on to QWidget
         */
        HeaderFooterPropertiesWidget( QWidget* parent = 0 );

        /** Destructor. */
        ~HeaderFooterPropertiesWidget();

        void setHeaderFooter( HeaderFooter* hf );
        void setInstantApply( bool value );
	void readFromHeaderFooter( const HeaderFooter * headerFooter );
        void writeToHeaderFooter( HeaderFooter * headerFooter );

    protected Q_SLOTS:

        void slotTextChanged( const QString& text );
	void slotPositionChanged( int idx );
        void slotTypeChanged( bool toggled );

    Q_SIGNALS:
        // emitted whenever a value in the dialog changes
        void changed();
    };
}

#endif // KDChart_HEADERFOOTER_PROPERTIES_Widget_H
