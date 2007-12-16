/* This file is part of the KDE project
 *
 * Copyright 2007 Johannes Simon <johannes.simon@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Local
#include "ChartDatasetConfigWidget.h"

// Qt
#include <QButtonGroup>

// KDE
#include <KDebug>
#include <KIconLoader>

// KOffice
#include "KoCanvasResourceProvider.h"

// KChart
#include "ui_ChartDatasetConfigWidget.h"
#include "ChartShape.h"


using namespace KChart;

class ChartDatasetConfigWidget::Private
{
public:
    Private();
    ~Private();

    ChartShape                  *chart;
    Ui::ChartDatasetConfigWidget  ui;
};

ChartDatasetConfigWidget::Private::Private()
{
}

ChartDatasetConfigWidget::Private::~Private()
{
}

ChartDatasetConfigWidget::ChartDatasetConfigWidget()
    : KoShapeConfigWidgetBase()
    , d( new Private )
{
    setupUi();
}

ChartDatasetConfigWidget::~ChartDatasetConfigWidget()
{
    delete d;
}

void ChartDatasetConfigWidget::setupUi()
{
    d->ui.setupUi( this );
}

void ChartDatasetConfigWidget::open( KoShape* chart )
{
    d->chart = dynamic_cast<ChartShape*>( chart );
}

void ChartDatasetConfigWidget::save()
{
}

KAction* ChartDatasetConfigWidget::createAction()
{
    return 0;
}

#include "ChartDatasetConfigWidget.moc"
