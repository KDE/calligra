/* This file is part of the KDE project

   Copyright 2007 Inge Wallin   <inge@lysator.liu.se>

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

#include "ChartDataConfigWidget.h"

#include "KoCanvasResourceProvider.h"

#include "ui_ChartDataConfigWidget.h"
#include "ChartShape.h"


using namespace KChart;

class ChartDataConfigWidget::Private
{
public:
    ChartShape                *chart;
    Ui::ChartDataConfigWidget  ui;
};


ChartDataConfigWidget::ChartDataConfigWidget()
    : KoShapeConfigWidgetBase()
    , d( new Private )
{
    d->chart = 0;
    d->ui.setupUi(this);
}

ChartDataConfigWidget::~ChartDataConfigWidget()
{
    delete d;
}

void ChartDataConfigWidget::open(KoShape* chart)
{
    d->chart = dynamic_cast<ChartShape*>( chart );
}

void ChartDataConfigWidget::save()
{
    // FIXME: Add commands here.
}

KAction* ChartDataConfigWidget::createAction()
{
    return 0;
}

#include "ChartDataConfigWidget.moc"
