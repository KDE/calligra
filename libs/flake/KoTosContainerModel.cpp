/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoTosContainerModel.h"

#include "KoTextShapeDataBase.h"
#include "KoTosContainer.h"

#include <FlakeDebug.h>
#include <QSizeF>

KoTosContainerModel::KoTosContainerModel()
    : m_textShape(nullptr)
{
}

KoTosContainerModel::~KoTosContainerModel() = default;

void KoTosContainerModel::add(KoShape *shape)
{
    // make sure shape is a text shape
    KoTextShapeDataBase *shapeData = qobject_cast<KoTextShapeDataBase *>(shape->userData());
    Q_ASSERT(shapeData != nullptr);
    if (shapeData) {
        m_textShape = shape;
    }
}

void KoTosContainerModel::remove(KoShape *shape)
{
    Q_ASSERT(m_textShape == nullptr || shape == m_textShape);
    if (shape == m_textShape) {
        m_textShape = nullptr;
    }
}

void KoTosContainerModel::setClipped(const KoShape *shape, bool clipping)
{
    Q_UNUSED(shape);
    Q_UNUSED(clipping);
}

bool KoTosContainerModel::isClipped(const KoShape *shape) const
{
    Q_UNUSED(shape)
    return false;
}

void KoTosContainerModel::setInheritsTransform(const KoShape *shape, bool inherit)
{
    Q_UNUSED(shape);
    Q_UNUSED(inherit);
}

bool KoTosContainerModel::inheritsTransform(const KoShape *shape) const
{
    Q_UNUSED(shape)
    return true;
}

bool KoTosContainerModel::isChildLocked(const KoShape *child) const
{
    Q_ASSERT(child == m_textShape);
    Q_ASSERT(child->parent());
    // TODO do we need to guard this?
    return child->isGeometryProtected() || child->parent()->isGeometryProtected();
}

int KoTosContainerModel::count() const
{
    return m_textShape != nullptr ? 1 : 0;
}

QList<KoShape *> KoTosContainerModel::shapes() const
{
    QList<KoShape *> shapes;
    if (m_textShape) {
        shapes << m_textShape;
    }
    return shapes;
}

void KoTosContainerModel::containerChanged(KoShapeContainer *container, KoShape::ChangeType type)
{
    debugFlake << "change type:" << type << KoShape::SizeChanged << KoShape::ContentChanged;
    if (type != KoShape::SizeChanged && type != KoShape::ContentChanged) {
        return;
    }
    KoTosContainer *tosContainer = dynamic_cast<KoTosContainer *>(container);
    debugFlake << "tosContainer" << tosContainer;
    if (tosContainer) {
        debugFlake << "behaviour" << tosContainer->resizeBehavior() << KoTosContainer::TextFollowsPreferredTextRect;
    }
    if (m_textShape && tosContainer && tosContainer->resizeBehavior() != KoTosContainer::TextFollowsPreferredTextRect) {
        debugFlake << "change type setSize";
        m_textShape->setSize(tosContainer->size());
    }
}
