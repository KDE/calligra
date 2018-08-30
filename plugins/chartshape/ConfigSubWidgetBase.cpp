/* This file is part of the KDE project

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
 * Boston, MA 02110-1301, USA.
*/


#include "ConfigSubWidgetBase.h"

using namespace KoChart;

ConfigSubWidgetBase::ConfigSubWidgetBase(QWidget *parent)
    : QWidget(parent)
    , chart(0)
{
    
}

ConfigSubWidgetBase::ConfigSubWidgetBase(QList<ChartType> types, QWidget *parent)
    : QWidget(parent)
    , chart(0)
    , chartTypes(types)
{
    
}

ConfigSubWidgetBase::~ConfigSubWidgetBase()
{

}

void ConfigSubWidgetBase::setChartTypes(QList<ChartType> types)
{
    chartTypes = types;
}

void ConfigSubWidgetBase::open(ChartShape *shape)
{
    chart = shape;
    if (chart) {
        connect(chart, &ChartShape::chartTypeChanged, this, &ConfigSubWidgetBase::removeSubDialogs);
    }
}

void ConfigSubWidgetBase::deactivate()
{
    if (chart) {
        deleteSubDialogs();
        disconnect(chart, &ChartShape::chartTypeChanged, this, &ConfigSubWidgetBase::removeSubDialogs);
    }
}

void ConfigSubWidgetBase::deleteSubDialogs(ChartType type)
{
    Q_UNUSED(type)
}

void ConfigSubWidgetBase::updateData(ChartType type, ChartSubtype subtype)
{
    Q_UNUSED(type)
    Q_UNUSED(subtype)
}

void ConfigSubWidgetBase::blockSignals(bool block)
{
    blockSignals(this, block);
}

void ConfigSubWidgetBase::blockSignals(QWidget *w, bool block)
{
    QList<QWidget*> lst = w->findChildren<QWidget*>();
    for (int i = 0; i < lst.count(); ++i) {
        lst.at(i)->blockSignals(block);
    }
}

void ConfigSubWidgetBase::removeSubDialogs(ChartType type, ChartType prev)
{
    if (type != prev) {
        deleteSubDialogs();
    }
}
