/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ConfigSubWidgetBase.h"

using namespace KoChart;

ConfigSubWidgetBase::ConfigSubWidgetBase(QWidget *parent)
    : QWidget(parent)
    , chart(nullptr)
{
}

ConfigSubWidgetBase::ConfigSubWidgetBase(QList<ChartType> types, QWidget *parent)
    : QWidget(parent)
    , chart(nullptr)
    , chartTypes(types)
{
}

ConfigSubWidgetBase::~ConfigSubWidgetBase() = default;

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
    QList<QWidget *> lst = w->findChildren<QWidget *>();
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
