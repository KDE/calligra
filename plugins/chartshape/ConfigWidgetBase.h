/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_CONFIGWIDGETBASE
#define KOCHART_CONFIGWIDGETBASE

#include <KoShapeConfigWidgetBase.h>

#include "ChartShape.h"
#include "ConfigSubWidgetBase.h"

#include <QWidget>

#include "ChartDebug.h"

namespace KoChart
{

class ConfigWidgetBase : public KoShapeConfigWidgetBase
{
public:
    ConfigWidgetBase() = default;
    ~ConfigWidgetBase() = default;

    /// Calling open() with @p shape will call deactivate()
    /// reimplemented from KoShapeConfigWidgetBase
    void open(KoShape *shape) override
    {
        if (!shape) {
            deactivate();
            return;
        }
        chart = dynamic_cast<ChartShape *>(shape);
        if (!chart) {
            chart = dynamic_cast<ChartShape *>(shape->parent());
            if (!chart) {
                deactivate();
                return;
            }
        }
        if (chart) {
            connect(chart, &ChartShape::chartTypeChanged, this, &ConfigWidgetBase::removeSubDialogs);
        }
    }
    virtual void deactivate()
    {
        for (ConfigSubWidgetBase *w : findChildren<ConfigSubWidgetBase *>()) {
            w->deactivate();
        }
        if (chart) {
            deleteSubDialogs();
            disconnect(chart, &ChartShape::chartTypeChanged, this, &ConfigWidgetBase::removeSubDialogs);
        }
        chart = nullptr;
    }

    /// reimplemented from KoShapeConfigWidgetBase
    void save() override
    {
        Q_ASSERT(false);
    }

    /// Reimplement to update the ui
    virtual void updateData()
    {
    }

    /// Reimplement if you open any dialogs
    /// This is called from close()
    virtual void deleteSubDialogs(KoChart::ChartType type = LastChartType)
    {
        Q_UNUSED(type)
    }

    void blockSignals(bool block)
    {
        blockSignals(this, block);
    }
    void blockSignals(QWidget *w, bool block)
    {
        QList<QWidget *> lst = w->findChildren<QWidget *>();
        for (int i = 0; i < lst.count(); ++i) {
            lst.at(i)->blockSignals(block);
        }
    }

public:
    ChartShape *chart;

private Q_SLOTS:
    void removeSubDialogs(KoChart::ChartType type, KoChart::ChartType prev = LastChartType)
    {
        if (type != prev) {
            deleteSubDialogs();
        }
    }
};

} // namespace KoChart

#endif // KOCHART_CONFIGWIDGETBASE
