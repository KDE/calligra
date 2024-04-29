/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_CONFIGSUBWIDGETBASE
#define KOCHART_CONFIGSUBWIDGETBASE

#include "ChartShape.h"

#include <QWidget>

namespace KoChart
{

class ConfigSubWidgetBase : public QWidget
{
    Q_OBJECT
public:
    ConfigSubWidgetBase(QWidget *parent = nullptr);

    ConfigSubWidgetBase(QList<ChartType> types, QWidget *parent = nullptr);

    ~ConfigSubWidgetBase();

    void setChartTypes(QList<ChartType> types);

    virtual void open(ChartShape *shape);
    virtual void deactivate();

    // reimplement to update the ui
    virtual void updateData(ChartType type, ChartSubtype subtype);

    void deleteSubDialogs(ChartType type = LastChartType);

    void blockSignals(bool block);

    void blockSignals(QWidget *w, bool block);

public:
    ChartShape *chart;
    QList<ChartType> chartTypes;

private Q_SLOTS:
    void removeSubDialogs(KoChart::ChartType type, KoChart::ChartType prev = LastChartType);
};

} // namespace KoChart

#endif // KOCHART_CONFIGSUBWIDGETBASE
