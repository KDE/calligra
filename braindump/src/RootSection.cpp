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

#include <KoUndoStack.h>

#include "Section.h"
#include "SectionsIO.h"

#include "ViewManager.h"

#include <KDebug>

RootSection::RootSection() : SectionGroup(0), m_undoStack(new KoUndoStack(0)), m_viewManager(new ViewManager(this)), m_sectionsSaver(new SectionsIO(this))
{
  connect(m_undoStack, SIGNAL(indexChanged(int)), SIGNAL(commandExecuted()));
}

RootSection::~RootSection()
{
  delete m_undoStack;
}

ViewManager* RootSection::viewManager()
{
  return m_viewManager;
}

SectionsIO* RootSection::sectionsIO()
{
  return m_sectionsSaver;
}

void RootSection::addCommand(Section* _section, QUndoCommand* _command)
{
  m_undoStack->push(_command);
  m_sectionsSaver->push(_section);
}

void RootSection::createActions(KActionCollection* _actionCollection) {
  m_undoStack->createUndoAction(_actionCollection);
  m_undoStack->createRedoAction(_actionCollection);
}

KoUndoStack* RootSection::undoStack()
{
  return m_undoStack;
}

#include "RootSection.moc"
