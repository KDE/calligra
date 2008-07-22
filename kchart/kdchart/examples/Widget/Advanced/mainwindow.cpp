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

#include "mainwindow.h"

#include <KDChartWidget>
#include <QDebug>

using namespace KDChart;

MainWindow::MainWindow( QWidget* parent )
 : QWidget( parent ), datasetCount( 3 )
{
    setupUi( this );

    QHBoxLayout* chartLayout = new QHBoxLayout( chartFrame );
    widget = new Widget( chartFrame );
    chartLayout->addWidget( widget );

    connect( typeSelector, SIGNAL( activated( int )), SLOT( changeType() ));

    connect( btnAddDataset, SIGNAL( clicked()), SLOT( addDataset() ));

    connect( leadingSelector, SIGNAL( valueChanged( int )),
		    this, SLOT( changeLeading( int )));
}

void MainWindow::changeType()
{
    QString text = typeSelector->currentText();
    if ( text == "Widget::Bar" )
        widget->setType( Widget::Bar );
    else if ( text == "Widget::Line" )
        widget->setType( Widget::Line );
    else if ( text == "Widget::Pie" )
        widget->setType( Widget::Pie );
    else if ( text == "Widget::Polar" )
        widget->setType( Widget::Polar );
    else if ( text == "Widget::Ring" )
        widget->setType( Widget::Ring );
    else
        widget->setType( Widget::NoType );
}

void MainWindow::changeLeading( int leading )
{
    widget->setGlobalLeading( leading, leading, leading, leading );
}

void MainWindow::addDataset()
{
    QStringList parts = lineAddDataset->text().split(";");
    bool ok;
    QVector< double > vec;
    foreach (QString str, parts ) {
        vec.append( str.toDouble(&ok) );
    }
    widget->setDataset( datasetCount++, vec );
}
