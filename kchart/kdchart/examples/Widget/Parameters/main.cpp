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
#include <KDChartBarDiagram>
#include <QPen>


using namespace KDChart;

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    Widget widget;
    widget.resize( 600, 600 );

    QVector< double > vec0,  vec1;

    vec0 << 5 << 4 << 3 << 2 << 1 << 0
         << 1 << 2 << 3 << 4 << 5;
    vec1 << 25 << 16 << 9 << 4 << 1 << 0
         << 1 << 4 << 9 << 16 << 25;

    widget.setDataset( 0, vec0, "vec0" );
    widget.setDataset( 1, vec1, "vec1" );
    widget.setType( Widget::Bar ,  Widget::Stacked );

    //Configure a pen and draw a line
    //surrounding the bars
    QPen pen;
    pen.setWidth( 2 );
    pen.setColor(  Qt::darkGray );
    // call your diagram and set the new pen
    widget.barDiagram()->setPen(  pen );

    //Set up your ThreeDAttributes
    //display in ThreeD mode
    ThreeDBarAttributes td( widget.barDiagram()->threeDBarAttributes() );
    td.setDepth(  15 );
    td.setEnabled(  true );
    widget.barDiagram()->setThreeDBarAttributes( td );
    widget.setGlobalLeadingTop( 20 );
    widget.show();

    return app.exec();
}
