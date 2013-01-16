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
#include "SectionShapeContainerModel.h"

#include <KoShapeContainer.h>
#include "Layout.h"

SectionShapeContainerModel::SectionShapeContainerModel(Section* _section) : m_section(_section), m_updateLayout(true)
{
}

SectionShapeContainerModel::~SectionShapeContainerModel()
{
}

void SectionShapeContainerModel::add(KoShape *child)
{
    if(m_members.contains(child))
        return;
    m_members.append(child);
    if(m_updateLayout) {
        m_section->layout()->addShape(child);
    }
}

void SectionShapeContainerModel::setClipped(const KoShape *, bool)
{
}

bool SectionShapeContainerModel::isClipped(const KoShape *) const
{
    return false;
}

void SectionShapeContainerModel::setInheritsTransform(const KoShape *, bool)
{
}

bool SectionShapeContainerModel::inheritsTransform(const KoShape *) const
{
    return false;
}

void SectionShapeContainerModel::remove(KoShape *child)
{
    m_members.removeAll(child);
    if(m_updateLayout) {
        m_section->layout()->removeShape(child);
    }
}

int SectionShapeContainerModel::count() const
{
    return m_members.count();
}

QList<KoShape*> SectionShapeContainerModel::shapes() const
{
    return QList<KoShape*>(m_members);
}

void SectionShapeContainerModel::containerChanged(KoShapeContainer *, KoShape::ChangeType)
{
}

void SectionShapeContainerModel::childChanged(KoShape *, KoShape::ChangeType)
{
}

bool SectionShapeContainerModel::isChildLocked(const KoShape *child) const
{
    Q_ASSERT(child->parent());
    return child->isGeometryProtected() || child->parent()->isGeometryProtected();
}

void SectionShapeContainerModel::setUpdateLayout(bool v)
{
    m_updateLayout = v;
}
