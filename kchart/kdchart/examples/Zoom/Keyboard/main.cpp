/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
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
    window.widget->setDataset( 0, vec0);

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
    window.widget->setDataset( 1, vec1 );

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
    window.widget->setDataset( 2, vec2 );

    window.widget->addHeaderFooter( "Zoom Example",
                                    HeaderFooter::Header, Position::NorthWest );
    window.widget->addHeaderFooter( "zoom in/out: PageDn / PageUp",
                                    HeaderFooter::Header, Position::North );
    window.widget->addHeaderFooter( "pan around: Left / Right / Up / Down",
                                    HeaderFooter::Header, Position::NorthEast );
    window.widget->addHeaderFooter( "SouthEast",
                                    HeaderFooter::Footer, Position::SouthEast );
    window.widget->addHeaderFooter( "South",
                                    HeaderFooter::Footer, Position::South );
    window.widget->addHeaderFooter( "SouthWest",
                                    HeaderFooter::Footer, Position::SouthWest );

    window.widget->addLegend( Position::East );

    //window.widget->setType( Widget::Polar );

    window.show();

    return app.exec();
}
