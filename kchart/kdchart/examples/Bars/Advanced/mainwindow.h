/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <TableModel.h>

namespace KDChart {
    class Chart;
    class BarDiagram;
}

class MainWindow : public QWidget, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget* parent = 0 );

private slots:

    void on_barTypeCB_currentIndexChanged( const QString & text );
    void on_paintValuesCB_toggled( bool checked );
    void on_paintThreeDBarsCB_toggled( bool checked );
    void on_markColumnCB_toggled( bool checked );
    void on_markColumnSB_valueChanged( int i );
    void on_threeDDepthCB_toggled( bool checked );
    void on_depthSB_valueChanged( int i );
    void on_widthCB_toggled( bool checked );
    void on_widthSB_valueChanged( int i );

private:
    KDChart::Chart* m_chart;
    KDChart::BarDiagram* m_bars;
    TableModel m_model;
};


#endif /* MAINWINDOW_H */

