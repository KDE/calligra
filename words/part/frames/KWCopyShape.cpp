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

#include <KoShapeBorderModel.h>
#include <KoShapeLoadingContext.h>
#include <KoViewConverter.h>
#include <KoTextShapeData.h>
#include <KoShapeContainer.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>

#include <QPainter>
#include <QPainterPath>
#include <KDebug>

KWCopyShape::KWCopyShape(KoShape *original, const KWPageManager *pageManager)
        : m_original(original),
        m_pageManager(pageManager)
{
    setSize(m_original->size());
    setSelectable(original->isSelectable());
    // allow selecting me to get the tool for the original to still work.
    QSet<KoShape*> delegates;
    delegates << m_original;
    setToolDelegates(delegates);

    kDebug(32001) << "originalShape=" << original;
}

KWCopyShape::~KWCopyShape()
{
    kDebug(32001);
}

void KWCopyShape::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_ASSERT(m_original);

    // Since the rootArea is shared between the copyShape and the originalShape we need to
    // temporary switch the used KoTextPage to be sure the proper page-numbers are displayed.
    class ScopedPageSwitcher {
        public:
            ScopedPageSwitcher(const KWPageManager *m_pageManager, KWCopyShape *copyshape, KoShape *original) {
                KWFrame *frame = dynamic_cast<KWFrame*>(original->applicationData());
                Q_ASSERT(frame);
                KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
                Q_ASSERT(frameset);
                KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout*>(frameset->document()->documentLayout());
                Q_ASSERT(lay);
                m_rootArea = lay->rootAreaForPosition(0);
                Q_ASSERT(m_rootArea);
                Q_ASSERT(m_rootArea->associatedShape() == original);
                KWPage page = m_pageManager->page(copyshape);
                Q_ASSERT(page.isValid());
                m_oldPage = dynamic_cast<KWPage*>(m_rootArea->page());
                Q_ASSERT(m_oldPage);
                m_oldPage = new KWPage(*m_oldPage);
                m_rootArea->setPage(new KWPage(page)); // takes over ownership
            }
            ~ScopedPageSwitcher() {
                m_rootArea->setPage(m_oldPage);
            }
        private:
            KoTextLayoutRootArea *m_rootArea;
            KWPage *m_oldPage;
    };
    ScopedPageSwitcher scopedswitcher(m_pageManager, this, m_original);

    //paint all child shapes
    KoShapeContainer* container = dynamic_cast<KoShapeContainer*>(m_original);
    if (container && container->shapeCount()) {
        QList<KoShape*> sortedObjects = container->shapes();
        sortedObjects.append(m_original);
        qSort(sortedObjects.begin(), sortedObjects.end(), KoShape::compareShapeZIndex);

        // Do the following to revert the absolute transformation of the
        // container that is re-applied in shape->absoluteTransformation()
        // later on.  The transformation matrix of the container has already
        // been applied once before this function is called.
        QTransform baseMatrix = container->absoluteTransformation(&converter).inverted() * painter.transform();

        foreach(KoShape *shape, sortedObjects) {
            painter.save();
            if (shape != m_original) {
                painter.setTransform(shape->absoluteTransformation(&converter) * baseMatrix);
            }
            shape->paint(painter, converter);
            painter.restore();
            if (shape->border()) {
                painter.save();
                painter.setTransform(shape->absoluteTransformation(&converter) * baseMatrix);
                shape->border()->paint(shape, painter, converter);
                painter.restore();
            }
        }
    } else {
        //paint the original shape
        painter.save();
        m_original->paint(painter, converter);
        painter.restore();
        if (m_original->border()) {
            m_original->border()->paint(m_original, painter, converter);
        }
    }
}

void KWCopyShape::paintDecorations(QPainter &painter, const KoViewConverter &converter, const KoCanvasBase *canvas)
{
    Q_ASSERT(m_original);
    m_original->paintDecorations(painter, converter, canvas);
}

QPainterPath KWCopyShape::outline() const
{
    Q_ASSERT(m_original);
    return m_original->outline();
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
#ifdef __GNUC__
    #warning TODO: implement KWCopyShape::loadOdf
#endif

    return false; // TODO
}

KoShape *KWCopyShape::original() const
{
    return m_original;
}
