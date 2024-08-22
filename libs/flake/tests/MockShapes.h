/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef MOCKSHAPES_H
#define MOCKSHAPES_H

#include "FlakeDebug.h"
#include "KoShapeManager.h"
#include "KoSnapData.h"
#include "KoUnit.h"
#include <KoCanvasBase.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoShapeContainerModel.h>
#include <KoShapeGroup.h>
#include <QPainter>

class MockShape : public KoShape
{
public:
    MockShape()
        : paintedCount(0)
    {
    }
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &) override
    {
        Q_UNUSED(painter);
        Q_UNUSED(converter);
        // qDebug() << "Shape" << kBacktrace( 10 );
        paintedCount++;
    }
    void saveOdf(KoShapeSavingContext &) const override
    {
    }
    bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) override
    {
        return true;
    }
    int paintedCount;
};

class MockContainer : public KoShapeContainer
{
public:
    MockContainer(KoShapeContainerModel *model = nullptr)
        : KoShapeContainer(model)
        , paintedCount(0)
    {
    }
    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &) override
    {
        Q_UNUSED(painter);
        Q_UNUSED(converter);
        // qDebug() << "Container:" << kBacktrace( 10 );
        paintedCount++;
    }

    void saveOdf(KoShapeSavingContext &) const override
    {
    }
    bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) override
    {
        return true;
    }
    int paintedCount;
};

class MockGroup : public KoShapeGroup
{
    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &) override
    {
        Q_UNUSED(painter);
        Q_UNUSED(converter);
    }
};

class KoToolProxy;

class MockCanvas : public KoCanvasBase
{
public:
    MockCanvas(KoShapeBasedDocumentBase *aKoShapeBasedDocumentBase = nullptr) // made for TestSnapStrategy.cpp
        : KoCanvasBase(aKoShapeBasedDocumentBase)
        , m_shapeManager(new KoShapeManager(this))
        , m_guideData(nullptr)
    {
    }
    ~MockCanvas() override = default;
    void setHorz(qreal pHorz)
    {
        m_horz = pHorz;
    }
    void setVert(qreal pVert)
    {
        m_vert = pVert;
    }
    void setGuidesData(KoGuidesData *pGuideData)
    {
        m_guideData = pGuideData;
    }
    void gridSize(qreal *horizontal, qreal *vertical) const override
    {
        *horizontal = m_horz;
        *vertical = m_vert;
    }
    bool snapToGrid() const override
    {
        return true;
    }
    void addCommand(KUndo2Command *) override
    {
    }
    KoShapeManager *shapeManager() const override
    {
        return m_shapeManager;
    }
    void updateCanvas(const QRectF &) override
    {
    }
    KoToolProxy *toolProxy() const override
    {
        return nullptr;
    }
    KoViewConverter *viewConverter() const override
    {
        return nullptr;
    }
    QWidget *canvasWidget() override
    {
        return nullptr;
    }
    const QWidget *canvasWidget() const override
    {
        return nullptr;
    }
    KoUnit unit() const override
    {
        return KoUnit(KoUnit::Millimeter);
    }
    KoGuidesData *guidesData() override
    {
        return m_guideData;
    }
    void updateInputMethodInfo() override
    {
    }
    void setCursor(const QCursor &) override
    {
    }

private:
    KoShapeManager *m_shapeManager;
    qreal m_horz;
    qreal m_vert;
    KoGuidesData *m_guideData;
};

class MockShapeController : public KoShapeBasedDocumentBase
{
public:
    void addShape(KoShape *shape) override
    {
        m_shapes.insert(shape);
    }
    void removeShape(KoShape *shape) override
    {
        m_shapes.remove(shape);
    }
    bool contains(KoShape *shape)
    {
        return m_shapes.contains(shape);
    }

private:
    QSet<KoShape *> m_shapes;
};

class MockContainerModel : public KoShapeContainerModel
{
public:
    MockContainerModel()
    {
        resetCounts();
    }

    /// reimplemented
    void add(KoShape *child) override
    {
        m_children.append(child); // note that we explicitly do not check for duplicates here!
    }
    /// reimplemented
    void remove(KoShape *child) override
    {
        m_children.removeAll(child);
    }

    /// reimplemented
    void setClipped(const KoShape *, bool) override
    {
    } // ignored
    /// reimplemented
    bool isClipped(const KoShape *) const override
    {
        return false;
    } // ignored
    /// reimplemented
    bool isChildLocked(const KoShape *child) const override
    {
        return child->isGeometryProtected();
    }
    /// reimplemented
    int count() const override
    {
        return m_children.count();
    }
    /// reimplemented
    QList<KoShape *> shapes() const override
    {
        return m_children;
    }
    /// reimplemented
    void containerChanged(KoShapeContainer *, KoShape::ChangeType) override
    {
        m_containerChangedCalled++;
    }
    /// reimplemented
    void proposeMove(KoShape *, QPointF &) override
    {
        m_proposeMoveCalled++;
    }
    /// reimplemented
    void childChanged(KoShape *, KoShape::ChangeType) override
    {
        m_childChangedCalled++;
    }
    void setInheritsTransform(const KoShape *, bool) override
    {
    }
    bool inheritsTransform(const KoShape *) const override
    {
        return false;
    }

    int containerChangedCalled() const
    {
        return m_containerChangedCalled;
    }
    int childChangedCalled() const
    {
        return m_childChangedCalled;
    }
    int proposeMoveCalled() const
    {
        return m_proposeMoveCalled;
    }

    void resetCounts()
    {
        m_containerChangedCalled = 0;
        m_childChangedCalled = 0;
        m_proposeMoveCalled = 0;
    }

private:
    QList<KoShape *> m_children;
    int m_containerChangedCalled, m_childChangedCalled, m_proposeMoveCalled;
};

#endif
