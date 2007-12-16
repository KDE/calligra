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

#include <KDChartChart>
#include <KDChartAbstractDiagram>
#include <KDChartAbstractCoordinatePlane>


using namespace KChart;

class ChartDatasetConfigWidget::Private
{
public:
    Private();
    ~Private();

    ChartShape                  *chart;
    Ui::ChartDatasetConfigWidget  ui;
    int                           dataset;
};

ChartDatasetConfigWidget::Private::Private()
{
    chart   = 0;
    dataset = -1;
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
    connect( d->ui.color, SIGNAL( changed( const QColor& ) ),
             this,        SLOT( colorChanged( const QColor& ) ) );
}

void ChartDatasetConfigWidget::save()
{
}

void ChartDatasetConfigWidget::selectDataset( int dataset )
{
    if ( dataset >= 0 ) {
        d->ui.note->hide();
        d->ui.color->setEnabled( true );
        d->ui.color->blockSignals( true );
        if ( d->chart && d->chart->chart() && d->chart->chart()->coordinatePlane() ) {
            d->ui.color->setColor( d->chart->chart()->coordinatePlane()->diagram()->brush( dataset ).color() );
        }
        d->ui.color->blockSignals( false );
    } else {
        d->ui.note->show();
        d->ui.color->setEnabled( false );
    }
    d->dataset = dataset;
}

void ChartDatasetConfigWidget::colorChanged( const QColor& color )
{
    emit datasetColorChanged( d->dataset, color );
}

KAction* ChartDatasetConfigWidget::createAction()
{
    return 0;
}

#include "ChartDatasetConfigWidget.moc"
