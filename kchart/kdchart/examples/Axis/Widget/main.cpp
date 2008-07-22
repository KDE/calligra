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

#include <QApplication>
#include <KDChartWidget>
#include <KDChartLineDiagram>
#include <KDChartCartesianAxis>
#include <KDChartDataValueAttributes>

using namespace KDChart;

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    Widget widget;
    widget.resize( 600, 600 );

    QVector< QPair <double, double> > vec0;
    QVector< double > vec1,  vec2;

    vec1 << 25 << 16 << 9 << 4 << 1 << 0
         << 1 << 4 << 9 << 16 << 25;

    // assign your datasets
    // while setting the legend
    // items text
    widget.setDataset( 0, vec1, "vec1" );


    // add and position
    widget.addLegend(Position::North);

    // configure the axes
    CartesianAxis *xAxis = new CartesianAxis( widget.lineDiagram() );
    CartesianAxis *yAxis = new CartesianAxis (widget.lineDiagram() );
    xAxis->setPosition ( CartesianAxis::Bottom );
    yAxis->setPosition ( CartesianAxis::Left );
    xAxis->setTitleText ( "Abscissa bottom position" );
    yAxis->setTitleText ( "Ordinate left position" );

    // configure Xaxis labels
    // no need to re-write labels it iterates until all
    // labels are written
    QStringList daysOfWeek;
    daysOfWeek << "Monday" << "Tuesday" << "Wednesday";
    QStringList shortDays;
    shortDays << "Mon" << "Tue" << "Wed";

    // set user configured x axis labels
    xAxis->setLabels( daysOfWeek );
    xAxis->setShortLabels(  shortDays );

    widget.lineDiagram()->addAxis( xAxis );
    widget.lineDiagram()->addAxis( yAxis );

    // show data values
    DataValueAttributes a( widget.lineDiagram()->dataValueAttributes() );
    a.setVisible( true );
    widget.lineDiagram()->setDataValueAttributes( a );

    // make sure data values are displayed at the borders
    widget.setGlobalLeading( 20, 20, 20, 20 );

    widget.show();

    return app.exec();
}
