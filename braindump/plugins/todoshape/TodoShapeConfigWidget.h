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

#include <KoShapeConfigWidgetBase.h>

#include "ui_TodoShapeConfigWidget.h"

class KCategorizedSortFilterProxyModel;
class StatesModel;
class TodoShape;

class TodoShapeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
  public:
    TodoShapeConfigWidget();
    /// reimplemented
    virtual void open(KoShape *shape);
    /// reimplemented
    virtual void save();
    /// reimplemented
    virtual bool showOnShapeCreate() { return false; }
    /// reimplemented
    virtual QUndoCommand * createCommand();
  private:
    void blockChildSignals( bool block );
  private:
    TodoShape *m_shape;
    StatesModel* m_model;
    KCategorizedSortFilterProxyModel* m_proxyModel;
    Ui::TodoShapeConfigWidget m_widget;
};
