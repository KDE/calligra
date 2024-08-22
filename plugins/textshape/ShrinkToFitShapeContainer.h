/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Sebastian Sauer <sebsauer@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SHRINKTOFITSHAPECONTAINER_H
#define SHRINKTOFITSHAPECONTAINER_H

#include <KoOdfLoadingContext.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoTextShapeData.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <QObject>
#include <QPainter>
#include <SimpleShapeContainerModel.h>

#include <KoDocumentResourceManager.h>
#include <KoShapeContainer_p.h>
#include <KoShapeLoadingContext.h>
#include <KoTextDocumentLayout.h>

/**
 * \internal d-pointer class for the \a ShrinkToFitShapeContainer class.
 */
class ShrinkToFitShapeContainerPrivate : public KoShapeContainerPrivate
{
public:
    explicit ShrinkToFitShapeContainerPrivate(KoShapeContainer *q, KoShape *childShape)
        : KoShapeContainerPrivate(q)
        , childShape(childShape)
    {
    }
    ~ShrinkToFitShapeContainerPrivate() override = default;
    KoShape *childShape; // the original shape not owned by us
};

/**
 * Container that is used to wrap a shape and shrink a text-shape to fit the content.
 */
class ShrinkToFitShapeContainer : public KoShapeContainer
{
public:
    explicit ShrinkToFitShapeContainer(KoShape *childShape, KoDocumentResourceManager *documentResources = nullptr);
    ~ShrinkToFitShapeContainer() override;

    // reimplemented
    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    // reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    // reimplemented
    void saveOdf(KoShapeSavingContext &context) const override;

    /**
     * Factory function to create and return a ShrinkToFitShapeContainer instance that wraps the \a shape with it.
     */
    static ShrinkToFitShapeContainer *wrapShape(KoShape *shape, KoDocumentResourceManager *documentResourceManager = nullptr);

    /**
     * Try to load text-on-shape from \a element and wrap \a shape with it.
     */
    static void tryWrapShape(KoShape *shape, const KoXmlElement &element, KoShapeLoadingContext &context);

    /**
     * Undo the wrapping done in the \a wrapShape method.
     */
    void unwrapShape(KoShape *shape);

private:
    Q_DECLARE_PRIVATE(ShrinkToFitShapeContainer)
};

/**
 * The container-model class implements \a KoShapeContainerModel for the \a ShrinkToFitShapeContainer to
 * to stuff once our container changes.
 */
class ShrinkToFitShapeContainerModel : public QObject, public SimpleShapeContainerModel
{
    Q_OBJECT
    friend class ShrinkToFitShapeContainer;

public:
    ShrinkToFitShapeContainerModel(ShrinkToFitShapeContainer *q, ShrinkToFitShapeContainerPrivate *d);

    // reimplemented
    void containerChanged(KoShapeContainer *container, KoShape::ChangeType type) override;
    // reimplemented
    bool inheritsTransform(const KoShape *child) const override;
    // reimplemented
    bool isChildLocked(const KoShape *child) const override;
    // reimplemented
    bool isClipped(const KoShape *child) const override;

private Q_SLOTS:
    void finishedLayout();

private:
    ShrinkToFitShapeContainer *q;
    ShrinkToFitShapeContainerPrivate *d;
    qreal m_scale;
    QSizeF m_shapeSize, m_documentSize;
    int m_dirty;
    bool m_maybeUpdate;
};

#endif
