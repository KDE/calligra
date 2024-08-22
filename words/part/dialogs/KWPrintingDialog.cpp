/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWPrintingDialog.h"

#include "KWDocument.h"
#include "KWPage.h"
#include "KWPageManager.h"
#include "KWView.h"
#include "frames/KWFrame.h"
#include "frames/KWTextFrameSet.h"

#include "KoCanvasBase.h"
#include <KoInsets.h>
#include <KoShapeManager.h>
#include <KoUnit.h>

#include <QApplication>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>

KWPrintingDialog::KWPrintingDialog(KWDocument *document, KoShapeManager *shapeManager, KWView *view)
    : KoPrintingDialog(view)
    , m_document(document)
    , m_view(view)
{
    setShapeManager(shapeManager);

    // while (! m_document->layoutFinishedAtleastOnce()) {
    //     QCoreApplication::processEvents();
    //     if (! QCoreApplication::hasPendingEvents())
    //         break;
    // }
    printer().setFromTo(documentFirstPage(), documentLastPage());
}

KWPrintingDialog::~KWPrintingDialog() = default;

QRectF KWPrintingDialog::preparePage(int pageNumber)
{
    const int resolution = printer().resolution();
    KWPage page = m_document->pageManager()->page(pageNumber);
    if (!page.isValid())
        return QRectF();
    printer().setPageSize(QPageSize(page.rect().size().toSize(), QPageSize::Unit::Point));

    KoInsets bleed = m_document->pageManager()->padding();
    const int bleedOffsetX = qRound(POINT_TO_INCH(bleed.left * resolution));
    const int bleedOffsetY = qRound(POINT_TO_INCH(bleed.top * resolution));
    const int bleedWidth = qRound(POINT_TO_INCH((bleed.left + bleed.right) * resolution));
    const int bleedHeight = qRound(POINT_TO_INCH((bleed.top + bleed.bottom) * resolution));

    const int pageOffset = qRound(POINT_TO_INCH(resolution * page.offsetInDocument()));
    painter().translate(0, -pageOffset);

    const int clipHeight = (int)POINT_TO_INCH(resolution * page.height());
    int clipWidth = (int)POINT_TO_INCH(resolution * page.width());
    int offsetX = -bleedOffsetX;

    return QRectF(offsetX, pageOffset - bleedOffsetY, clipWidth + bleedWidth, clipHeight + bleedHeight);
}

QList<KoShape *> KWPrintingDialog::shapesOnPage(int pageNumber)
{
    Q_ASSERT(pageNumber > 0);
    KWPage page = m_document->pageManager()->page(pageNumber);
    Q_ASSERT(page.isValid());
    return shapeManager()->shapesAt(page.rect());
}

void KWPrintingDialog::printingDone()
{
}

QList<QWidget *> KWPrintingDialog::createOptionWidgets() const
{
    return QList<QWidget *>();
}

int KWPrintingDialog::documentFirstPage() const
{
    return m_document->pageManager()->begin().pageNumber();
}

int KWPrintingDialog::documentLastPage() const
{
    KWPage lastPage = m_document->pageManager()->last();
    return lastPage.pageNumber();
}

int KWPrintingDialog::documentCurrentPage() const
{
    return m_view->currentPage().pageNumber();
}

QAbstractPrintDialog::PrintDialogOptions KWPrintingDialog::printDialogOptions() const
{
    return QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintPageRange | QAbstractPrintDialog::PrintCurrentPage
        | QAbstractPrintDialog::PrintCollateCopies | QAbstractPrintDialog::PrintShowPageSize;
}

// options;
//   DPI
//   fontEmbeddingEnabled
