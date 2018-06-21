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
    : m_shape(0)
{
    setObjectName("TitlesConfigWdget");
    m_ui.setupUi(this);

    connect(m_ui.showTitle, SIGNAL(toggled(bool)), this, SIGNAL(showTitleChanged(bool)));
    connect(m_ui.showSubTitle, SIGNAL(toggled(bool)), this, SIGNAL(showSubTitleChanged(bool)));
    connect(m_ui.showFooter, SIGNAL(toggled(bool)), this, SIGNAL(showFooterChanged(bool)));

}

TitlesConfigWidget::~TitlesConfigWidget()
{
}

void TitlesConfigWidget::open(KoShape* shape)
{
    qInfo()<<Q_FUNC_INFO<<shape;
    m_shape = dynamic_cast<ChartShape*>(shape);
    if (!m_shape) {
        // a child may have been clicked
        m_shape = dynamic_cast<ChartShape*>(shape->parent());
        if (!m_shape) {
            return;
        }
    }
    updateData();
}


void TitlesConfigWidget::updateData()
{
    if (!m_shape) {
        return;
    }
    m_ui.showTitle->setChecked(m_shape->title()->isVisible());
    m_ui.showSubTitle->setChecked(m_shape->subTitle()->isVisible());
    m_ui.showFooter->setChecked(m_shape->footer()->isVisible());
}



