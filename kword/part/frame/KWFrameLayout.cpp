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

#include "KWFrameLayout.h"
#include "KWPageManager.h"
#include "KWTextFrameSet.h"
#include "KWTextFrame.h"
#include "KWPageSettings.h"
#include "KWPage.h"

#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>
#include <KoTextShape.h>
#include <KoShape.h>

#include <klocale.h>
#include <kdebug.h>

KWFrameLayout::KWFrameLayout(const KWPageManager *pageManager, QList<KWFrameSet*> frameSets, const KWPageSettings *pageSettings)
    : m_pageManager(pageManager),
    m_pageSettings(pageSettings),
    m_frameSets(frameSets),
    m_oddHeaders(0),
    m_evenHeaders(0),
    m_oddFooters(0),
    m_evenFooters(0),
    m_firstHeader(0),
    m_firstFooter(0),
    m_maintext(0),
    m_setup(false)
{
}

// pageNumber is a real page number, not a zero-based index
void KWFrameLayout::createNewFramesForPage(int pageNumber) {
    KWPage *page = m_pageManager->page(pageNumber);
    // create headers & footers
    KWord::TextFrameSetType origin;
    if(shouldCreateHeaderOrFooter(pageNumber, true, &origin))
        new KWTextFrame(createTextShape(page), getOrCreate(origin));
    if(shouldCreateHeaderOrFooter(pageNumber, false, &origin))
        new KWTextFrame(createTextShape(page), getOrCreate(origin));
    if(page->pageSide() == KWPage::PageSpread) {
        // inline helper method
        class PageSpreadShapeFactory {
           public:
            PageSpreadShapeFactory(KWFrameLayout *parent) {
                m_parent = parent;
            }
            KoTextShape *create(KWPage *page) {
                KoTextShape *shape = m_parent->createTextShape(page);
kDebug() << "shape pos; " << shape->position().x() << "," << shape->position().y() << endl;
                shape->setPosition(QPointF(page->width()/2+1, shape->position().y()));
kDebug() << "         ; " << shape->position().x() << "," << shape->position().y() << endl;
                return shape;
            }
            KWFrameLayout *m_parent;
        };
        PageSpreadShapeFactory factory(this);
        if(shouldCreateHeaderOrFooter(pageNumber+1, true, &origin))
            new KWTextFrame(factory.create(page), getOrCreate(origin));
        if(shouldCreateHeaderOrFooter(pageNumber+1, false, &origin))
            new KWTextFrame(factory.create(page), getOrCreate(origin));
        if(m_pageSettings->hasMainTextFrame())
            new KWTextFrame(factory.create(page), getOrCreate(KWord::MainTextFrameSet));
    }

    // create main text frame
    if(m_pageSettings->hasMainTextFrame())
        new KWTextFrame(createTextShape(page), getOrCreate(KWord::MainTextFrameSet));

    // copy frames from last 2 pages
    // TODO

    layoutFramesOnPage(pageNumber);
    if(page->pageSide() == KWPage::PageSpread)
        layoutFramesOnPage(pageNumber + 1);
}

