/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "KWPrintingDialog.h"

#include "KWDocument.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWView.h"
#include "KWCanvas.h"
#include "frames/KWFrameSet.h"

#include <KoInsets.h>
#include <KoShapeManager.h>


// #define PRINT_VIA_KRITA

KWPrintingDialog::KWPrintingDialog(KWView *view)
    : KoPrintingDialog(view),
    m_document(view->kwdocument()),
    m_clipToPage(false)
{
    setShapeManager(view->kwcanvas()->shapeManager());
    printer().setFromTo(m_document->startPage(), m_document->lastPage());
    // TODO if the doc is not yet done layouting, the lastpage is not correct
}

KWPrintingDialog::~KWPrintingDialog() {
}

void KWPrintingDialog::preparePage(int pageNumber) {
    const int resolution = printer().resolution();
    KoInsets bleed = m_document->pageManager()->padding();
    const int bleedOffset = (int) (m_clipToPage?0:POINT_TO_INCH(-bleed.left * resolution));
    const int bleedWidth = (int) (m_clipToPage?0:POINT_TO_INCH((bleed.left + bleed.right) * resolution));
    const int bleedHeigt = (int) (m_clipToPage?0:POINT_TO_INCH((bleed.top + bleed.bottom) * resolution));

    KWPage *page = m_document->pageManager()->page(pageNumber);
    if(! page)
        return;
    QRectF pageRect = page->rect(pageNumber);
    pageRect.adjust(-bleed.left, -bleed.top, bleed.right, bleed.bottom);
    printer().setPaperSize(pageRect.size(), QPrinter::Point);
//qDebug() << "paperSize for page " << pageNumber << printer().paperSize(QPrinter::Point);
    const double offsetInDocument = page->offsetInDocument();
    // find images
    foreach(KWFrameSet *fs, m_document->frameSets()) {
        if(fs->frameCount() == 0) continue;
        KWImageFrame *image = dynamic_cast<KWImageFrame*> (fs->frames().at(0));
        if(image == 0) continue;
        if(m_originalImages.contains(image)) continue;
        QRectF bound = image->shape()->boundingRect();
        if(offsetInDocument > bound.bottom() || offsetInDocument + page->height() < bound.top())
            continue;
        m_originalImages.insert(image, image->imageQuality());
#ifdef PRINT_VIA_KRITA
        image->setImageQuality(KWImageFrame::EditableQuality);
#else
        image->setImageQuality(KWImageFrame::HighQuality);
#endif
        shapeManager()->add(image->shape()); // just in case the image change internally create a new shape
    }

    const int pageOffset = qRound(POINT_TO_INCH( resolution * offsetInDocument));

    painter().translate(0, -pageOffset);
    double width = page->width();
    int clipHeight = (int) POINT_TO_INCH( resolution * page->height());
    int clipWidth = (int) POINT_TO_INCH( resolution * page->width());
    int offset = bleedOffset;
    if(page->pageSide() == KWPage::PageSpread) {
        width /= 2;
        clipWidth /= 2;
        if(pageNumber != page->pageNumber()) { // right side
            offset += clipWidth;
            painter().translate(-clipWidth, 0);
        }
    }

    m_currentPage = QRectF(offset, pageOffset, clipWidth + bleedWidth, clipHeight + bleedHeigt);
    painter().setClipRect(m_currentPage);
}

QList<KoShape*> KWPrintingDialog::shapesOnPage(int pageNumber) {
    Q_ASSERT(pageNumber > 0);
    KWPage *page = m_document->pageManager()->page(pageNumber);
    return shapeManager()->shapesAt(page->rect());
}

void KWPrintingDialog::printingDone() {
    foreach(KWImageFrame *image, m_originalImages.keys())
        image->setImageQuality(m_originalImages[image]);
}

QList<QWidget*> KWPrintingDialog::createOptionWidgets() const {
    return QList<QWidget*>();
}

// options;
//   DPI
//   fontEmbeddingEnabled

