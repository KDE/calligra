/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#include "KWFrame.h"
#include "KWFrameSet.h"
#include "KWTextFrameSet.h"
#include "KWCopyShape.h"
#include "KWOutlineShape.h"
#include "KoShapeAnchor.h"
#include "KWPage.h"
#include "KWRootAreaProvider.h"
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <KoXmlWriter.h>
#include <kdebug.h>

KWFrame::KWFrame(KoShape *shape, KWFrameSet *parent, KoShapeAnchor *anchor)
        : m_shape(shape),
        m_frameBehavior(Words::AutoExtendFrameBehavior),
        m_newFrameBehavior(Words::NoFollowupFrame),
        m_anchoredFrameOffset(0.0),
        m_frameSet(parent),
        m_minimumFrameHeight(0.0), // no minimum height per default
        m_anchor(anchor)
{
    Q_ASSERT(shape);
    shape->setApplicationData(this);
    if (parent)
        parent->addFrame(this);

    KWTextFrameSet* parentFrameSet = dynamic_cast<KWTextFrameSet*>(parent);
    if (parentFrameSet) {
        if (Words::isHeaderFooter(parentFrameSet)) {
            if (KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData())) {
                // header and footer are always auto-grow-height independent of whatever
                // was defined for them in the document.
                data->setResizeMethod(KoTextShapeDataBase::AutoGrowHeight);
            }
        }
        if (parentFrameSet->textFrameSetType() == Words::OtherTextFrameSet) {
            /* NoResize should be default this days. Setting it here would overwrite any value
              read in TextShape::loadStyle what is not what we want.

            if (KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData())) {
                data->setResizeMethod(KoTextShapeDataBase::NoResize);
            }
            */
        } else {
            shape->setGeometryProtected(true);

            // We need to keep collision detection on or we will not relayout when page anchored shapes are
            // moved. For page anchored shapes (which are different from anchored shapes which are usually
            // children of the shape they are anchored too and therefore the ShapeManager filters collision
            // events for them out) the KoTextRootAreaProvider::relevantObstructions method is used to produce
            // obstructions whereas for anchored shapes the KoTextDocumentLayout::registerAnchoredObstruction
            // is used to explicit register the obstructions.
            //shape->setCollisionDetection(false);
        }
    }

    //kDebug(32001) << "frame=" << this << "frameSet=" << frameSet() << "frameSetType=" << Words::frameSetTypeName(frameSet()) << "anchoredPageNumber=" << m_anchoredPageNumber;
}

KWFrame::~KWFrame()
{
    //kDebug(32001) << "frame=" << this << "frameSet=" << frameSet() << "frameSetType=" << Words::frameSetTypeName(frameSet()) << "anchoredPageNumber=" << m_anchoredPageNumber;

    KoShape *ourShape = m_shape;
    m_shape = 0; // no delete is needed as the shape deletes us.

    if (m_frameSet) {
        cleanupShape(ourShape);

        bool justMe = m_frameSet->frameCount() == 1;
        m_frameSet->removeFrame(this, ourShape); // first remove me so we won't get double
                                                 // deleted. ourShape is needed to mark any
                                                 // copyShapes as retired
        if (justMe) {
            kDebug(32001) << "Last KWFrame removed from frameSet=" << m_frameSet;
            // FIXME: Fix when a proper way to delete framesets have been found.
            // The frameset is never deleted (here) because removeFrame() above results in
            // m_frameSet to be set to 0.
            delete m_frameSet;
            m_frameSet = 0;
        }
    }
}

qreal KWFrame::minimumFrameHeight() const
{
    return m_minimumFrameHeight;
}

void KWFrame::setMinimumFrameHeight(qreal minimumFrameHeight)
{
    if (m_minimumFrameHeight == minimumFrameHeight)
        return;
    m_minimumFrameHeight = minimumFrameHeight;

    // transfer the minimumFrameHeight to the copy-shapes
    foreach(KWFrame* copyFrame, m_copyShapes) {
        copyFrame->setMinimumFrameHeight(m_minimumFrameHeight);
    }
}

