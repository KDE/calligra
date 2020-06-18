/* This file is part of the KDE project
 * Copyright (C) 2020 Dag Andersen <dag.andersen@kdemail.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef TABLESHAPECONFIGWIDGET_H
#define TABLESHAPECONFIGWIDGET_H

#include <KoShapeConfigWidgetBase.h>
#include <ui_TableShapeConfigWidget.h>


namespace Calligra
{
namespace Sheets
{

class TableShape;

class TableShapeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    TableShapeConfigWidget();
    ~TableShapeConfigWidget() override;

    /// reimplemented from KoShapeConfigWidgetBase
    void open(KoShape *shape) override;
    /// reimplemented from KoShapeConfigWidgetBase
    void save() override;
    /// reimplemented from KoShapeConfigWidgetBase
    bool showOnShapeCreate() override;
    /// reimplemented from KoShapeConfigWidgetBase
    bool showOnShapeSelect() override;

private Q_SLOTS:
    void documentLoaded();

private:
    TableShape *m_shape;
    Ui::TableShapeConfigWidget ui;
};


}
}
#endif
