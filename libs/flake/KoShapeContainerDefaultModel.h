/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECONTAINERDEFAULTMODEL_H
#define KOSHAPECONTAINERDEFAULTMODEL_H

#include "KoShapeContainerModel.h"

#include "flake_export.h"

/**
 * A default implementation of the KoShapeContainerModel.
 */
class FLAKE_EXPORT KoShapeContainerDefaultModel : public KoShapeContainerModel
{
public:
    KoShapeContainerDefaultModel();
    ~KoShapeContainerDefaultModel() override;

    void add(KoShape *shape) override;

    void proposeMove(KoShape *shape, QPointF &move) override;

    void setClipped(const KoShape *shape, bool clipping) override;

    bool isClipped(const KoShape *shape) const override;

    void setInheritsTransform(const KoShape *shape, bool inherit) override;

    bool inheritsTransform(const KoShape *shape) const override;

    void remove(KoShape *shape) override;

    int count() const override;

    QList<KoShape *> shapes() const override;

    bool isChildLocked(const KoShape *child) const override;

    /// empty implementation.
    void containerChanged(KoShapeContainer *container, KoShape::ChangeType type) override;

private:
    class Private;
    Private *const d;
};

#endif
