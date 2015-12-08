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

#include "RootSection.h"

#include <QDebug>

#include <kundo2stack.h>

#include "Section.h"
#include "SectionsIO.h"

#include "ViewManager.h"

RootSection::RootSection() : SectionGroup(0), m_undoStack(new KUndo2Stack(this)), m_viewManager(new ViewManager(this)), m_sectionsSaver(new SectionsIO(this)), m_currentSection(0)
{
    connect(m_undoStack, SIGNAL(indexChanged(int)), SIGNAL(commandExecuted()));
    connect(m_undoStack, SIGNAL(indexChanged(int)), SLOT(undoIndexChanged(int)));
    if(sections().isEmpty()) {
        newSection(0);
    }
}

RootSection::~RootSection()
{
}

ViewManager* RootSection::viewManager()
{
    return m_viewManager;
}

SectionsIO* RootSection::sectionsIO()
{
    return m_sectionsSaver;
}

void RootSection::addCommand(Section* _section, KUndo2Command* _command)
{
    qDebug() << _command << " is added for section " << _section;
    m_commandsMap[_command] = _section;
    m_undoStack->push(_command);
}

void RootSection::createActions(KActionCollection* _actionCollection)
{
    m_undoStack->createUndoAction(_actionCollection);
    m_undoStack->createRedoAction(_actionCollection);
}

KUndo2Stack* RootSection::undoStack()
{
    return m_undoStack;
}

void RootSection::undoIndexChanged(int idx)
{
    const KUndo2Command* command = m_undoStack->command(idx - 1);
    qDebug() << idx << " " << command << " " << m_undoStack->count() << " " << m_undoStack->cleanIndex() << " " << m_undoStack->index();
    Section* section = m_commandsMap[command];
    if(!section && idx == m_undoStack->count()) {
        section = m_currentSection;
        m_commandsMap[command] = section;
    }
    m_sectionsSaver->push(section);
    qDebug() << "save section: " << section;
}

void RootSection::setCurrentSection(Section* _section)
{
    m_currentSection = _section;
}
