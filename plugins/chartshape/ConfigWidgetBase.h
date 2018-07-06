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


#ifndef KOCHART_CONFIGWIDGETBASE
#define KOCHART_CONFIGWIDGETBASE


#include <KoShapeConfigWidgetBase.h>

#include "ChartShape.h"

#include <QWidget>

namespace KoChart
{

class ConfigWidgetBase : public KoShapeConfigWidgetBase
{
public:
    ConfigWidgetBase() {}
    ~ConfigWidgetBase() {}

    // reimplemented from KoShapeConfigWidgetBase
    void open(KoShape *shape) {
        chart = dynamic_cast<ChartShape*>(shape);
        if (!chart) {
            chart = dynamic_cast<ChartShape*>(shape->parent());
            if (!chart) {
                return;
            }
        }
        if (chart) {
            updateData();
        }
    }
    // reimplemented from KoShapeConfigWidgetBase
    void save() { Q_ASSERT(false); }

    // reimplement to update the ui
    virtual void updateData() {}
    // reimplement if you open any dialogs
    virtual void deleteSubDialogs(ChartType type = LastChartType) {Q_UNUSED(type)}

    void blockSignals(bool block) {
        blockSignals(this, block);
    }
    void blockSignals(QWidget *w, bool block) {
        QList<QWidget*> lst = w->findChildren<QWidget*>();
        for (int i = 0; i < lst.count(); ++i) {
            lst.at(i)->blockSignals(block);
        }
    }

public:
    ChartShape *chart;
};

}  // namespace KoChart


#endif // KOCHART_CONFIGWIDGETBASE
