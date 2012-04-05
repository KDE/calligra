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

#include "KWTextFrameSet.h"
#include "KWFrame.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWRootAreaProvider.h"
#include "KWDocument.h"
#include "KWCopyShape.h"

#include <KoTextShapeData.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextDocumentLayout.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>

#include <changetracker/KoChangeTracker.h>

#include <klocale.h>
#include <kdebug.h>
#include <QTextDocument>
#include <QTextBlock>

KWTextFrameSet::KWTextFrameSet(KWDocument *wordsDocument, Words::TextFrameSetType type)
    : KWFrameSet(Words::TextFrameSet)
    , m_document(new QTextDocument())
    , m_textFrameSetType(type)
    , m_pageManager(wordsDocument->pageManager())
    , m_wordsDocument(wordsDocument)
    , m_rootAreaProvider(new KWRootAreaProvider(this))
{
    Q_ASSERT(m_wordsDocument);
    setName(Words::frameSetTypeName(m_textFrameSetType));
    setupDocument();

    kDebug(32001) << "frameSet=" << this << "frameSetType=" << Words::frameSetTypeName(textFrameSetType());
}

KWTextFrameSet::~KWTextFrameSet()
{
    kDebug(32001) << "frameSet=" << this << "frameSetType=" << Words::frameSetTypeName(textFrameSetType());
    delete m_rootAreaProvider;
#if 0
    // first remove the doc from all our frames so they won't try to use it after we delete it.
    if (!m_frames.isEmpty()) {
        // we transfer ownership of the doc to our last shape so it will keep being alive until nobody references it anymore.
        QList<KWFrame*>::Iterator iter = m_frames.end();
        --iter;
        do {
            // try to find a frame that already has layout data to take ownership of the doc.
            KoTextShapeData *tsd = qobject_cast<KoTextShapeData*>(m_frames.last()->shape()->userData());
            if (tsd) {
                tsd->setDocument(m_document);
                m_document = 0;
                break;
            }
            --iter;
        } while (iter != m_frames.begin());
        // if no frames have a KoTextShapeData, its save to delete m_document.
    }
#endif
    delete m_document;
}

void KWTextFrameSet::setupFrame(KWFrame *frame)
{
    Q_ASSERT(frame->shape());
    Q_ASSERT(frame->frameSet() == this);
    Q_ASSERT(frames().contains(frame));

    KWPage page = m_pageManager->page(frame->shape());
    if (!page.isValid()) {
        // This can happen if the KWFrame was created by someone else at a time where we didn't start
        // layouting yet and therefore things are not proper setup yet. That's okay and they will be
        // proper setup at a later time once we start layouting.
    } else {
        if (page.pageNumber() <= m_rootAreaProvider->pages().count()) {
            // The just added KWFrame needs to invalidate the layouter so the layouter picks up the new
            // KWFrame on the next layout-run.
            m_rootAreaProvider->clearPages(page.pageNumber());
        }
    }

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
    if (!data) {
        // copy-frames don't need to be setup cause they only point to the referenced KWFrame which
        // contains everything needed and which was or will be proper setup.
        Q_ASSERT(dynamic_cast<KWCopyShape*>(frame->shape()));
        return;
    }

    kDebug(32001) << "frameSet=" << frame->frameSet() << "frame=" << frame << "pageNumber=" << page.pageNumber();

    // Handle the special case that the KoTextShapeData already defines a QTextDocument that we need
    // to take over. This is the case for example with OtherTextFrameSet's where the KWTextFrameSet
    // and the KWFrame are created after the TextShape was created and it's loadOdf was called what
    // means that the QTextDocument of the KoTextShapeData already has content we like to take over.
    // The mainTextFrame's are created on demand and need to be ignored.
    if (textFrameSetType() != Words::MainTextFrameSet && frameCount() == 1 && data->document() && m_document->isEmpty()) {
        Q_ASSERT(m_document != data->document());
        delete m_document;
        m_document = data->document();
        setupDocument();
    }

    // The QTexDocument is shared between the shapes and we are the owner.
    data->setDocument(m_document, false);

#if 0
    m_frameOrderDirty = true;
    data->setDocument(m_document, false);
    data->setEndPosition(-1);
    data->foul();
    if (m_allowLayoutRequests) {
        KWTextDocumentLayout *lay = dynamic_cast<KWTextDocumentLayout*>(m_document->documentLayout());
        if (lay) {
            lay->scheduleLayout();
            emit lay->shapeAdded(frame->shape());
        }
    }
    connect(data, SIGNAL(relayout()), this, SLOT(updateTextLayout()));
#endif
#ifndef QT_NO_DEBUG // these checks are just sanity checks in development mode
    KoTextDocument doc(m_document);
    KoStyleManager *styleManager = m_wordsDocument->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    Q_ASSERT(doc.styleManager() == styleManager);
    KoChangeTracker *changeTracker = m_wordsDocument->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
    Q_ASSERT(doc.changeTracker() == changeTracker);
    Q_ASSERT(doc.inlineTextObjectManager() == m_wordsDocument->inlineTextObjectManager());
    Q_ASSERT(doc.undoStack() == m_wordsDocument->resourceManager()->undoStack());
#endif
}

