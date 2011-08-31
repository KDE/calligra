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

#ifndef _SECTION_GROUP_H_
#define _SECTION_GROUP_H_

#include <QList>

class Section;

class SectionGroup
{
public:
    explicit SectionGroup(SectionGroup* parent);
    SectionGroup(const SectionGroup& _rhs);
    virtual ~SectionGroup();
    void insertSection(Section* page, int index);
    void insertSection(Section* page, Section* before = 0);
    void removeSection(Section* page);
    QList<Section*> sections() const;
    Section* newSection(Section* before = 0);
    Section* nextSection(Section* section);
    int indexOf(Section* section);
protected:
    virtual void sectionAdded(Section* page);
    virtual void sectionRemoved(Section* page);
public:
    /**
     * @return the parent section (if any) of that document.
     */
    SectionGroup* sectionParent();
    static QString nextName();
private:
    void setSectionParent(SectionGroup* parent);
private:
    SectionGroup* m_parent;
    QList<Section*> m_children;
    static int s_count;
};

#endif
