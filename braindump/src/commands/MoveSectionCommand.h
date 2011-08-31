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

#ifndef _MOVE_SECTION_COMMAND_H_
#define _MOVE_SECTION_COMMAND_H_

#include <kundo2command.h>

class DocumentModel;
class Section;
class SectionGroup;

class MoveSectionCommand : public KUndo2Command
{
public:
    MoveSectionCommand(Section* _section, SectionGroup* _parent, DocumentModel* _model, Section* _above);
    MoveSectionCommand(Section* _section, SectionGroup* _parent, DocumentModel* _model, int _idx);
    virtual void undo();
    virtual void redo();
private:
    Section* m_section;
    SectionGroup* m_parent;
    SectionGroup* m_previousParent;
    DocumentModel* m_model;
    Section* m_above;
    int m_idx;
    int m_previousIndex;
};

#endif
