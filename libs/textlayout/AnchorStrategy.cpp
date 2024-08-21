/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Matus Hanzes <matus.hanzes@ixonos.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AnchorStrategy.h"

#include "KoTextLayoutRootArea.h"
#include "KoTextShapeContainerModel.h"

#include <KoShapeContainer.h>

#include <TextLayoutDebug.h>

AnchorStrategy::AnchorStrategy(KoShapeAnchor *anchor, KoTextLayoutRootArea *rootArea)
    : m_anchor(anchor)
    , m_rootArea(rootArea)
    , m_model(nullptr)
    , m_pageRect(0, 0, 10, 10)
    , m_pageContentRect(0, 0, 10, 10)
    , m_paragraphRect(0, 0, 0, 0)
    , m_pageNumber(0)
{
}

AnchorStrategy::~AnchorStrategy()
{
    if (m_model)
        m_model->removeAnchor(m_anchor);
}

void AnchorStrategy::detachFromModel()
{
    m_model = nullptr;
}

QRectF AnchorStrategy::pageRect() const
{
    return m_pageRect;
}

void AnchorStrategy::setPageRect(const QRectF &pageRect)
{
    m_pageRect = pageRect;
}

QRectF AnchorStrategy::pageContentRect() const
{
    return m_pageContentRect;
}

void AnchorStrategy::setPageContentRect(const QRectF &pageContentRect)
{
    m_pageContentRect = pageContentRect;
}

QRectF AnchorStrategy::paragraphRect() const
{
    return m_paragraphRect;
}

void AnchorStrategy::setParagraphRect(const QRectF &paragraphRect)
{
    m_paragraphRect = paragraphRect;
}

QRectF AnchorStrategy::paragraphContentRect() const
{
    return m_paragraphContentRect;
}

void AnchorStrategy::setParagraphContentRect(const QRectF &paragraphContentRect)
{
    m_paragraphContentRect = paragraphContentRect;
}

QRectF AnchorStrategy::layoutEnvironmentRect() const
{
    return m_layoutEnvironmentRect;
}

void AnchorStrategy::setLayoutEnvironmentRect(const QRectF &layoutEnvironmentRect)
{
    m_layoutEnvironmentRect = layoutEnvironmentRect;
}

int AnchorStrategy::pageNumber() const
{
    return m_pageNumber;
}

void AnchorStrategy::setPageNumber(int pageNumber)
{
    m_pageNumber = pageNumber;
}

void AnchorStrategy::updateContainerModel()
{
    KoShape *shape = m_anchor->shape();

    KoShapeContainer *container = dynamic_cast<KoShapeContainer *>(m_rootArea->associatedShape());
    if (container == nullptr) {
        if (m_model)
            m_model->removeAnchor(m_anchor);
        m_model = nullptr;
        shape->setParent(nullptr);
        return;
    }

    KoTextShapeContainerModel *theModel = dynamic_cast<KoTextShapeContainerModel *>(container->model());
    if (theModel != m_model) {
        if (m_model)
            m_model->removeAnchor(m_anchor);
        if (shape->parent() != container) {
            if (shape->parent()) {
                shape->parent()->removeShape(shape);
            }
            container->addShape(shape);
        }
        m_model = theModel;
        m_model->addAnchor(m_anchor);
    }
    Q_ASSERT(m_model == theModel);
}
