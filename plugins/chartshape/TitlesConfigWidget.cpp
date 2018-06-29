/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009      Inge Wallin    <inge@lysator.liu.se>
   Copyright 2018 Dag Andersen <danders@get2net.dk>

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

// Own
#include "TitlesConfigWidget.h"

// KoChart
#include "ChartDebug.h"

using namespace KoChart;



TitlesConfigWidget::TitlesConfigWidget()
{
    setObjectName("TitlesConfigWdget");
    ui.setupUi(this);
}

TitlesConfigWidget::~TitlesConfigWidget()
{
}

void TitlesConfigWidget::updateData()
{
    if (!chart) {
        return;
    }
    blockSignals(true);

    ui.showTitle->setChecked(chart->title()->isVisible());
    ui.titlePositioning->setCurrentIndex(chart->title()->additionalStyleAttribute("chart:auto-position") == "true" ? 0 : 1);

    ui.showSubTitle->setChecked(chart->subTitle()->isVisible());
    ui.subtitlePositioning->setCurrentIndex(chart->subTitle()->additionalStyleAttribute("chart:auto-position") == "true" ? 0 : 1);

    ui.showFooter->setChecked(chart->footer()->isVisible());
    ui.footerPositioning->setCurrentIndex(chart->footer()->additionalStyleAttribute("chart:auto-position") == "true" ? 0 : 1);

    blockSignals(false);
}



