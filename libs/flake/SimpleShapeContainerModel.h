/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLESHAPECONTAINERMODEL_H
#define SIMPLESHAPECONTAINERMODEL_H

#include "KoShapeContainerModel.h"

/// \internal
class SimpleShapeContainerModel : public KoShapeContainerModel
{
public:
    SimpleShapeContainerModel() = default;
    ~SimpleShapeContainerModel() override = default;
    void add(KoShape *child) override
    {
        if (m_members.contains(child))
            return;
        m_members.append(child);
    }
    void setClipped(const KoShape *, bool) override
    {
    }
    bool isClipped(const KoShape *) const override
    {
        return false;
    }
    void remove(KoShape *child) override
    {
        m_members.removeAll(child);
    }
    int count() const override
    {
        return m_members.count();
    }
    QList<KoShape *> shapes() const override
    {
        return QList<KoShape *>(m_members);
    }
    void containerChanged(KoShapeContainer *, KoShape::ChangeType) override
    {
    }
    bool isChildLocked(const KoShape *child) const override
    {
        Q_ASSERT(child->parent());
        if (child->parent()) {
            return child->isGeometryProtected() || child->parent()->isGeometryProtected();
        } else {
            return child->isGeometryProtected();
        }
    }
    void setInheritsTransform(const KoShape *, bool) override
    {
    }
    bool inheritsTransform(const KoShape *) const override
    {
        return false;
    }

private: // members
    QList<KoShape *> m_members;
};

#endif
