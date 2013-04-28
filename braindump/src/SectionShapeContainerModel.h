/*
 *  Copyright (c) 2006-2007 Thomas Zander <zander@kde.org>
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
#ifndef SECTIONSHAPECONTAINERMODEL_H
#define SECTIONSHAPECONTAINERMODEL_H

#include "KoShapeContainerModel.h"

#include <Section.h>

class SectionShapeContainerModel: public KoShapeContainerModel
{
public:
    explicit SectionShapeContainerModel(Section* _section);
    ~SectionShapeContainerModel();
    void add(KoShape *child);
    void setClipped(const KoShape *, bool);
    bool isClipped(const KoShape *) const;
    void setInheritsTransform(const KoShape *, bool);
    bool inheritsTransform(const KoShape *) const;
    void remove(KoShape *child);
    int count() const;
    QList<KoShape*> shapes() const;
    void containerChanged(KoShapeContainer *, KoShape::ChangeType type);
    void childChanged(KoShape *, KoShape::ChangeType);
    bool isChildLocked(const KoShape *child) const;
public:
    /**
     * Determine whether to update or not the layout.
     */
    void setUpdateLayout(bool v);

private: // members
    QList <KoShape *> m_members;
    Section* m_section;
    bool m_updateLayout;
};

#endif
