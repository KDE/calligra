/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
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

#include "StateTool.h"

#include <QPainter>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include <State.h>
#include <StatesRegistry.h>

#include "StateCategory.h"
#include "StateShape.h"
#include "StateShapeChangeStateCommand.h"
#include "StateToolWidget.h"

StateTool::StateTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
{
}

StateTool::~StateTool() = default;

void StateTool::activate(ToolActivation /*toolActivation*/, const QSet<KoShape *> & /*shapes*/)
{
    KoSelection *selection = canvas()->shapeManager()->selection();
    const auto shapes = selection->selectedShapes();
    for (KoShape *shape : shapes) {
        m_currentShape = dynamic_cast<StateShape *>(shape);
        if (m_currentShape)
            break;
    }
    Q_EMIT shapeChanged(m_currentShape);
    if (m_currentShape == nullptr) {
        // none found
        Q_EMIT done();
        return;
    }
    useCursor(QCursor(Qt::ArrowCursor));
}
void StateTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

void StateTool::mousePressEvent(KoPointerEvent *event)
{
    StateShape *hit = nullptr;
    const QRectF roi(event->point, QSizeF(1, 1));
    const QList<KoShape *> shapes = canvas()->shapeManager()->shapesAt(roi);
    KoSelection *selection = canvas()->shapeManager()->selection();
    for (KoShape *shape : shapes) {
        hit = dynamic_cast<StateShape *>(shape);
        if (hit) {
            if (hit == m_currentShape) {
                const State *state = StatesRegistry::instance()->state(m_currentShape->categoryId(), m_currentShape->stateId());
                const State *newState = StatesRegistry::instance()->nextState(state);
                if (newState) {
                    canvas()->addCommand(new StateShapeChangeStateCommand(m_currentShape, newState->category()->id(), newState->id()));
                }
            } else {
                selection->deselectAll();
                m_currentShape = hit;
                selection->select(m_currentShape);
                emit(shapeChanged(m_currentShape));
            }
        }
    }
}

void StateTool::mouseMoveEvent(KoPointerEvent *event)
{
    event->ignore();
}

void StateTool::mouseReleaseEvent(KoPointerEvent *event)
{
    event->ignore();
}

QList<QPointer<QWidget>> StateTool::createOptionWidgets()
{
    QList<QPointer<QWidget>> widgets;
    StateToolWidget *widget = new StateToolWidget(this);
    widget->open(m_currentShape);
    widget->setWindowTitle(i18n("State tool options"));
    widgets.append(widget);
    return widgets;
}
