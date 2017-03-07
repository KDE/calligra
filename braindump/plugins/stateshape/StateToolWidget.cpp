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

#include "StateToolWidget.h"

#include <QItemDelegate>

#include <kundo2command.h>
#include <kcategorizedsortfilterproxymodel.h>

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>

#include <State.h>
#include <StateCategory.h>
#include <StatesRegistry.h>
#include <StateShape.h>

#include "StateTool.h"
#include "StatesModel.h"
#include "StateShapeChangeStateCommand.h"
#include "CategorizedItemDelegate.h"

StateToolWidget::StateToolWidget(StateTool* _stateTool) : m_tool(_stateTool)
{
    m_widget.setupUi(this);
    connect(m_widget.stateComboBox, SIGNAL(activated(int)), SLOT(save()));
    connect(m_tool, SIGNAL(shapeChanged(StateShape*)), SLOT(open(StateShape*)));
    m_model = new StatesModel();
    m_proxyModel = new KCategorizedSortFilterProxyModel();
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->sort(0);
    m_proxyModel->setSortRole(StatesModel::SortRole);
    m_proxyModel->setCategorizedModel(true);
    m_widget.stateComboBox->setModel(m_proxyModel);
    m_widget.stateComboBox->setItemDelegate(new CategorizedItemDelegate(new QItemDelegate));
}

void StateToolWidget::blockChildSignals(bool block)
{
    m_widget.stateComboBox->blockSignals(block);
}

void StateToolWidget::open(StateShape *shape)
{
    m_shape = dynamic_cast<StateShape*>(shape);
    if(! m_shape)
        return;
    blockChildSignals(true);
    m_widget.stateComboBox->setCurrentIndex(
        m_proxyModel->mapFromSource(
            m_model->indexFor(m_shape->categoryId(), m_shape->stateId())).row());
    blockChildSignals(false);
}

void StateToolWidget::save()
{
    if(!m_shape)
        return;

//   QString newUrl = m_widget.urlEdit->text();
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    if(canvasController) {
        KoCanvasBase* canvas = canvasController->canvas();
        const State* state = m_model->stateAt(
                                 m_proxyModel->mapToSource(m_proxyModel->index(m_widget.stateComboBox->currentIndex(), 0, QModelIndex())).row());
        if(state->category()->id() != m_shape->categoryId() || state->id() != m_shape->stateId()) {
            canvas->addCommand(new StateShapeChangeStateCommand(m_shape, state->category()->id(), state->id()));
        }
    }
}

KUndo2Command * StateToolWidget::createCommand()
{
    save();

    return 0;
}
