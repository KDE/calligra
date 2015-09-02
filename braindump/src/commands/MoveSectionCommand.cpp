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

#include "MoveSectionCommand.h"

#include "DocumentModel.h"
#include "Section.h"

MoveSectionCommand::MoveSectionCommand(Section* _section, SectionGroup* _parent, DocumentModel* _model, Section* _above) : m_section(_section), m_parent(_parent), m_previousParent(_section->sectionParent()), m_model(_model), m_above(_above), m_idx(-1), m_previousIndex(_section->sectionParent()->indexOf(_section))
{
    Q_ASSERT(_above == 0 || _parent == _above->sectionParent());
    Q_ASSERT(m_previousIndex >= 0 && m_previousIndex < m_previousParent->sections().count());
}

MoveSectionCommand::MoveSectionCommand(Section* _section, SectionGroup* _parent, DocumentModel* _model, int _idx) : m_section(_section), m_parent(_parent), m_previousParent(_section->sectionParent()), m_model(_model), m_above(0), m_idx(_idx), m_previousIndex(_section->sectionParent()->indexOf(_section))
{
    Q_ASSERT(_idx >= 0 && _idx <= _parent->sections().count());
    Q_ASSERT(m_previousIndex >= 0 && m_previousIndex < m_previousParent->sections().count());
}

void MoveSectionCommand::undo()
{
    m_model->removeSection(m_section);
    m_model->insertSection(m_section, m_previousParent, m_previousIndex);
}

void MoveSectionCommand::redo()
{
    Q_ASSERT(m_idx != -1 || m_above);
    m_model->removeSection(m_section);
    if(m_above || m_idx == -1) {
        m_model->insertSection(m_section, m_parent, m_above);
    } else {
        m_model->insertSection(m_section, m_parent, m_idx);
    }
}
