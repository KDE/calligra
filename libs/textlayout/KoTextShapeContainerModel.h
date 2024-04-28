/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOTEXTSHAPECONTAINERMODEL_H
#define KOTEXTSHAPECONTAINERMODEL_H

#include <KoShapeContainerModel.h>

#include "kotextlayout_export.h"

class KoShapeAnchor;
class KoShapeContainer;

/**
 *  A model to position children of the text shape.
 * All anchored frames are children of the text shape, and they get positioned
 * by the text layouter.
 */
class KOTEXTLAYOUT_EXPORT KoTextShapeContainerModel : public KoShapeContainerModel
{
public:
    /// constructor
    KoTextShapeContainerModel();
    ~KoTextShapeContainerModel() override;

    /// reimplemented from KoShapeContainerModel
    void add(KoShape *child) override;
    /// reimplemented from KoShapeContainerModel
    void remove(KoShape *child) override;
    /// reimplemented from KoShapeContainerModel
    void setClipped(const KoShape *child, bool clipping) override;
    /// reimplemented from KoShapeContainerModel
    bool isClipped(const KoShape *child) const override;
    /// reimplemented from KoShapeContainerModel
    int count() const override;
    /// reimplemented from KoShapeContainerModel
    QList<KoShape *> shapes() const override;
    /// reimplemented from KoShapeContainerModel
    void containerChanged(KoShapeContainer *container, KoShape::ChangeType type) override;
    /// reimplemented from KoShapeContainerModel
    void proposeMove(KoShape *child, QPointF &move) override;
    /// reimplemented from KoShapeContainerModel
    void childChanged(KoShape *child, KoShape::ChangeType type) override;
    /// reimplemented from KoShapeContainerModel
    bool isChildLocked(const KoShape *child) const override;
    /// reimplemented from KoShapeContainerModel
    void setInheritsTransform(const KoShape *shape, bool inherit) override;
    /// reimplemented from KoShapeContainerModel
    bool inheritsTransform(const KoShape *shape) const override;

    /// each child that is added due to being anchored in the text has an anchor; register it for rules based placement.
    void addAnchor(KoShapeAnchor *anchor);
    /// When a shape is removed or stops being anchored, remove it.
    void removeAnchor(KoShapeAnchor *anchor);

private:
    // reset child position and relayout shape to which this shape is linked
    void relayoutInlineObject(KoShape *child);

    class Private;
    Private *const d;
};

#endif
