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

#include "ChangeLayoutCommand.h"

#include <KoShapeLayer.h>

#include <Layout.h>
#include <LayoutFactoryRegistry.h>
#include <Section.h>
#include <SectionContainer.h>

#include "RememberPositionCommand.h"

ChangeLayoutCommand::ChangeLayoutCommand(Section* _section, const QString& _newLayout) :
    m_section(_section),
    m_oldLayout(_section->layout()->id()),
    m_newLayout(_newLayout),
    m_command(new RememberPositionCommand(_section->sectionContainer()->layer()->shapes()))
{

}

ChangeLayoutCommand::~ChangeLayoutCommand()
{
    delete m_command;
}

void ChangeLayoutCommand::undo()
{
    m_section->setLayout(LayoutFactoryRegistry::instance()->createLayout(m_oldLayout));
    m_command->undo();
}

void ChangeLayoutCommand::redo()
{
    m_section->setLayout(LayoutFactoryRegistry::instance()->createLayout(m_newLayout));
}