void KWFrameLayout::layoutFramesOnPage(int pageNumber) {
kDebug() << "KWFrameLayout::layoutFramesOnPage" << endl;
/* assumes all frames are there and will do layouting of all the frames
    - headers/footers/main FS are positioned
    - normal frames are clipped to page */

    KWPage *page = m_pageManager->page(pageNumber);
    /* +-----------------+
       |  0              | <- m_pageSettings->pageLayout()->ptTop
       |  1  [ header ]  |
       |  2              | <- m_pageSettings->headerDistance()
       |  3  [ maintxt ] |
       |  4              | <- m_pageSettings->endNoteDistance()
       |  5  [ endnote ] |
       |  6              | <- m_pageSettings->footnoteDistance()
       |  7 [ footnote ] |
       |  8              | <- m_pageSettings->footerDistance()
       |  9  [ footer ]  |
       | 10               | <- m_pageSettings->pageLayout()->ptBottom
       +-----------------+ */

    // Create some data structures used for the layouting of the frames later
    double requestedHeight[11], minimumHeight[11], resultingPositions[11];
    for(int i=0; i < 11; i++) { // zero fill.
        requestedHeight[i] = 0;
        minimumHeight[i] = 0;
        resultingPositions[i] = 0;
    }
    minimumHeight[0] = m_pageSettings->pageLayout()->ptTop;
    minimumHeight[10] = m_pageSettings->pageLayout()->ptBottom;

    KoPageLayout layout = m_pageManager->pageLayout(pageNumber);
    double left = 0, width = page->width();
    if(page->pageSide() == KWPage::PageSpread) {
        width /= 2;
        if(page->pageNumber() != pageNumber) { // doing the 'right' one
            left = width;
            double x = layout.ptLeft; // swap margins
            layout.ptLeft = layout.ptRight;
            layout.ptRight = x;
        }
    }
    double textWidth = width - layout.ptLeft - layout.ptRight;

    KWTextFrame *main=0, *footer=0, *endnote=0, *header=0, *footnote=0;
    QRectF pageRect(left, page->offsetInDocument(), width, page->height());
    foreach(KWFrame *frame, framesInPage( pageRect )) {
        KWTextFrameSet *textFrameSet = dynamic_cast<KWTextFrameSet*> (frame->frameSet());
        if(textFrameSet == 0) continue;
        switch(textFrameSet->textFrameSetType()) {
            case KWord::FirstPageHeaderTextFrameSet:
            case KWord::OddPagesHeaderTextFrameSet:
            case KWord::EvenPagesHeaderTextFrameSet: {
                header = static_cast<KWTextFrame *> (frame);
                minimumHeight[1] = 10;
                requestedHeight[1] = header->shape()->size().height();
                minimumHeight[2] = m_pageSettings->headerDistance();
                break;
            }
            case KWord::FirstPageFooterTextFrameSet:
            case KWord::OddPagesFooterTextFrameSet:
            case KWord::EvenPagesFooterTextFrameSet: {
                footer = static_cast<KWTextFrame *> (frame);
                minimumHeight[9] = 10;
                requestedHeight[9] = footer->shape()->size().height();
                minimumHeight[8] = m_pageSettings->headerDistance();
                break;
            }
            case KWord::MainTextFrameSet: {
                main = static_cast<KWTextFrame *> (frame);
                minimumHeight[3] = 10;
                requestedHeight[3] = -1; // rest
                break;
            }
            // TODO end + foot note frameset
            default:;
        }
    }

    // spread space accross items.
    double heightLeft = page->height();
    for(int i=0; i < 11; i++)
        heightLeft -= qMax(minimumHeight[i], requestedHeight[i]);
    if(heightLeft >= 0) { // easy; plenty of space
        if(minimumHeight[5] > 0) // if we have an endnote
            minimumHeight[6] += heightLeft; // add space below endnote
        else
            minimumHeight[3] += heightLeft; // add space to main text frame
        double y=page->offsetInDocument();
        for(int i=0; i < 11; i++) {
            resultingPositions[i] = y;
            y += qMax(minimumHeight[i], requestedHeight[i]);
        }
    }
    else {
        // for situations where the header + footer are too big to fit together with a
        // minimum sized main text frame.
        minimumHeight[5] = 0; // no end note
        minimumHeight[7] = 0; // no footnote
        heightLeft = page->height();
        for(int i=0; i < 11; i++)
            heightLeft -= minimumHeight[i];
        double y=page->offsetInDocument();
        for(int i=0; i < 11; i++) {
            resultingPositions[i] = y;
            double row = minimumHeight[i];
            if(requestedHeight[i] > row) {
                row += heightLeft / 3;
            }
            y += row;
        }
    }

    // actually move / size the frames.
    if(main) {
        main->shape()->setPosition(
                QPointF(left + layout.ptLeft, resultingPositions[3]));
        main->shape()->resize( QSizeF(textWidth, resultingPositions[4] - resultingPositions[3]));
    }
    if(footnote) {
        footnote->shape()->setPosition(
                QPointF(left + layout.ptLeft, resultingPositions[7]));
        footnote->shape()->resize(QSizeF(textWidth, resultingPositions[8] - resultingPositions[7]));
    }
    if(endnote) {
        endnote->shape()->setPosition(
                QPointF(left + layout.ptLeft, resultingPositions[5]));
        endnote->shape()->resize( QSizeF(textWidth, resultingPositions[6] - resultingPositions[5]));
    }
    if(header) {
        header->shape()->setPosition(
                QPointF(left + layout.ptLeft, resultingPositions[1]));
        header->shape()->resize( QSizeF(textWidth, resultingPositions[2] - resultingPositions[1]));
    }
    if(footer) {
        footer->shape()->setPosition(
                QPointF(left + layout.ptLeft, resultingPositions[9]));
        footer->shape()->resize( QSizeF(textWidth, resultingPositions[10] - resultingPositions[9]));
    }
// TODO footnotes, endnotes
// TODO closer-to [binding/page edge]
}

