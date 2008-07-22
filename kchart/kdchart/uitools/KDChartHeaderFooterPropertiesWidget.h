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
