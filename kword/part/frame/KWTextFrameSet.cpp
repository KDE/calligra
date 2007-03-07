/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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
#include <KoInlineTextObjectManager.h>

#include <klocale.h>
#include <kdebug.h>
#include <QTextDocument>
#include <QTimer>

KWTextFrameSet::KWTextFrameSet(const KWDocument *doc)
    : m_document( new QTextDocument() ),
    m_protectContent(false),
    m_layoutTriggered(false),
    m_allowLayoutRequests(true),
    m_textFrameSetType( KWord::OtherTextFrameSet ),
    m_pageManager(0),
    m_kwordDocument(doc)
{
    KWTextDocumentLayout *layout = new KWTextDocumentLayout(this);
    if(doc)
        layout->setInlineObjectTextManager(m_kwordDocument->inlineTextObjectManager());
    m_document->setDocumentLayout(layout);
    m_document->setUseDesignMetrics(true);
}

KWTextFrameSet::KWTextFrameSet(const KWDocument *doc, KWord::TextFrameSetType type)
    : m_document( new QTextDocument() ),
    m_protectContent(false),
    m_layoutTriggered(false),
    m_allowLayoutRequests(true),
    m_textFrameSetType( type ),
    m_pageManager(0),
    m_kwordDocument(doc)
{
    KWTextDocumentLayout *layout = new KWTextDocumentLayout(this);
    if(doc)
        layout->setInlineObjectTextManager(m_kwordDocument->inlineTextObjectManager());
    m_document->setDocumentLayout(layout);
    m_document->setUseDesignMetrics(true);
    switch(m_textFrameSetType) {
        case KWord::FirstPageHeaderTextFrameSet:
            setName(i18n("First Page Header"));
            break;
        case KWord::OddPagesHeaderTextFrameSet:
            setName(i18n("Odd Pages Header"));
            break;
        case KWord::EvenPagesHeaderTextFrameSet:
            setName(i18n("Even Pages Header"));
            break;
        case KWord::FirstPageFooterTextFrameSet:
            setName(i18n("First Page Footer"));
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
        default: ;
    }
}

KWTextFrameSet::~KWTextFrameSet() {
    delete m_document;
}

void KWTextFrameSet::setupFrame(KWFrame *frame) {
    if(m_textFrameSetType != KWord::OtherTextFrameSet)
        frame->shape()->setLocked(true);
    KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (frame->shape()->userData());
    if(data == 0) {// probably a copy frame.
        Q_ASSERT(frameCount() > 1);
        return;
    }
    if(frameCount() == 1 && m_document->isEmpty()) { // just added first frame...
        delete m_document;
        m_document = data->document();
        KWTextDocumentLayout *layout = new KWTextDocumentLayout(this);
        if(m_kwordDocument)
            layout->setInlineObjectTextManager(m_kwordDocument->inlineTextObjectManager());
        m_document->setDocumentLayout(layout);
        data->setDocument(m_document, false);
    }
    else {
        data->setDocument(m_document, false);
        data->setEndPosition(-1);
        data->faul();
        updateLayout();
    }
    connect (data, SIGNAL(relayout()), this, SLOT(updateLayout()));
}

void KWTextFrameSet::updateLayout(bool reset) {
    if(! m_allowLayoutRequests)
        return;
    if(! m_layoutTriggered) {
        QTimer::singleShot(0, this, SLOT(relayout()));
        if( reset ) {
            KWTextDocumentLayout *lay = dynamic_cast<KWTextDocumentLayout*>( m_document->documentLayout() );
            if(lay)
                lay->interruptLayout();
        }
    }
    m_layoutTriggered = true;
}

void KWTextFrameSet::scheduleLayout() {
    updateLayout(false);
}

void KWTextFrameSet::relayout() {
    m_layoutTriggered = false;
    QList<KWFrame*> dirtyFrames = m_frames;
    bool foundADirtyOne = false;
    KWFrame *firstDirtyFrame = 0;
    foreach(KWFrame *frame, m_frames) {
        KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (frame->shape()->userData());
        if(!firstDirtyFrame && data && data->isDirty())
            firstDirtyFrame = frame;
        if(! firstDirtyFrame)
            dirtyFrames.removeAll(frame);
    }
    qSort(m_frames.begin(), m_frames.end(), sortTextFrames); // make sure the ordering is proper
    if(foundADirtyOne) {
        // if the dirty frame has been resorted to no longer be the first one, then we should
        // mark dirty any frame that were previously later in the flow, but are now before it.
        foreach(KWFrame *frame, m_frames) {
            if(frame == firstDirtyFrame)
                break;
            if(dirtyFrames.contains(frame)) {
                static_cast<KoTextShapeData*> (frame->shape()->userData())->faul();
                // just the first is enough.
                break;
            }
        }
    }

    KWTextDocumentLayout *lay = dynamic_cast<KWTextDocumentLayout*>( m_document->documentLayout() );
    if(lay)
        lay->layout();
}

