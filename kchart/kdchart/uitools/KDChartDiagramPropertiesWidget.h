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
#ifndef __KDCHART_DIAGRAM_PROPERTIES_WIDGET_H__
#define __KDCHART_DIAGRAM_PROPERTIES_WIDGET_H__

#include <QWidget>

#include "kdchart_export.h"
#include "ui_KDChartDiagramPropertiesWidget.h"
#include "KDChartGlobal.h"

namespace KDChart {

    class LineDiagram;
    class BarDiagram;
    class PieDiagram;

    /**
     * \class DiagramPropertiesWidget KDChartDiagramPropertiesWidget.h
     * \brief
     *
     *
     */

    class UITOOLS_EXPORT DiagramPropertiesWidget
       : public QWidget, private Ui::KDChartDiagramPropertiesWidget
    {
        Q_OBJECT

        Q_DISABLE_COPY( DiagramPropertiesWidget )
        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC_QWIDGET( DiagramPropertiesWidget )

    public:
        /**
         * Default Constructor
         *
         * Creates a new widget with all data initialized empty.
         *
         * \param parent the widget parent; passed on to QWidget
         */
        DiagramPropertiesWidget( QWidget* parent = 0 );

        /** Destructor. */
        ~DiagramPropertiesWidget();

        void setLineDiagram( LineDiagram* ld );
        void setBarDiagram( BarDiagram* bd );
        void setPieDiagram( PieDiagram* pd );
        void setInstantApply( bool value );
	void readFromLineDiagram( const LineDiagram * ld );
        void readFromBarDiagram( const BarDiagram* bd );
        void readFromPieDiagram( const PieDiagram* pd );
        void writeToLineDiagram( LineDiagram * ld );
        void writeToBarDiagram( BarDiagram * bd );
        void writeToPieDiagram( PieDiagram * pd );

    protected Q_SLOTS:
      
        void slotTypeChanged( int type );

    Q_SIGNALS:
        // emitted whenever a value in the dialog changes
        void changed();
    };
}

#endif // KDChart_HEADERFOOTER_PROPERTIES_Widget_H