void KWTextFrameSet::setupDocument()
{
    m_document->setUseDesignMetrics(true);

    KoTextDocument doc(m_document);
    doc.setInlineTextObjectManager(m_wordsDocument->inlineTextObjectManager());
    KoStyleManager *styleManager = m_wordsDocument->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    doc.setStyleManager(styleManager);
    KoChangeTracker *changeTracker = m_wordsDocument->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
    doc.setChangeTracker(changeTracker);
    doc.setUndoStack(m_wordsDocument->resourceManager()->undoStack());
    doc.setShapeController(m_wordsDocument->shapeController());

    doc.setRelativeTabs(true);
    doc.setParaTableSpacingAtStart(true);

    // the KoTextDocumentLayout needs to be setup after the actions above are done to prepare the document
    KoTextDocumentLayout *lay = new KoTextDocumentLayout(m_document, m_rootAreaProvider);
    lay->setWordprocessingMode();
    m_document->setDocumentLayout(lay);
    QObject::connect(lay, SIGNAL(layoutIsDirty()), lay, SLOT(scheduleLayout()));
}
    
void KWTextFrameSet::setPageStyle(const KWPageStyle &style)
{
    kDebug(32001) << "frameSet=" << this << "frameSetType=" << Words::frameSetTypeName(textFrameSetType()) << "pageStyleName=" << style.name() << "pageStyleIsValid=" << style.isValid();
    m_pageStyle = style;
    if (style.isValid()) {
        foreach(KWFrame* frame, frames()) {
            if (frame->shape()) {
                frame->shape()->setBackground(style.background());
            }
        }
    }
}

const KWPageStyle& KWTextFrameSet::pageStyle() const
{
    return m_pageStyle;
}

#if 0
// static   returns true if frame1 comes before frame2
bool KWTextFrameSet::sortTextFrames(const KWFrame *f1, const KWFrame *f2)
{
    if (!f1 && f2) // copy always come after textframe
        return false;
    if (f1 && !f2) // copy always come after textframe
        return true;
    if (f1 && f2 && f1->sortingId() >= 0 && f2->sortingId() >= 0) { // copy frames don't have a sortingId
        return f1->sortingId() < f2->sortingId();
    }
#if 1
    // use a more performant way of sorting the frames
    QPointF tl1 = frame1->shape()->absolutePosition(KoFlake::TopLeftCorner);
    QPointF tl2 = frame2->shape()->absolutePosition(KoFlake::TopLeftCorner);

    if (tl1.y() == tl2.y()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(frame1->frameSet());
        bool rtl = false; // right-to-left
        if (tfs && tfs->pageManager()) { // check per page.
            KWPage page1 = tfs->pageManager()->page(frame1->shape());
            if (page1.isValid()) {
                rtl = page1.directionHint() == KoText::RightLeftTopBottom;
            }
        }

        if (rtl) {
            QPointF br1 = frame1->shape()->absolutePosition(KoFlake::BottomRightCorner);
            QPointF br2 = frame2->shape()->absolutePosition(KoFlake::BottomRightCorner);
            return br2.x() < br1.x();
        }
        return tl1.x() < tl2.x();
    }
    return tl1.y() < tl2.y();
#else
    KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(frame1->frameSet());
    bool rtl = false; // right-to-left
    if (tfs && tfs->pageManager()) { // check per page.
        KWPage page1 = tfs->pageManager()->page(frame1->shape());
        KWPage page2 = tfs->pageManager()->page(frame2->shape());
        if (page1 != page2 && page1.isValid() && page2.isValid())
            return page1.pageNumber() < page2.pageNumber();

        // both on same page
        if (page1.isValid())
            rtl = page1.directionHint() == KoText::RightLeftTopBottom;
    }
    QRectF boundsF1 = frame1->shape()->boundingRect();
    QRectF boundsF2 = frame2->shape()->boundingRect();
    //kDebug(32001) << "boundsF1:" << boundsF1 << "boundsF2:" << boundsF2;

    // support frame stacking.
    if (boundsF1.bottom() < boundsF2.top() && boundsF1.left() > boundsF2.right())
        return true;
    if (boundsF1.top() > boundsF2.bottom() && boundsF1.right() < boundsF2.left())
        return false;

    QPointF posF1 = frame1->shape()->absolutePosition();
    if (posF1.x() > boundsF2.right())
        return rtl;
    if (posF1.x() < boundsF2.left())
        return !rtl;

    // check the Y position. Y is greater only when it is below the second frame.
    if (posF1.y() > boundsF2.bottom())
        return false;
    if (posF1.y() < boundsF2.top())
        return true;

    // my center lies inside frame2. Lets check the topleft pos.
    if (frame1->shape()->boundingRect().top() > boundsF2.top())
        return false;
#endif
    return true;
}
#endif
