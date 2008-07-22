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

#include <QPixmap>

namespace KDChart {
    class Chart;
    class DatasetProxyModel;
    class LineDiagram;
    class LineAttributes;
    class CartesianAxis;
    class Legend;
}

class QLabel;

class MainWindow : public QWidget, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget* parent = 0 );

    protected:
        /**
         * Adjusts the two small charts when the window is resized.
         */
        /* reimp */ void resizeEvent ( QResizeEvent * );

private slots:

    void on_lineTypeCB_currentIndexChanged( const QString & text );
    void on_paintLegendCB_toggled( bool checked );
    void on_paintValuesCB_toggled( bool checked );
    void on_paintMarkersCB_toggled( bool checked );
    void on_markersStyleCB_currentIndexChanged( const QString & text );
    void on_markersWidthSB_valueChanged( int i );
    void on_markersHeightSB_valueChanged( int i);
    void on_displayAreasCB_toggled( bool checked );
    void on_transparencySB_valueChanged( int i );
    void on_zoomFactorSB_valueChanged( double factor );
    void on_hSBar_valueChanged( int value );
    void on_vSBar_valueChanged( int value );
    void on_savePB_clicked();


private:
    TableModel m_model;
    KDChart::DatasetProxyModel* m_datasetProxy;
    KDChart::Legend* m_legend;
    // mutable KDChart::CartesianAxis xAxis;
    //mutable KDChart::CartesianAxis yAxis;

protected:
    KDChart::Chart* m_chart;
    KDChart::LineDiagram* m_lines;
    QPixmap m_pix1, m_pix2;
    QLabel* m_smallChart1;
    QLabel* m_smallChart2;


};


#endif /* MAINWINDOW_H */

