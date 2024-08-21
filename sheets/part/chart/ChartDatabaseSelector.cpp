/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ChartDatabaseSelector.h"

#include "ui_ChartDatabaseSelector.h"

#include "KoCanvasResourceIdentities.h"
#include "KoShape.h"

#include "KoChartInterface.h"

#include "core/Map.h"
#include "core/Sheet.h"
#include "core/SheetAccessModel.h"
#include "odf/SheetsOdf.h"
#include "ui/Selection.h"

using namespace Calligra::Sheets;

class ChartDatabaseSelector::Private
{
public:
    Map *map;
    Selection *selection;
    KoChart::ChartInterface *shape;
    Ui::ChartDatabaseSelector widget;
};

ChartDatabaseSelector::ChartDatabaseSelector(Map *map)
    : KoShapeConfigWidgetBase()
    , d(new Private)
{
    d->map = map;
    d->selection = nullptr;
    d->shape = nullptr;
    d->widget.setupUi(this);
}

ChartDatabaseSelector::~ChartDatabaseSelector()
{
    delete d;
}

void ChartDatabaseSelector::open(KoShape *shape)
{
    QObject *const object = dynamic_cast<QObject *>(shape);
    Q_ASSERT(object);
    if (!object) {
        return;
    }
    d->shape = qobject_cast<KoChart::ChartInterface *>(object);
    Q_ASSERT(d->shape);
}

void ChartDatabaseSelector::save()
{
    Sheet *sheet = d->selection->activeSheet();
    const Region selectedRegion = d->map->regionFromName(d->widget.m_cellRegion->text(), sheet);
    if (!selectedRegion.isValid())
        return;

    d->shape->setSheetAccessModel(sheet->fullMap()->sheetAccessModel());
    d->shape->reset(Odf::saveRegion(selectedRegion.name()),
                    d->widget.m_firstRowAsLabel->isChecked(),
                    d->widget.m_firstColumnAsLabel->isChecked(),
                    d->widget.m_dataInRows->isChecked() ? Qt::Horizontal : Qt::Vertical);
}

void ChartDatabaseSelector::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    Q_ASSERT(m_resourceManager);
    d->selection = static_cast<Selection *>(m_resourceManager->resource(::Sheets::CanvasResource::Selection).value<void *>());
    d->widget.m_cellRegion->setText(d->selection->Region::name());
}
