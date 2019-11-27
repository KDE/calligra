/* This file is part of the KDE project
 * Copyright (C) 2006, 2009,2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWCopyShape.h"
#include "KWPage.h"
#include "KWPageManager.h"
#include "KWFrame.h"
#include "KWFrameSet.h"
#include "KWTextFrameSet.h"
#include "KWRootAreaProvider.h"

#include <KoShapeStrokeModel.h>
#include <KoShapeLoadingContext.h>
#include <KoViewConverter.h>
#include <KoTextShapeData.h>
#include <KoShapeContainer.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <KoXmlReader.h>

#include <QPainter>
#include <QPainterPath>
#include <WordsDebug.h>

#include <algorithm>

KWCopyShape::KWCopyShape(KoShape *original, const KWPageManager *pageManager)
        : KoShape()
        ,m_original(original)
        ,m_pageManager(pageManager)
{
    setSize(m_original->size());
    setSelectable(original->isSelectable());
    // allow selecting me to get the tool for the original to still work.
    QSet<KoShape*> delegates;
    delegates << m_original;
    setToolDelegates(delegates);

    debugWords << "originalShape=" << m_original;
}

KWCopyShape::~KWCopyShape()
{
    debugWords << "originalShape=" << m_original;
}

void KWCopyShape::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext)
{
    Q_ASSERT(m_original);

    //paint all child shapes
    KoShapeContainer* container = dynamic_cast<KoShapeContainer*>(m_original);
    if (container) {
        QList<KoShape*> sortedObjects = container->shapes();
        sortedObjects.append(m_original);
        std::sort(sortedObjects.begin(), sortedObjects.end(), KoShape::compareShapeZIndex);

        // Do the following to revert the absolute transformation of the
        // container that is re-applied in shape->absoluteTransformation()
        // later on.  The transformation matrix of the container has already
        // been applied once before this function is called.
        QTransform baseMatrix = container->absoluteTransformation(&converter).inverted() * painter.transform();

        KWPage copypage = m_pageManager->page(this);
        Q_ASSERT(copypage.isValid());
        foreach(KoShape *shape, sortedObjects) {
            painter.save();
            if (shape != m_original) {
                painter.setTransform(shape->absoluteTransformation(&converter) * baseMatrix);
            }
            KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
            if (data == 0) {
                shape->paint(painter, converter, paintcontext);
            }
            else {
                // Since the rootArea is shared between the copyShape and the originalShape we need to
                // temporary switch the used KoTextPage to be sure the proper page-numbers are displayed.
                KWPage originalpage = m_pageManager->page(shape);
                Q_ASSERT(originalpage.isValid());
                KoTextLayoutRootArea *area = data->rootArea();
                bool wasBlockChanges = false;
                if (area) {
                    // We need to block documentChanged() signals emitted cause for example page-variables
                    // may change there content to result in us marking root-areas dirty for relayout else
                    // we could end in an infinite relayout ping-pong.
                    wasBlockChanges = area->documentLayout()->changesBlocked();
                    area->documentLayout()->setBlockChanges(true);
                    area->setPage(new KWPage(copypage));
                }
                shape->paint(painter, converter, paintcontext);
                if (area) {
                    area->setPage(new KWPage(originalpage));
                    area->documentLayout()->setBlockChanges(wasBlockChanges);
                }
            }
            painter.restore();
            if (shape->stroke()) {
                painter.save();
                painter.setTransform(shape->absoluteTransformation(&converter) * baseMatrix);
                shape->stroke()->paint(shape, painter, converter);
                painter.restore();
            }
        }
    } else {
        //paint the original shape
        painter.save();
        m_original->paint(painter, converter, paintcontext);
        painter.restore();
        if (m_original->stroke()) {
            m_original->stroke()->paint(m_original, painter, converter);
        }
    }
}

QPainterPath KWCopyShape::outline() const
{
    Q_ASSERT(m_original);
    return m_original->outline();
}

QRectF KWCopyShape::outlineRect() const
{
    return m_original->outlineRect();
}

QRectF KWCopyShape::boundingRect() const
{
    // Since we paint the originals children we also need to report the translated
    // boundingRects of those children as part of our own boundingRect in order to
    // make sure they are drawn if update rect intersects them but not the m_original
    // itself.
    QRectF bb = KoShape::boundingRect();
    QPointF offset = bb.topLeft() - m_original->boundingRect().topLeft();

    KoShapeContainer* container = dynamic_cast<KoShapeContainer*>(m_original);
    if (container) {
        foreach (KoShape *shape, container->shapes()) {
            bb |= shape->boundingRect().translated(offset);
        }
    }

    return bb;
}

void KWCopyShape::saveOdf(KoShapeSavingContext &context) const
{
    Q_ASSERT(m_original);
    KWCopyShape *me = const_cast<KWCopyShape*>(this);
    me->setAdditionalAttribute("draw:copy-of", m_original->name());
    saveOdfAttributes(context, OdfAllAttributes);
    me->removeAdditionalAttribute("draw:copy-of");
}

bool KWCopyShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);

    return false; // TODO well not really as we only use copy shapes for headers and footers and
                  // those copies are not saved
}

KoShape *KWCopyShape::original() const
{
    return m_original;
}
