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

#include "KoResourceManager.h"
#include "KoShape.h"

#include "KoChartInterface.h"

#include "Binding.h"
#include "CanvasResources.h"
#include "CellStorage.h"
#include "Region.h"
#include "ui/Selection.h"
#include "Sheet.h"

using namespace KSpread;

class ChartDatabaseSelector::Private
{
public:
    Map* map;
    Selection* selection;
    KoChart::ChartInterface* shape;
    Ui::ChartDatabaseSelector widget;
};

ChartDatabaseSelector::ChartDatabaseSelector(Map *map)
        : KoShapeConfigWidgetBase()
        , d(new Private)
{
    d->map = map;
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
    QObject* const object = dynamic_cast<QObject*>(shape);
    Q_ASSERT(object);
    if (!object) {
        return;
    }
    d->shape = qobject_cast<KoChart::ChartInterface*>(object);
    Q_ASSERT(d->shape);
}

void ChartDatabaseSelector::save()
{
    // This region contains the entire sheet
    const Region region(1, 1, KS_colMax, KS_rowMax, d->selection->activeSheet());
    // The region to be displayed in the chart
    const Region selectedRegion(d->widget.m_cellRegion->text(), d->map, d->selection->activeSheet());
    if (!region.isValid() || !region.isContiguous())
        return;
    Binding binding(region);
    d->shape->setModel(binding.model(), selectedRegion.rects());
    d->shape->setFirstRowIsLabel(d->widget.m_firstRowAsLabel->isChecked());
    d->shape->setFirstColumnIsLabel(d->widget.m_firstColumnAsLabel->isChecked());
    d->shape->setDataDirection(d->widget.m_dataInRows->isChecked() ? Qt::Horizontal : Qt::Vertical);
    region.firstSheet()->cellStorage()->setBinding(region, binding);
}

KAction* ChartDatabaseSelector::createAction()
{
    return 0;
}

void ChartDatabaseSelector::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    Q_ASSERT(m_resourceManager);
    d->selection = static_cast<Selection*>(m_resourceManager->resource(CanvasResource::Selection).value<void*>());
    d->widget.m_cellRegion->setText(d->selection->Region::name());
}


#include "ChartDatabaseSelector.moc"
