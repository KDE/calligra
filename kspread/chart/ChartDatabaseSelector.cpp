/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "ChartDatabaseSelector.h"

#include "ui_ChartDatabaseSelector.h"

#include "KoShape.h"

#include "koChart.h"

#include "Doc.h"
#include "Region.h"
#include "TableModel.h"

using namespace KSpread;

class ChartDatabaseSelector::Private
{
public:
    Doc* doc;
    KoChart::ChartInterface* shape;
    Ui::ChartDatabaseSelector widget;
};

ChartDatabaseSelector::ChartDatabaseSelector( Doc* doc )
    : d( new Private )
{
    d->doc = doc;
    d->shape = 0;
    d->widget.setupUi(this);
}

ChartDatabaseSelector::~ChartDatabaseSelector()
{
    delete d;
}

void ChartDatabaseSelector::open(KoShape* shape)
{
    d->shape = dynamic_cast<KoChart::ChartInterface*>( shape );
}

void ChartDatabaseSelector::save()
{
    const Region region( d->doc->map(), d->widget.m_cellRegion->text(), 0 );
    if ( !region.isValid() )
        return;
    TableModel* tableModel = new TableModel();
    tableModel->setRegion( region );
    d->shape->setModel( tableModel );
}

KAction* ChartDatabaseSelector::createAction()
{
    return 0;
}

#include "ChartDatabaseSelector.moc"
