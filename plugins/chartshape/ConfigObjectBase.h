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


#ifndef KOCHART_CONFIGOBJECTBASE
#define KOCHART_CONFIGOBJECTBASE


#include "ChartShape.h"

#include <QWidget>

namespace KoChart
{

class ConfigObjectBase : public QWidget
{
    Q_OBJECT
public:
    ConfigObjectBase(QWidget *parent = 0);

    ConfigObjectBase(QList<ChartType> types, QWidget *parent = 0);

    ~ConfigObjectBase();

    void setChartTypes(QList<ChartType> types);

    void open(ChartShape *shape);
    // reimplement to update the ui
    virtual void updateData(ChartType type, ChartSubtype subtype);

    void blockSignals(bool block);

    void blockSignals(QWidget *w, bool block);

public:
    ChartShape *chart;
    QList<ChartType> chartTypes;
};

}  // namespace KoChart


#endif // KOCHART_CONFIGOBJECTBASE
