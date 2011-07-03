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
#include "StateShapeChangeStateCommand.h"
#include "StateShape.h"

StateShapeChangeStateCommand::StateShapeChangeStateCommand(StateShape* _shape, const QString& _newCategoryId, const QString& _newStateId) : m_shape(_shape), m_newCategoryId(_newCategoryId), m_newStateId(_newStateId), m_oldCategoryId(m_shape->categoryId()), m_oldStateId(m_shape->stateId())
{

}

void StateShapeChangeStateCommand::undo()
{
    m_shape->setCategoryId(m_oldCategoryId);
    m_shape->setStateId(m_oldStateId);
}

void StateShapeChangeStateCommand::redo()
{
    m_shape->setCategoryId(m_newCategoryId);
    m_shape->setStateId(m_newStateId);
}