void KWTextFrameSet::requestMoreFrames(double textHeight) {
//kDebug() << "KWTextFrameSet::requestMoreFrames " << textHeight << endl;
    if(frameCount() == 0)
        return; // there is no way we can get more frames anyway.
    KWTextFrame *lastFrame = static_cast<KWTextFrame*> (frames()[frameCount()-1]);
    if(textHeight == 0.0 || lastFrame->frameBehavior() == KWord::AutoCreateNewFrameBehavior) {
        if(lastFrame->newFrameBehavior() == KWord::ReconnectNewFrame)
            emit moreFramesNeeded(this);
    }
    else if(lastFrame->frameBehavior() == KWord::AutoExtendFrameBehavior && lastFrame->canAutoGrow()) {
        // enlarge last shape
        KoShape *shape = lastFrame->shape();
        // TODO make the following work for rotated / skewed frames as well.  The position should be updated.
        QSizeF size = shape->size();
        shape->resize(QSizeF(size.width(), size.height() + textHeight));
        shape->repaint(QRectF(0.0, size.height(), size.width(), textHeight));
    }
}

void KWTextFrameSet::spaceLeft(double excessHeight) {
    Q_ASSERT(excessHeight >= 0);
    if(frameCount() == 0)
        return; // there is no way we can get more frames anyway.
    KWTextFrame *lastFrame = static_cast<KWTextFrame*> (frames()[frameCount()-1]);
    if(lastFrame->frameBehavior() == KWord::AutoExtendFrameBehavior)
        lastFrame->autoShrink(lastFrame->shape()->size().height() - excessHeight);
}

void KWTextFrameSet::framesEmpty(int framesInUse) {
    kDebug() << "KWTextFrameSet::framesEmpty " << framesInUse << endl;
}

void KWTextFrameSet::setAllowLayout(bool allow) {
    if(allow == m_allowLayoutRequests)
        return;
    m_allowLayoutRequests = allow;
    if(m_allowLayoutRequests)
        updateLayout(false);
}

// static
bool KWTextFrameSet::sortTextFrames(const KWFrame *frame1, const KWFrame *frame2) {
    const KWTextFrame *f1 = static_cast<const KWTextFrame*>(frame1);
    const KWTextFrame *f2 = static_cast<const KWTextFrame*>(frame2);

    if(f1->sortingId() >= 0 && f2->sortingId() >= 0) {
        return f1->sortingId() > f2->sortingId();
    }
    QPointF pos = f1->shape()->absolutePosition();
    QRectF bounds = f2->shape()->boundingRect();

    KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (f1->frameSet());
    if(tfs && tfs->pageManager()) { // check per page.
        KWPage *page1 = tfs->pageManager()->page(f1->shape());
        KWPage *page2 = tfs->pageManager()->page(f2->shape());
        if(page1 != page2 && page1 != 0 && page2 != 0)
            return page1->pageNumber() < page2->pageNumber();
    }

    // reverse the next 2 return values if the frameset is RTL
    if(pos.x() > bounds.right()) return false;
    if(pos.x() < bounds.left()) return true;

    // check the Y position. Y is greater only when it is below the second frame.
    if(pos.y() > bounds.bottom()) return false;
    if(pos.y() < bounds.top()) return true;

    // my center lies inside f2. Lets check the topleft pos.
    if(f1->shape()->boundingRect().top() > bounds.top()) return false;
    return true;
}

#ifndef NDEBUG
void KWTextFrameSet::printDebug(KWFrame *frame) {
    KWFrameSet::printDebug(frame);
    KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData*> (frame->shape()->userData());
    if(textShapeData == 0) return;
    kDebug() << "     Text position: " << textShapeData->position() << ", end: " << textShapeData->endPosition() << endl;
    kDebug() << "     Offset in text-document; " << textShapeData->documentOffset() << endl;
}

void KWTextFrameSet::printDebug() {
    static const char * type[] = { "FirstPageHeader", "OddPagesHeader", "EvenPagesHeader", "FirstPageFooter", "OddPagesFooter", "EvenPagesFooter", "Main", "FootNote", "Other", "ERROR" };
    kDebug() << " | Is a KWTextFrameSet" << endl;
    kDebug() << " | FS Type: " << type[m_textFrameSetType] << endl;
    KWFrameSet::printDebug();
}
#endif

#include "KWTextFrameSet.moc"
