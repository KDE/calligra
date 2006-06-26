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
#include "KWFrame.h"

#include <KoTextShapeData.h>

#include <klocale.h>
#include <kdebug.h>
#include <QTextDocument>

KWTextFrameSet::KWTextFrameSet()
    : m_document( new QTextDocument() ),
    m_textFrameSetType( KWord::OtherTextFrameSet )
{
}

KWTextFrameSet::KWTextFrameSet(KWord::TextFrameSetType type)
    : m_document( new QTextDocument() ),
    m_textFrameSetType( type )
{
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

m_document->setHtml(name() +"<br>"+ name()); // TEST :)
}

KWTextFrameSet::~KWTextFrameSet() {
    delete m_document;
}

void KWTextFrameSet::setupFrame(KWFrame *frame) {
    if(m_textFrameSetType != KWord::OtherTextFrameSet)
        frame->shape()->setLocked(true);
    if(frameCount() == 1 && m_document->isEmpty()) {
        delete m_document;
        KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (frame->shape()->userData());
        Q_ASSERT(data);
        m_document = data->document();
    }
    else {
        KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (frame->shape()->userData());
        Q_ASSERT(data);
        data->setDocument(m_document, false);
    }
}
