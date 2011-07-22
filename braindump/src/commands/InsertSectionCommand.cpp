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

#include "InsertSectionCommand.h"
#include "DocumentModel.h"
#include "Section.h"
#include <SectionsIO.h>

InsertSectionCommand::InsertSectionCommand(SectionsIO* _sectionIO, Section* _section, SectionGroup* _parent, DocumentModel* _model, Section* _above) : m_sectionIO(_sectionIO), m_section(_section), m_parent(_parent), m_model(_model), m_above(_above), m_idx(-1)
{
    Q_ASSERT(_above == 0 || _parent == _above->sectionParent());
}

InsertSectionCommand::InsertSectionCommand(SectionsIO* _sectionIO, Section* _section, SectionGroup* _parent, DocumentModel* _model, int _idx) : m_sectionIO(_sectionIO), m_section(_section), m_parent(_parent), m_model(_model), m_above(0), m_idx(_idx)
{
    Q_ASSERT(_idx >= 0 && _idx <= _parent->sections().count());
}

void InsertSectionCommand::undo()
{
    m_model->removeSection(m_section);
}

void InsertSectionCommand::redo()
{
    m_sectionIO->push(m_section, SectionsIO::RecursivePush);
    if(m_above || m_idx == -1) {
        m_model->insertSection(m_section, m_parent, m_above);
    } else {
        m_model->insertSection(m_section, m_parent, m_idx);
    }
}
