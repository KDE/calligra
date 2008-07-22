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
#include <KDChartLegend>


using namespace KDChart;

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    Widget widget;
    widget.resize( 600, 600 );

    QVector< double > vec0,  vec1,  vec2;

    vec0 << 5 << 1 << 3 << 4 << 1;
    vec1 << 3 << 6 << 2 << 4 << 8;
    vec2 << 0 << 7 << 1 << 2 << 1;

    widget.setDataset( 0, vec0, "vec0" );
    widget.setDataset( 1, vec1, "vec1" );
    widget.setDataset( 2, vec2, "vec2" );
    widget.setSubType(  Widget::Percent );

    //Configure a pen and draw a line
    //a dot line for column 1
    QPen pen;
    QBrush brush;
    pen.setWidth(3);
    pen.setStyle( Qt::DotLine );
    pen.setColor(  Qt::yellow );
    brush.setColor(  Qt::yellow );
    // call your diagram and set the new pen
    // and brush
    widget.lineDiagram()->setPen(  1 , pen );
    widget.lineDiagram()->setBrush( 1,  brush );

    //set up a legend
    Legend* legend = new Legend( &widget );
    legend->setPosition( Position::East );
    legend->setAlignment( Qt::AlignCenter );
    legend->setOrientation( Qt::Vertical );
    legend->setLegendStyle( Legend::LinesOnly );
    legend->setShowLines(  true );
    widget.addLegend( legend );
    //Set up your Attributes
    //display areas
    LineAttributes la( widget.lineDiagram()->lineAttributes() );
    la.setDisplayArea(  true );
    la.setTransparency( 25 );
    widget.lineDiagram()->setLineAttributes( la );

    widget.show();

    return app.exec();
}
