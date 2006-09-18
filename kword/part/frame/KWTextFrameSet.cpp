/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include <KoTextShapeData.h>

#include <klocale.h>
#include <kdebug.h>
#include <QTextDocument>
#include <QTimer>

KWTextFrameSet::KWTextFrameSet()
    : m_document( new QTextDocument() ),
    m_protectContent(false),
    m_layoutTriggered(false),
    m_textFrameSetType( KWord::OtherTextFrameSet )
{
    m_document->setDocumentLayout(new KWTextDocumentLayout(this));
    m_document->setUseDesignMetrics(true);
}

KWTextFrameSet::KWTextFrameSet(KWord::TextFrameSetType type)
    : m_document( new QTextDocument() ),
    m_protectContent(false),
    m_layoutTriggered(false),
    m_textFrameSetType( type )
{
    m_document->setDocumentLayout(new KWTextDocumentLayout(this));
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
    if(m_textFrameSetType != KWord::OtherTextFrameSet) {
        frame->shape()->setLocked(true);
        if(m_textFrameSetType != KWord::MainTextFrameSet && frameCount() > 1)
            frame->setCopy(true);
    }
    // TODO sort frames
    KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (frame->shape()->userData());
    Q_ASSERT(data);
    if(frameCount() == 1 && m_document->isEmpty()) { // just added first frame...
        delete m_document;
        m_document = data->document();
        m_document->setDocumentLayout(new KWTextDocumentLayout(this));
        data->setDocument(m_document, false);
    }
    else {
        data->setDocument(m_document, false);
        data->faul();
        requestLayout();
    }
    connect (data, SIGNAL(relayout()), this, SLOT(requestLayout())); 
}

void KWTextFrameSet::requestLayout() {
    if(!m_layoutTriggered)
        QTimer::singleShot(0, this, SLOT(relayout()));
    m_layoutTriggered = true;
}

void KWTextFrameSet::relayout() {
    m_layoutTriggered = false;
    KWTextDocumentLayout *lay = dynamic_cast<KWTextDocumentLayout*>( m_document->documentLayout() );
    if(lay)
        lay->layout();
}

void KWTextFrameSet::requestMoreFrames() {
    if(frameCount() == 0)
        return; // there is no way we can get more frames anyway.
    KWFrame *lastFrame = frames()[frameCount()-1];
    switch(lastFrame->frameBehavior()) {
        case KWord::IgnoreContentFrameBehavior:
            return;
        case KWord::AutoCreateNewFrameBehavior:
            if(lastFrame->newFrameBehavior() == KWord::ReconnectNewFrame)
                emit moreFramesNeeded(this);
            break;
        case KWord::AutoExtendFrameBehavior:
                // TODO emit a request for more space, but not until we know how
                //      much (vertical) space we actually need.
            break;
    }
}

void KWTextFrameSet::framesEmpty(int framesInUse) {
    kDebug() << "KWTextFrameSet::framesEmpty " << framesInUse << endl;
}

#ifndef NDEBUG
void KWTextFrameSet::printDebug(KWFrame *frame) {
    KWFrameSet::printDebug(frame);
}

void KWTextFrameSet::printDebug() {
    static const char * type[] = { "FirstPageHeader", "OddPagesHeader", "EvenPagesHeader", "FirstPageFooter", "OddPagesFooter", "EvenPagesFooter", "Main", "FootNote", "Other", "ERROR" };
    kDebug() << " | Is a KWTextFrameSet" << endl;
    kDebug() << " | FS Type: " << type[m_textFrameSetType] << endl;
    KWFrameSet::printDebug();
}
#endif

#include "KWTextFrameSet.moc"