bool KWFrameLayout::shouldCreateHeaderOrFooter(int pageNumber, bool header, KWord::TextFrameSetType *origin) {
    if(pageNumber == m_pageManager->startPage()) {
        if(header) {
            *origin = KWord::FirstPageHeaderTextFrameSet;
            return m_pageSettings->firstHeader() != KWord::HFTypeNone;
        }
        *origin = KWord::FirstPageFooterTextFrameSet;
        return m_pageSettings->firstFooter() != KWord::HFTypeNone;
    }
    // otherwise
    switch(header?m_pageSettings->headers():m_pageSettings->footers()) {
        case KWord::HFTypeNone: break;
        case KWord::HFTypeEvenOdd:
            if(header)
                *origin = pageNumber%2==0 ? KWord::EvenPagesHeaderTextFrameSet :
                    KWord::OddPagesHeaderTextFrameSet;
            else
                *origin = pageNumber%2==0 ? KWord::EvenPagesFooterTextFrameSet :
                    KWord::OddPagesFooterTextFrameSet;
            break;
        case KWord::HFTypeUniform:
            *origin = header?KWord::OddPagesHeaderTextFrameSet:KWord::OddPagesFooterTextFrameSet;
            break;
        case KWord::HFTypeSameAsFirst:
            *origin = header?KWord::FirstPageHeaderTextFrameSet:KWord::FirstPageFooterTextFrameSet;
            break;
    }
    if(header)
        return m_pageSettings->headers() != KWord::HFTypeNone;
    return m_pageSettings->footers() != KWord::HFTypeNone;
}

QList<KWFrame *> KWFrameLayout::framesInPage(QRectF page) {
    // hopefully replaced with a tree
    QList<KWFrame*> answer;
    foreach(KWFrameSet *fs, m_frameSets) {
        foreach(KWFrame *frame, fs->frames()) {
            if(page.contains(frame->shape()->absolutePosition()))
                answer.append(frame);
        }
    }
    return answer;
}

KWTextFrameSet *KWFrameLayout::getOrCreate(KWord::TextFrameSetType type) {
    setup();
    KWTextFrameSet **answer = 0;
    switch(type) {
        case KWord::FirstPageHeaderTextFrameSet:
            answer = &m_firstHeader; break;
        case KWord::OddPagesHeaderTextFrameSet:
            answer = &m_oddHeaders; break;
        case KWord::EvenPagesHeaderTextFrameSet:
            answer = &m_evenHeaders; break;
        case KWord::FirstPageFooterTextFrameSet:
            answer = &m_firstFooter; break;
        case KWord::OddPagesFooterTextFrameSet:
            answer = &m_oddFooters; break;
        case KWord::EvenPagesFooterTextFrameSet:
            answer = &m_evenFooters; break;
        case KWord::MainTextFrameSet:
            answer = &m_maintext; break;
        default:
            KWTextFrameSet *newFS = new KWTextFrameSet();
            m_frameSets.append(newFS);
            emit newFrameSet(newFS);
            return newFS;
    }
    Q_ASSERT(answer);
    if(*answer == 0) {
        KWTextFrameSet *newFS = new KWTextFrameSet(type);
        m_frameSets.append(newFS);
        emit newFrameSet(newFS);
        *answer = newFS;
    }
    return *answer;
}

void KWFrameLayout::updateFramesAfterDelete(int deletedPage) {
    // TODO
}

