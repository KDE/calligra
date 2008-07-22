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
#include <KDChartDataValueAttributes>



using namespace KDChart;

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    Widget widget;
    widget.resize( 600, 600 );

    QVector< double > vec0,  vec1,  vec2;

    vec0 << 5 << 1 << 3 << 4 << 1;
    vec1 << 3 << 6 << 2 << 4 << 8;
    vec2 << 2 << 7 << 1 << 2 << 1;

    widget.setDataset( 0, vec0, "vec0" );
    widget.setDataset( 1, vec1, "vec1" );
    widget.setDataset( 2, vec2, "vec2" );

    widget.lineDiagram()->setPen(  Qt::NoPen );

    DataValueAttributes dva( widget.lineDiagram()->dataValueAttributes() );
    MarkerAttributes ma(  dva.markerAttributes() );
    TextAttributes ta(  dva.textAttributes() );
    ma.setVisible( true );
    // display values or not
    ta.setVisible(  false );
    dva.setTextAttributes(  ta );
    dva.setMarkerAttributes(  ma );
    dva.setVisible( true );

    widget.lineDiagram()->setDataValueAttributes( dva );
    widget.setGlobalLeading( 10,  10,  10, 10 );

    widget.show();

    return app.exec();
}
