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
}

KWTextFrameSet::KWTextFrameSet(KWord::TextFrameSetType type)
    : m_document( new QTextDocument() ),
    m_textFrameSetType( type )
{
    m_document->setDocumentLayout(new KWTextDocumentLayout(this));
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
m_document->setHtml("Pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom fap25 abcde tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzz. Laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple klm50 lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom faple abc75 tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gapl. Klmno pm100 pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom fa125 abcde tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple km150 lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumi. Qonaq issum daple ussum ronaq ossom faple ab175 tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple klmno pm200 pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom fa225 abcde tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple km250 lorem monaq morel plaff lerom baple merol plif. Ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom faple ab275 tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple klmno pm300 pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom fa325 abcde tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple km350 lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom faple ab375 tonaq fghij ga?le klmno vonaq pqrst haple uvwxy nonaq <p>zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple klmno pm400 pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple us;um ronaq ossom fa425 abcde tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple km450 lorem monaq morel plaff lerom baple merol pliff ipsum pona. Mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom faple ab475 tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple klmno pm500 pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom fa525 abcde tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum pona? Gaple km550 <p>lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom faple ab575 tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum p.Naq gaple klmno pm600 pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom fa625 abcde tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple km650 http: //des ktopp ub.mi ning. com/i ndex. htm!! ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom faple ab675 tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qon?Q issum daple ussum ponaq gaple klmno pm700 pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip <p>qonaq issum daple ussum ronaq ossom fa725 abcde tonaq fghij gaple klmno vonaq pq.St haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple km750 lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom faple ab775 tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple klmno pm800 pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom fa825 Deskt topub lishi ngatt heMin ingCo pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaf. Sumip qonaq issum daple ussum ponaq gaple km850 lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom faple ab875 tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq z.Z.z<p> laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple klmno pm900 pleff lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum daple ussum ronaq ossom fa925 abcde tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple km950 lorem monaq morel plaff lerom baple merol pliff ipsum ponaq mipsu ploff pimsu caple supim pluff sumip qonaq issum dapl. Ussum ronaq ossom faple ab975 tonaq fghij gaple klmno vonaq pqrst haple uvwxy nonaq zzzzz laple pleff lorem monaq morel plaff sumip qonaq issum daple ussum ponaq gaple klmno p1000");
            break;
        default: ;
    }

//if(m_textFrameSetType != KWord::MainTextFrameSet)
    m_document->setHtml(name() +"<br>"+ name()); // TEST :)
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
    if(frameCount() == 1 && m_document->isEmpty()) {
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

#include "KWTextFrameSet.moc"
