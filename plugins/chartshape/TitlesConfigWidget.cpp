/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
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

TitlesConfigWidget::~TitlesConfigWidget() = default;

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
