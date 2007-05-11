
/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include "KDChartChartSerializer.h"


#include <QDebug>
#include <QtDesignerUI4>


using namespace KDChart;

ChartSerializer::ChartSerializer( )
{
}

ChartSerializer::~ChartSerializer( )
{
}

bool ChartSerializer::saveExtraInfo( const Chart * chart, DomWidget * ui_widget )
{
    Q_UNUSED( chart)

    QList<DomItem*> items;
    DomItem* item = new DomItem;
    item->setText(QLatin1String("Chart Details"));
    items.append( item );

    // PENDING(kalle) Save planes
    // PENDING(kalle) Save legends
    // PENDING(kalle) Save header footers
    // PENDING(kalle) Save global leadings


    ui_widget->setElementItem( items );

    return true;
}


bool ChartSerializer::loadExtraInfo( Chart * chart, DomWidget * ui_widget )
{
    Q_UNUSED( chart)

    const QList<DomItem*> items = ui_widget->elementItem();

    for ( QList<DomItem*>::const_iterator itemit = items.begin(), itemsend = items.end() ;
          itemit != itemsend ; ++itemit )
    {
        DomItem* item = (*itemit);
        QList<DomProperty*> properties = item->elementProperty();

        for( QList<DomProperty*>::const_iterator propsit = properties.begin()
             , propsend = properties.end()
             ; propsit != propsend; ++propsit )
        {
            DomProperty *p = (*propsit);
            if ( p->text() == QLatin1String("Chart Details"))
                qDebug() << "Found a KDChart::Chart";
        }
    }
    return true;
}

