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

#include "Section.h"

#include "SectionContainer.h"
#include "LayoutFactoryRegistry.h"
#include "Layout.h"

Section::Section(RootSection* _rootSection) : SectionGroup(0), m_layout(LayoutFactoryRegistry::instance()->createLayout("freelayout")), m_sectionContainer(new SectionContainer(this, _rootSection))
{
}

Section::Section(const Section& _rhs) : SectionGroup(_rhs), m_layout(LayoutFactoryRegistry::instance()->createLayout(_rhs.m_layout->id())), m_sectionContainer(new SectionContainer(*_rhs.m_sectionContainer, this))
{
    setName(_rhs.name());
}

Section::~Section()
{
    delete m_sectionContainer;
}

SectionContainer* Section::sectionContainer()
{
    return m_sectionContainer;
}

const QString& Section::name() const
{
    return m_name;
}

void Section::setName(const QString& _name)
{
    m_name = _name;
}

Layout* Section::layout()
{
    return m_layout;
}

void Section::setLayout(Layout* layout)
{
    layout->replaceLayout(m_layout);
    delete m_layout;
    m_layout = layout;
}
