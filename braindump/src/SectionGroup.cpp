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

#include <KLocalizedString>

#include "SectionGroup.h"
#include "Section.h"
#include "RootSection.h"

int SectionGroup::s_count = 0;

SectionGroup::SectionGroup(SectionGroup* parent) : m_parent(parent)
{
}

SectionGroup::SectionGroup(const SectionGroup& _rhs) : m_parent(0)
{
    foreach(Section * section, _rhs.m_children) {
        insertSection(new Section(*section));
    }
}

SectionGroup::~SectionGroup()
{
}

void SectionGroup::insertSection(Section* page, int index)
{
    if(page->sectionParent() == this) return;
    if(page->sectionParent()) page->sectionParent()->removeSection(page);
    m_children.insert(index, page);
    page->setSectionParent(this);
}

void SectionGroup::insertSection(Section* page, Section* before)
{
    if(before == 0) {
        insertSection(page, m_children.count());
    } else {
        insertSection(page, m_children.indexOf(before));
    }
}

void SectionGroup::removeSection(Section* page)
{
    page->setSectionParent(0);
    m_children.removeAll(page);
}

QList<Section*> SectionGroup::sections() const
{
    return m_children;
}

Section* SectionGroup::newSection(Section* before)
{
    SectionGroup* root = this;
    while(root->sectionParent()) root = root->sectionParent();
    Section* section = new Section(dynamic_cast<RootSection*>(root)) ;
    insertSection(section, before);
    section->setName(nextName());
    return section;
}

QString SectionGroup::nextName()
{
    return i18n("Untitled %1", ++s_count);
}

SectionGroup* SectionGroup::sectionParent()
{
    return m_parent;
}

void SectionGroup::setSectionParent(SectionGroup* parent)
{
    m_parent = parent;
}

void SectionGroup::sectionAdded(Section* page)
{
    if(m_parent)
        m_parent->sectionAdded(page);
}

void SectionGroup::sectionRemoved(Section* page)
{
    if(m_parent)
        m_parent->sectionRemoved(page);
}

int SectionGroup::indexOf(Section* section)
{
    return m_children.indexOf(section);
}

Section* SectionGroup::nextSection(Section* section)
{
    int idx = indexOf(section);
    idx += 1;
    Q_ASSERT(idx >= 0);
    if(idx < m_children.count()) {
        return m_children[idx];
    } else {
        return 0;
    }
}
