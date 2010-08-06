/* This file is part of the KDE project
 * Copyright (C) 2006-2010 Thomas Zander <zander@kde.org>
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
#include "KWTextDocumentLayout.h"
#include "KWFrame.h"
#include "KWTextFrame.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWDocument.h"

#include <KoTextShapeData.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>

#include <changetracker/KoChangeTracker.h>

#include <klocale.h>
#include <kdebug.h>
#include <QTextDocument>
#include <QTextBlock>

KWTextFrameSet::KWTextFrameSet(const KWDocument *doc)
        : m_document(new QTextDocument()),
        m_layoutTriggered(false),
        m_allowLayoutRequests(true),
        m_frameOrderDirty(true),
        m_textFrameSetType(KWord::OtherTextFrameSet),
        m_pageManager(0),
        m_kwordDocument(doc)
{
    m_document->setDocumentLayout(new KWTextDocumentLayout(this));
    if (m_kwordDocument) {
        KoTextDocument doc(m_document);
        doc.setInlineTextObjectManager(m_kwordDocument->inlineTextObjectManager());
        KoStyleManager *styleManager = m_kwordDocument->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
        Q_ASSERT(styleManager);
        doc.setStyleManager(styleManager);
        KoChangeTracker *changeTracker = m_kwordDocument->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
        Q_ASSERT(changeTracker);
        doc.setChangeTracker(changeTracker);
        doc.setUndoStack(m_kwordDocument->resourceManager()->undoStack());
    }
    m_document->setUseDesignMetrics(true);
}

KWTextFrameSet::KWTextFrameSet(const KWDocument *doc, KWord::TextFrameSetType type)
        : m_document(new QTextDocument()),
        m_layoutTriggered(false),
        m_allowLayoutRequests(true),
        m_frameOrderDirty(true),
        m_textFrameSetType(type),
        m_pageManager(0),
        m_kwordDocument(doc)
{
    m_document->setDocumentLayout(new KWTextDocumentLayout(this));
    if (m_kwordDocument) {
        KoTextDocument doc(m_document);
        doc.setInlineTextObjectManager(m_kwordDocument->inlineTextObjectManager());
        KoStyleManager *styleManager = m_kwordDocument->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
        doc.setStyleManager(styleManager);
        KoChangeTracker *changeTracker = m_kwordDocument->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
        doc.setChangeTracker(changeTracker);
        doc.setUndoStack(m_kwordDocument->resourceManager()->undoStack());
    }
    m_document->setUseDesignMetrics(true);
    switch (m_textFrameSetType) {
    case KWord::OddPagesHeaderTextFrameSet:
        setName(i18n("Odd Pages Header"));
        break;
    case KWord::EvenPagesHeaderTextFrameSet:
        setName(i18n("Even Pages Header"));
        break;
    case KWord::OddPagesFooterTextFrameSet:
        setName(i18n("Odd Pages Footer"));
        break;
    case KWord::EvenPagesFooterTextFrameSet:
        setName(i18n("Even Pages Footer"));
        break;
    case KWord::MainTextFrameSet:
        setName(i18n("Main text"));
        break;
    case KWord::PageBackgroundFrameSet:
        setName(i18n("Page Background"));
        break;
    default: ;
    }
}

KWTextFrameSet::~KWTextFrameSet()
{
    // first remove the doc from all our frames so they won't try to use it after we delete it.
    if (!m_frames.isEmpty() && m_textFrameSetType != KWord::PageBackgroundFrameSet) {
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
    delete m_document;
}

void KWTextFrameSet::setupFrame(KWFrame *frame)
{
    if (frame->shape() && m_pageStyle.isValid() && m_textFrameSetType == KWord::PageBackgroundFrameSet) {
        frame->shape()->setBackground(m_pageStyle.background());
        return;
    }
    if (m_textFrameSetType != KWord::OtherTextFrameSet)
        frame->shape()->setGeometryProtected(true);
    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
    if (data == 0) {// probably a copy frame.
        Q_ASSERT(frameCount() > 1);
        return;
    }
    if (frameCount() == 1 && m_document->isEmpty()) { // just added first frame...
        delete m_document;
        m_document = data->document();
        m_document->setDocumentLayout(new KWTextDocumentLayout(this));
        if (m_kwordDocument) {
            KoTextDocument doc(m_document);
            KoStyleManager *styleManager = m_kwordDocument->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
            doc.setStyleManager(styleManager);
            KoChangeTracker *changeTracker = m_kwordDocument->resourceManager()->resource(KoText::ChangeTracker).value<KoChangeTracker*>();
            doc.setChangeTracker(changeTracker);
            doc.setInlineTextObjectManager(m_kwordDocument->inlineTextObjectManager());
            doc.setUndoStack(m_kwordDocument->resourceManager()->undoStack());
        }
        data->setDocument(m_document, false);
    } else {
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
    }
    connect(data, SIGNAL(relayout()), this, SLOT(updateTextLayout()));
}

void KWTextFrameSet::updateTextLayout()
{
    if (! m_allowLayoutRequests)
        return;
    KWTextDocumentLayout *lay = dynamic_cast<KWTextDocumentLayout*>(m_document->documentLayout());
    if (lay)
        lay->scheduleLayout();
}

void KWTextFrameSet::requestMoreFrames(qreal textHeight)
{
    //kDebug() <<"KWTextFrameSet::requestMoreFrames" << textHeight;
    if (frameCount() == 0)
        return; // there is no way we can get more frames anyway.
    KWTextFrame *lastFrame = static_cast<KWTextFrame*>(frames()[frameCount()-1]);

    if (lastFrame && KWord::isHeaderFooter(this)) {
        KWTextFrame *frame = static_cast<KWTextFrame*>(frames().first());
        frame->setMinimumFrameHeight(frame->minimumFrameHeight() + textHeight + 1E-6);
        emit decorationFrameResize(this);
    } else if (textHeight == 0.0 || lastFrame->frameBehavior() == KWord::AutoCreateNewFrameBehavior) {
        if (lastFrame->newFrameBehavior() == KWord::ReconnectNewFrame)
            emit moreFramesNeeded(this);
    } else if (lastFrame->frameBehavior() == KWord::AutoExtendFrameBehavior
            && lastFrame->canAutoGrow() && qAbs(textHeight) > 2) {
        // enlarge last shape
        KoShape *shape = lastFrame->shape();
        if (shape->isGeometryProtected()) { // don't alter a locked shape.
            requestMoreFrames(0);
            return;
        }
        QSizeF size = shape->size();
        QPointF orig = shape->absolutePosition(KoFlake::TopLeftCorner);
        shape->setSize(QSizeF(size.width(), size.height() + textHeight + 1E-6));
        shape->setAbsolutePosition(orig, KoFlake::TopLeftCorner);
        shape->update(QRectF(0.0, size.height(), size.width(), textHeight + 1E-6));
        lastFrame->allowToGrow();
    }
}

void KWTextFrameSet::spaceLeft(qreal excessHeight)
{
//kDebug() <<"KWTextFrameSet::spaceLeft" << excessHeight;
    Q_ASSERT(excessHeight >= 0);
    if (m_frames.count() == 0)
        return;
    if (KWord::isHeaderFooter(this)) {
        KWTextFrame *frame = static_cast<KWTextFrame*>(frames().first());
        frame->setMinimumFrameHeight(frame->minimumFrameHeight() - excessHeight);
        emit  decorationFrameResize(this);
        return;
    }
    QList<KWFrame*>::Iterator iter = --m_frames.end();
    do {
        KWTextFrame *tf = dynamic_cast<KWTextFrame*>(*(iter));
        if (tf) {
            if (tf && tf->frameBehavior() == KWord::AutoExtendFrameBehavior) {
                tf->autoShrink(tf->shape()->size().height() - excessHeight);
                tf->allowToGrow();
            }
            return;
        }
        --iter;
    } while (iter != m_frames.begin());
}

void KWTextFrameSet::framesEmpty(int emptyFrames)
{
    //kDebug() <<"KWTextFrameSet::framesEmpty" << emptyFrames;
    if (m_pageManager == 0) // be lazy; just refuse to delete frames if we don't know which are on which page
        return;
    if (KWord::isHeaderFooter(this)) // then we are deleted by the frameManager
        return;
    QList<KWFrame*> myFrames = m_frames; // make a copy so we can do a removeFrame without worries
    QList<KWFrame*>::Iterator deleteFrom = myFrames.end();
    QList<KWFrame*>::Iterator iter = --myFrames.end();
    KWPage page;
    bool deleteSome = false;
    do {
        KWPage pageForFrame = m_pageManager->page((*iter)->shape());
        if (!page.isValid()) { // first loop
            page = pageForFrame;
        } else if (page != pageForFrame) { // all frames on the page (of this FS) are empty.
            deleteFrom = iter;
            ++deleteFrom;
            page = pageForFrame;
            deleteSome = true;
        }
        if (--emptyFrames < 0)
            break;
    } while (iter-- != myFrames.begin());

    if (!deleteSome)
        return;

    iter = --myFrames.end();
    do { // remove all frames from end till last empty page
        if (*iter == *m_frames.begin())
            break;
        removeFrame(*iter);
        delete(*iter)->shape();
    } while (iter-- != deleteFrom);
}

void KWTextFrameSet::setAllowLayout(bool allow)
{
    if (allow == m_allowLayoutRequests)
        return;
    m_allowLayoutRequests = allow;
    if (m_allowLayoutRequests) {
        KWTextDocumentLayout *lay = dynamic_cast<KWTextDocumentLayout*>(m_document->documentLayout());
        if (lay)
            lay->scheduleLayout();
    }
}

bool KWTextFrameSet::allowLayout() const
{
    return m_allowLayoutRequests;
}

void KWTextFrameSet::setPageStyle(const KWPageStyle &style)
{
    m_pageStyle = style;
    if (style.isValid()) {
        foreach(KWFrame* frame, m_frames) {
            if (frame->shape()) {
                frame->shape()->setBackground(style.background());
            }
        }
    }
}

KWPageStyle KWTextFrameSet::pageStyle() const
{
    return m_pageStyle;
}

void KWTextFrameSet::sortFrames()
{
     // optimize to not sort more than needed
    if (!m_frames.isEmpty() && (m_frameOrderDirty || m_textFrameSetType == KWord::OtherTextFrameSet)) {
        KWFrame *first = m_frames.first();
        qSort(m_frames.begin(), m_frames.end(), sortTextFrames);
        if (m_frames[0] != first) { // that means it needs to be re-layouted
            KoTextShapeData *tsd = qobject_cast<KoTextShapeData*>(m_frames[0]->shape()->userData());
            if (tsd)
                tsd->foul();
        }
    }
    m_frameOrderDirty = false;
}

// static   returns true if frame1 comes before frame2
bool KWTextFrameSet::sortTextFrames(const KWFrame *frame1, const KWFrame *frame2)
{
    const KWTextFrame *f1 = dynamic_cast<const KWTextFrame*>(frame1);
    const KWTextFrame *f2 = dynamic_cast<const KWTextFrame*>(frame2);

    if (!f1 && f2) // copy always come after textframe
        return false;

    if (f1 && !f2) // copy always come after textframe
        return true;

    if (f1 && f2 && f1->sortingId() >= 0 && f2->sortingId() >= 0) { // copy frames don't have a sortingId
        return f1->sortingId() < f2->sortingId();
    }

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
    return true;
}

#ifndef NDEBUG
void KWTextFrameSet::printDebug(KWFrame *frame)
{
    KWFrameSet::printDebug(frame);
    KoTextShapeData *textShapeData = qobject_cast<KoTextShapeData*>(frame->shape()->userData());
    if (textShapeData == 0) return;
    kDebug(32001) << " Text position:" << textShapeData->position() << ", end:" << textShapeData->endPosition();
    kDebug(32001) << " Offset in text-document;" << textShapeData->documentOffset();
}

void KWTextFrameSet::printDebug()
{
    static const char * type[] = { "OddPagesHeader", "EvenPagesHeader", "OddPagesFooter", "EvenPagesFooter", "Main", "Other", "ERROR" };
    kDebug(32001) << " | Is a KWTextFrameSet";
    kDebug(32001) << " | FS Type:" << type[m_textFrameSetType];
    if (m_pageStyle.isValid())
        kDebug(32001) << " | belongs to page style:" << m_pageStyle.name();
    KWFrameSet::printDebug();
}
#endif