void KWFrameLayout::setup() {
    if(m_setup)
        return;
    foreach(KWFrameSet *fs, m_frameSets) {
        // add checks for out-of-area frames
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs);
        if(tfs) {
            switch(tfs->textFrameSetType()) {
                case KWord::FirstPageHeaderTextFrameSet:
                    m_firstHeader = tfs;
                    break;
                case KWord::OddPagesHeaderTextFrameSet:
                    m_oddHeaders = tfs;
                    break;
                case KWord::EvenPagesHeaderTextFrameSet:
                    m_evenHeaders = tfs;
                    break;
                case KWord::FirstPageFooterTextFrameSet:
                    m_firstFooter = tfs;
                    break;
                case KWord::OddPagesFooterTextFrameSet:
                    m_oddFooters = tfs;
                    break;
                case KWord::EvenPagesFooterTextFrameSet:
                    m_evenFooters = tfs;
                    break;
                case KWord::MainTextFrameSet:
                    m_maintext = tfs;
                default: ;// ignore
            }
        }
    }

/*
    QList<KWFrameSet*> createdFrameSets;
    if ( !m_firstHeader ) {
        KWTextFrameSet *fs = new KWTextFrameSet( KWord::FirstPageHeaderTextFrameSet );
        fs->setName(i18n("First Page Header"));
        KWTextFrame *tf = new KWTextFrame(createTextShape(), fs);
        tf->shape()->setVisible(false);
        tf->setFrameBehavior( KWord::AutoExtendFrameBehavior );
        tf->setNewFrameBehavior( KWord::CopyNewFrame );
        createdFrameSets.append(fs);
    }

    if ( !m_oddHeaders ) {
        KWTextFrameSet *fs = new KWTextFrameSet( KWord::OddPagesHeaderTextFrameSet );
        fs->setName(i18n("Odd Pages Header"));
        KWTextFrame *tf = new KWTextFrame(createTextShape(), fs);
        tf->shape()->setVisible(false);
        tf->setFrameBehavior( KWord::AutoExtendFrameBehavior );
        tf->setNewFrameBehavior( KWord::CopyNewFrame );
        createdFrameSets.append(fs);
    }

    if ( !m_evenHeaders ) {
        KWTextFrameSet *fs = new KWTextFrameSet( KWord::EvenPagesHeaderTextFrameSet );
        fs->setName(i18n("Even Pages Header"));
        KWTextFrame *tf = new KWTextFrame(createTextShape(), fs);
        tf->shape()->setVisible(false);
        tf->setFrameBehavior( KWord::AutoExtendFrameBehavior );
        tf->setNewFrameBehavior( KWord::CopyNewFrame );
        createdFrameSets.append(fs);
    }

    if ( !m_firstFooter ) {
        KWTextFrameSet *fs = new KWTextFrameSet( KWord::FirstPageFooterTextFrameSet );
        fs->setName(i18n("First Page Footer"));
        KWTextFrame *tf = new KWTextFrame(createTextShape(), fs);
        tf->shape()->setVisible(false);
        tf->setFrameBehavior( KWord::AutoExtendFrameBehavior );
        tf->setNewFrameBehavior( KWord::CopyNewFrame );
        createdFrameSets.append(fs);
    }

    if ( !m_oddFooters ) {
        KWTextFrameSet *fs = new KWTextFrameSet( KWord::OddPagesFooterTextFrameSet );
        fs->setName(i18n("Odd Pages Footer"));
        KWTextFrame *tf = new KWTextFrame(createTextShape(), fs);
        tf->shape()->setVisible(false);
        tf->setFrameBehavior( KWord::AutoExtendFrameBehavior );
        tf->setNewFrameBehavior( KWord::CopyNewFrame );
        createdFrameSets.append(fs);
    }

    if ( !m_evenFooters ) {
        KWTextFrameSet *fs = new KWTextFrameSet( KWord::EvenPagesFooterTextFrameSet );
        fs->setName(i18n("Even Pages Footer"));
        KWTextFrame *tf = new KWTextFrame(createTextShape(), fs);
        tf->shape()->setVisible(false);
        tf->setFrameBehavior( KWord::AutoExtendFrameBehavior );
        tf->setNewFrameBehavior( KWord::CopyNewFrame );
        createdFrameSets.append(fs);
    } */
    m_setup = true;
}

KoTextShape *KWFrameLayout::createTextShape(KWPage *page) {
    KoShapeFactory *factory = KoShapeRegistry::instance()->get(KoTextShape_SHAPEID);
    Q_ASSERT(factory);
    KoTextShape *shape = static_cast<KoTextShape*> (factory->createDefaultShape());
    shape->setPosition(QPointF(0, page->offsetInDocument()));
    return shape;
}

#include "KWFrameLayout.moc"
