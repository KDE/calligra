
/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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

