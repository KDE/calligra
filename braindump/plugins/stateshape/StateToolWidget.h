/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _STATETOOLWIDGET_H_
#define _STATETOOLWIDGET_H_

#include <QWidget>

#include "ui_StateShapeConfigWidget.h"

class KUndo2Command;
class KCategorizedSortFilterProxyModel;
class StatesModel;
class StateShape;
class StateTool;

class StateToolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StateToolWidget(StateTool*);
public Q_SLOTS:
    /// reimplemented
    virtual void open(StateShape *shape);
    /// reimplemented
    virtual void save();
private:
    /// reimplemented
    virtual KUndo2Command * createCommand();
private:
    void blockChildSignals(bool block);
private:
    StateTool* m_tool;
    StateShape *m_shape;
    StatesModel* m_model;
    KCategorizedSortFilterProxyModel* m_proxyModel;
    Ui::StateShapeConfigWidget m_widget;
};

#endif
