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

#include <QDebug>
#include <QVector>

#include "mainwindow.h"

using namespace KDChart;

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    MainWindow window;

    window.resize( 800, 600 );

    QVector< double > vec0;
    vec0.append( -5 );
    vec0.append( -4 );
    vec0.append( -3 );
    vec0.append( -2 );
    vec0.append( -1 );
    vec0.append( 0 );
    vec0.append( 1 );
    vec0.append( 2 );
    vec0.append( 3 );
    vec0.append( 4 );
    vec0.append( 5 );
    window.widget->setDataset( 0, vec0, "Linear" );

    QVector< double > vec1;
    vec1.append( 25 );
    vec1.append( 16 );
    vec1.append( 9 );
    vec1.append( 4 );
    vec1.append( 1 );
    vec1.append( 0 );
    vec1.append( 1 );
    vec1.append( 4 );
    vec1.append( 9 );
    vec1.append( 16 );
    vec1.append( 25 );
    window.widget->setDataset( 1, vec1, "Quadratic" );

    QVector< double > vec2;
    vec2.append( -125 );
    vec2.append( -64 );
    vec2.append( -27 );
    vec2.append( -8 );
    vec2.append( -1 );
    vec2.append( 0 );
    vec2.append( 1 );
    vec2.append( 8 );
    vec2.append( 27 );
    vec2.append( 64 );
    vec2.append( 125 );
    window.widget->setDataset( 2, vec2, "Cubic" );

    window.widget->addHeaderFooter( "Header West",   HeaderFooter::Header, Position::West );
    window.widget->addHeaderFooter( "Header Center", HeaderFooter::Header, Position::Center );
    window.widget->addHeaderFooter( "Header East",   HeaderFooter::Header, Position::East );
    window.widget->addHeaderFooter( "Footer West",   HeaderFooter::Footer, Position::West );
    window.widget->addHeaderFooter( "Footer Center", HeaderFooter::Footer, Position::Center );
    window.widget->addHeaderFooter( "Footer East",   HeaderFooter::Footer, Position::East );

    window.widget->addLegend( Position::East );
    window.show();

    return app.exec();
}
