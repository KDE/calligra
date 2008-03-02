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

#include "KoCanvasResourceProvider.h"
#include "KoShape.h"

#include "koChart.h"

#include "Binding.h"
#include "Canvas.h"
#include "CellStorage.h"
#include "Doc.h"
#include "Region.h"
#include "Selection.h"
#include "Sheet.h"

using namespace KSpread;

class ChartDatabaseSelector::Private
{
public:
    Doc* doc;
    Selection* selection;
    KoChart::ChartInterface* shape;
    Ui::ChartDatabaseSelector widget;
};

ChartDatabaseSelector::ChartDatabaseSelector( Doc* doc )
    : KoShapeConfigWidgetBase()
    , d( new Private )
{
    d->doc = doc;
    d->selection = 0;
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
    const Region region(d->widget.m_cellRegion->text(), d->doc->map(), d->selection->activeSheet());
    if (!region.isValid() || !region.isContiguous())
        return;
    Binding binding(region);
    d->shape->setModel(binding.model());
    d->shape->setFirstRowIsLabel( d->widget.m_firstRowAsLabel->isChecked() );
    d->shape->setFirstColumnIsLabel( d->widget.m_firstColumnAsLabel->isChecked() );
    region.firstSheet()->cellStorage()->setBinding(region, binding);
}

KAction* ChartDatabaseSelector::createAction()
{
    return 0;
}

void ChartDatabaseSelector::showEvent( QShowEvent* event )
{
    Q_UNUSED( event );
    Q_ASSERT( m_resourceProvider );
    d->selection = static_cast<Selection*>( m_resourceProvider->resource( Canvas::Selection ).value<void*>() );
    d->widget.m_cellRegion->setText( d->selection->Region::name() );
}


#include "ChartDatabaseSelector.moc"
