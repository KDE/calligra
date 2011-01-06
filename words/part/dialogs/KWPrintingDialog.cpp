/* This file is part of the KDE project
 * Copyright (C) 2007, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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
#include "frames/KWTextFrameSet.h"
#include "frames/KWFrame.h"

#include <KoInsets.h>
#include <KoShapeManager.h>
#include "KoCanvasBase.h"

#include <QApplication>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextDocument>

KWPrintingDialog::KWPrintingDialog(KWDocument *document, KoShapeManager *shapeManager, QWidget *parent)
        : KoPrintingDialog(parent),
        m_document(document)
{
    setShapeManager(shapeManager);

    while (! m_document->layoutFinishedAtleastOnce()) {
        QCoreApplication::processEvents();
        if (! QCoreApplication::hasPendingEvents())
            break;
    }
    printer().setFromTo(documentFirstPage(), documentLastPage());

    foreach (KWFrameSet *fs, m_document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs) {
            QTextDocument *doc = tfs->document();
            // TODO check if I can group changes into one undo-command and then redo it after printing.
            QTextBlock block = doc->begin();
            while (block.isValid()) {
                QTextLayout *layout = block.layout();
                if (layout)
                    layout->clearAdditionalFormats();
                block = block.next();
            }
        }
    }
}

KWPrintingDialog::~KWPrintingDialog()
{
}

QRectF KWPrintingDialog::preparePage(int pageNumber)
{
    const int resolution = printer().resolution();
    KWPage page = m_document->pageManager()->page(pageNumber);
    if (! page.isValid())
        return QRectF();
    printer().setPaperSize(page.rect(pageNumber).size(), QPrinter::Point);

    KoInsets bleed = m_document->pageManager()->padding();
    const int bleedOffsetX = qRound(POINT_TO_INCH(bleed.left * resolution));
    const int bleedOffsetY = qRound(POINT_TO_INCH(bleed.top * resolution));
    const int bleedWidth = qRound(POINT_TO_INCH((bleed.left + bleed.right) * resolution));
    const int bleedHeight = qRound(POINT_TO_INCH((bleed.top + bleed.bottom) * resolution));

    const int pageOffset = qRound(POINT_TO_INCH(resolution * page.offsetInDocument()));
    painter().translate(0, -pageOffset);

    qreal width = page.width();
    const int clipHeight = (int) POINT_TO_INCH(resolution * page.height());
    int clipWidth = (int) POINT_TO_INCH(resolution * page.width());
    int offsetX = -bleedOffsetX;
    if (page.pageSide() == KWPage::PageSpread) {
        width /= 2;
        clipWidth /= 2;
        if (pageNumber != page.pageNumber()) { // right side
            offsetX += clipWidth;
            painter().translate(-clipWidth, 0);
        }
    }

    return QRectF(offsetX, pageOffset - bleedOffsetY, clipWidth + bleedWidth, clipHeight + bleedHeight);
}

QList<KoShape*> KWPrintingDialog::shapesOnPage(int pageNumber)
{
    Q_ASSERT(pageNumber > 0);
    KWPage page = m_document->pageManager()->page(pageNumber);
    Q_ASSERT(page.isValid());
    return shapeManager()->shapesAt(page.rect());
}

void KWPrintingDialog::printingDone()
{
}

QList<QWidget*> KWPrintingDialog::createOptionWidgets() const
{
    return QList<QWidget*>();
}

int KWPrintingDialog::documentFirstPage() const
{
    return m_document->pageManager()->begin().pageNumber();
}

int KWPrintingDialog::documentLastPage() const
{
    KWPage lastPage = m_document->pageManager()->last();
    return lastPage.pageNumber() + (lastPage.pageSide() == KWPage::PageSpread ? 1 : 0);
}

QAbstractPrintDialog::PrintDialogOptions KWPrintingDialog::printDialogOptions() const
{
    return QAbstractPrintDialog::PrintToFile |
           QAbstractPrintDialog::PrintPageRange |
           QAbstractPrintDialog::PrintCollateCopies |
           QAbstractPrintDialog::DontUseSheet;
}

// options;
//   DPI
//   fontEmbeddingEnabled