void KWFrame::cleanupShape(KoShape* shape)
{
    Q_ASSERT(m_frameSet);
    KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(m_frameSet);
    if (tfs) {
        KWRootAreaProvider *rootAreaProvider = tfs->rootAreaProvider();
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(tfs->document()->documentLayout());
        Q_ASSERT(lay);
        QList<KoTextLayoutRootArea *> layoutRootAreas = lay->rootAreas();
        for(int i = 0; i < layoutRootAreas.count(); ++i) {
            KoTextLayoutRootArea *rootArea = layoutRootAreas[i];
            if (rootArea->associatedShape() == shape) {
                KoTextLayoutRootArea *prevRootArea = i >= 1 ? layoutRootAreas[i - 1] : 0;
                rootAreaProvider->releaseAllAfter(prevRootArea);
                lay->removeRootArea(prevRootArea);
                rootArea->setAssociatedShape(0);
                break;
            }
        }
    }
}

void KWFrame::setFrameSet(KWFrameSet *fs)
{
    if (fs == m_frameSet)
        return;
    Q_ASSERT_X(!fs || !m_frameSet, __FUNCTION__, "Changing the FrameSet afterwards needs to invalidate lots of stuff including whatever is done in the KWRootAreaProvider. The better way would be to not allow this.");
    if (m_frameSet) {
        if (m_shape)
            cleanupShape(m_shape);
        m_frameSet->removeFrame(this);
    }
    m_frameSet = fs;
    if (fs)
        fs->addFrame(this);
}

QList<KWFrame*> KWFrame::copies() const
{
    return m_copyShapes;
}

void KWFrame::addCopy(KWFrame* frame)
{
    if (!m_copyShapes.contains(frame))
        m_copyShapes.append(frame);
}

void KWFrame::removeCopy(KWFrame* frame)
{
    m_copyShapes.removeAll(frame);
}

void KWFrame::copySettings(const KWFrame *frame)
{
    setFrameBehavior(frame->frameBehavior());
    setNewFrameBehavior(frame->newFrameBehavior());
    shape()->copySettings(frame->shape());
}

void KWFrame::saveOdf(KoShapeSavingContext &context, const KWPage &page, int /*pageZIndexOffset*/) const
{
    QString value;
    switch (frameBehavior()) {
    case Words::AutoCreateNewFrameBehavior:
        value = "auto-create-new-frame";
        break;
    case Words::IgnoreContentFrameBehavior:
        value = "clip";
        break;
    case Words::AutoExtendFrameBehavior:
        // the third case, AutoExtendFrame is handled by min-height
        value.clear();
        if (minimumFrameHeight() > 1)
            m_shape->setAdditionalAttribute("fo:min-height", QString::number(minimumFrameHeight()) + "pt");
        break;
    }
    if (!value.isEmpty())
        m_shape->setAdditionalStyleAttribute("style:overflow-behavior", value);

    switch (newFrameBehavior()) {
    case Words::ReconnectNewFrame: value = "followup"; break;
    case Words::NoFollowupFrame: value.clear(); break; // "none" is the default
    case Words::CopyNewFrame: value = "copy"; break;
    }
    if (!value.isEmpty()) {
        m_shape->setAdditionalStyleAttribute("calligra:frame-behavior-on-new-page", value);
    }

    // shape properties
    const qreal pagePos = page.offsetInDocument();

    m_shape->setAdditionalAttribute("text:anchor-type", "page");
    m_shape->setAdditionalAttribute("text:anchor-page-number", QString::number(page.pageNumber()));
    context.addShapeOffset(m_shape, QTransform(1, 0, 0 , 1, 0, -pagePos));
    m_shape->saveOdf(context);
    context.removeShapeOffset(m_shape);
    m_shape->removeAdditionalAttribute("fo:min-height");
    m_shape->removeAdditionalAttribute("text:anchor-page-number");
    m_shape->removeAdditionalAttribute("text:anchor-page-number");
    m_shape->removeAdditionalAttribute("text:anchor-type");
}

bool KWFrame::isCopy() const
{
    return dynamic_cast<KWCopyShape*>(shape());
}
