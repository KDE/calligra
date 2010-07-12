/* This file is part of the KDE project
 * Copyright 2008-2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

#include "PrintJob.h"

#include "Canvas.h"
#include "Doc.h"
#include "HeaderFooter.h"
#include "Map.h"
#include "PrintSettings.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "View.h"

#include "part/dialogs/SheetSelectPage.h"

#include "ui/SheetView.h"

#include <KoGlobal.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoZoomHandler.h>

#include <KMessageBox>

#include <QHash>
#include <QPainter>
#include <QPrintPreviewDialog>

using namespace KSpread;

typedef QHash<Sheet *, SheetPrint *> PageManagerMap;

class PrintJob::Private
{
public:
    View* view;
    SheetSelectPage* sheetSelectPage;
    PageManagerMap pageManagers;

public:
    int setupPages(const QPrinter& printer, bool forceRecreation = false);
    Sheet* getSheetPageNumber(int* sheetPageNumber) const;

    /**
     * Checks whether the page has content to print.
     * \param sheet the page's sheet
     * \param cellRange the page's cell range to check
     */
    bool pageNeedsPrinting(Sheet *sheet, const QRect &cellRange) const;

    /**
     * Prints the header and footer on a page.
     * \param painter the painter to use
     * \param sheet the page's sheet
     * \param page the page number
     */
    void printHeaderFooter(QPainter &painter, Sheet *sheet, int page) const;
};

int PrintJob::Private::setupPages(const QPrinter& printer, bool forceRecreation)
{
    // Create the list of sheet, that should be printed.
    pageManagers.clear();
    if (printer.printRange() == QPrinter::Selection)
        pageManagers.insert(view->activeSheet(), view->activeSheet()->print());
    else if (sheetSelectPage->allSheetsButton->isChecked()) {
        const QList<Sheet *> sheets = view->doc()->map()->sheetList();
        for (int i = 0; i < sheets.count(); ++i) {
            pageManagers.insert(sheets[i], sheets[i]->print());
        }
    } else if (sheetSelectPage->activeSheetButton->isChecked()) {
        pageManagers.insert(view->activeSheet(), view->activeSheet()->print());
    } else if (sheetSelectPage->selectedSheetsButton->isChecked()) {
        const QStringList sheetNames = sheetSelectPage->selectedSheets();
        for (int i = 0; i < sheetNames.count(); ++i) {
            Sheet* sheet = view->doc()->map()->findSheet(sheetNames[i]);
            if (sheet == 0) {
                kWarning(36005) << i18n("Sheet %1 could not be found for printing", sheetNames[i]);
                continue;
            }
            pageManagers.insert(sheet, sheet->print());
        }
    }

    // (Re-)Create the pages of the sheets.
    int pageCount = 0;
    const PageManagerMap::Iterator end(pageManagers.constEnd());
    for (PageManagerMap::Iterator it(pageManagers.constBegin()); it != end; ++it) {
        SheetPrint *const pageManager = *it;
        PrintSettings settings = *pageManager->settings();
        // Set the print region, if the selection should be painted.
        // Temporarily! The print region is solely used for the page creation
        // of the current printout, but we are working with the permanent
        // SheetPrint object in this case.
        const Region printRegion = settings.printRegion();
        if (printer.printRange() == QPrinter::Selection)
            settings.setPrintRegion(*view->selection());
        pageManager->setSettings(settings, forceRecreation);
        pageCount += pageManager->pageCount();
        if (printer.printRange() == QPrinter::Selection) {
            // Restore the former print region.
            settings.setPrintRegion(printRegion);
            pageManager->setSettings(settings, true);
        }
    }
    return pageCount;
}

Sheet* PrintJob::Private::getSheetPageNumber(int* sheetPageNumber) const
{
    Q_ASSERT(sheetPageNumber);
    // Find the sheet specific page number.
    Sheet* sheet = 0;
    const PageManagerMap::ConstIterator end(pageManagers.constEnd());
    for (PageManagerMap::ConstIterator it(pageManagers.constBegin()); it != end; ++it) {
        sheet = it.key();
        SheetPrint *const pageManager = *it;
        if (*sheetPageNumber <= pageManager->pageCount())
            break;
        *sheetPageNumber -= pageManager->pageCount();
    }
    return sheet;
}

bool PrintJob::Private::pageNeedsPrinting(Sheet * sheet, const QRect& cellRange) const
{
    // TODO Stefan: Is there a better, faster approach?
    for (int row = cellRange.top(); row <= cellRange.bottom() ; ++row) {
        for (int col = cellRange.left(); col <= cellRange.right(); ++col) {
            if (Cell(sheet, col, row).needsPrinting()) {
                return true;
            }
        }
    }

    QRectF shapesBoundingRect;
    const QList<KoShape*> shapes = sheet->shapes();
    for (int i = 0; i < shapes.count(); ++i) {
        shapesBoundingRect |= shapes[i]->boundingRect();
    }
    const QRect shapesCellRange = sheet->documentToCellCoordinates(shapesBoundingRect);
    return !(cellRange & shapesCellRange).isEmpty();
}

void PrintJob::Private::printHeaderFooter(QPainter &painter, Sheet *sheet, int pageNo) const
{
    const SheetPrint *const pageManager = pageManagers[sheet];
    const PrintSettings *const settings = pageManager->settings();
    const KoPageLayout pageLayout = settings->pageLayout();

    const HeaderFooter *const headerFooter = pageManager->headerFooter();
#if 1 // debug header/footer
    const QString headLeft = headerFooter->headLeft(pageNo, sheet->sheetName());
    const QString headMid = headerFooter->headMid(pageNo, sheet->sheetName());
    const QString headRight = headerFooter->headRight(pageNo, sheet->sheetName());
    const QString footLeft = headerFooter->footLeft(pageNo, sheet->sheetName());
    const QString footMid = headerFooter->footMid(pageNo, sheet->sheetName());
    const QString footRight = headerFooter->footRight(pageNo, sheet->sheetName());
#else // debug header/footer
    const QString headLeft = "HeaderLeft";
    const QString headMid = "HeaderMid";
    const QString headRight = "HeaderRight";
    const QString footLeft = "FooterLeft";
    const QString footMid = "FooterMid";
    const QString footRight = "FooterRight";
#endif // debug header/footer
    kDebug() << headLeft << headMid << headRight << footLeft << footMid << footRight;

    qreal textWidth;
    const qreal headFootDistance = MM_TO_POINT(5.0 /*mm*/);
    const qreal leftMarginDistance = MM_TO_POINT(5.0 /*mm*/);
    painter.setFont(KoGlobal::defaultFont());
    const QFontMetricsF fontMetrics = painter.fontMetrics();
    const qreal ascent = fontMetrics.ascent();

    // print head line left
    textWidth = fontMetrics.width(headLeft);
    if (textWidth > 0) {
        painter.drawText(leftMarginDistance,
                         headFootDistance + ascent,
                         headLeft);
    }

    // print head line middle
    textWidth = fontMetrics.width(headMid);
    if (textWidth > 0) {
        painter.drawText((pageLayout.width - textWidth) / 2.0,
                         headFootDistance + ascent,
                         headMid);
    }

    // print head line right
    textWidth = fontMetrics.width(headRight);
    if (textWidth > 0) {
        painter.drawText(pageLayout.width - textWidth - leftMarginDistance,
                         headFootDistance + ascent,
                         headRight);
    }

    // print foot line left
    textWidth = fontMetrics.width(footLeft);
    if (textWidth > 0) {
        painter.drawText(leftMarginDistance,
                         pageLayout.height - headFootDistance,
                         footLeft);
    }

    // print foot line middle
    textWidth = fontMetrics.width(footMid);
    if (textWidth > 0) {
        painter.drawText((pageLayout.width - textWidth) / 2.0,
                         pageLayout.height - headFootDistance,
                         footMid);
    }

    // print foot line right
    textWidth = fontMetrics.width(footRight);
    if (textWidth > 0) {
        painter.drawText(pageLayout.width - textWidth - leftMarginDistance,
                         pageLayout.height - headFootDistance,
                         footRight);
    }
}


PrintJob::PrintJob(View *view)
        : KoPrintingDialog(view)
        , d(new Private)
{
    d->view = view;
    d->sheetSelectPage = new SheetSelectPage();

    setShapeManager(static_cast<Canvas*>(d->view->canvas())->shapeManager());

    // Setup the pages.
    // Force the creation of pages.
    const int pageCount = d->setupPages(printer(), true);
    printer().setFromTo(1, pageCount);

    //apply page layout parameters
    Sheet *const sheet = d->view->activeSheet();
    const PrintSettings* settings = d->pageManagers[sheet]->settings();
    const KoPageLayout pageLayout = settings->pageLayout();
    const KoPageFormat::Format pageFormat = pageLayout.format;
    printer().setPaperSize(static_cast<QPrinter::PageSize>(KoPageFormat::printerPageSize(pageFormat)));
    if (pageLayout.orientation == KoPageFormat::Landscape || pageFormat == KoPageFormat::ScreenSize)
        printer().setOrientation(QPrinter::Landscape);
    else
        printer().setOrientation(QPrinter::Portrait);
    printer().setPageMargins(pageLayout.leftMargin, pageLayout.topMargin,
                             pageLayout.rightMargin, pageLayout.bottomMargin,
                             QPrinter::Point);
    printer().setFullPage(true);

    //kDebug(36005) <<"Iterating through available sheets and initializing list of available sheets.";
    QList<Sheet*> sheetList = d->view->doc()->map()->sheetList();
    for (int i = sheetList.count() - 1; i >= 0; --i) {
        Sheet* sheet = sheetList[ i ];
        //kDebug(36005) <<"Adding" << sheet->sheetName();
        d->sheetSelectPage->prependAvailableSheet(sheet->sheetName());
    }
}

PrintJob::~PrintJob()
{
//     delete d->sheetSelectPage; // QPrintDialog takes ownership
    delete d;
}

int PrintJob::documentFirstPage() const
{
    return d->pageManagers.isEmpty() ? 0 : 1;
}

int PrintJob::documentLastPage() const
{
    int pageCount = 0;
    const PageManagerMap::ConstIterator end(d->pageManagers.constEnd());
    for (PageManagerMap::ConstIterator it(d->pageManagers.constBegin()); it != end; ++it) {
        pageCount += (*it)->pageCount();
    }
    return pageCount;
}

void PrintJob::startPrinting(RemovePolicy removePolicy)
{
    // Setup the pages.
    // No recreation forced, because the sheet contents remained the same since the dialog was created.
    const int pageCount = d->setupPages(printer());

    // If there's nothing to print and this slot was not called by the print preview dialog ...
    if (pageCount == 0 && (!sender() || !qobject_cast<QPrintPreviewDialog*>(sender()))) {
        QStringList sheetNames;
        const PageManagerMap::ConstIterator end(d->pageManagers.constEnd());
        for (PageManagerMap::ConstIterator it(d->pageManagers.constBegin()); it != end; ++it) {
            sheetNames.append(it.key()->sheetName());
        }
        KMessageBox::information(d->view, i18n("Nothing to print for sheet(s) %1.", sheetNames.join(", ")));
        return;
    }

    // Start the printing.
    KoPrintingDialog::startPrinting(removePolicy);
}

QRectF PrintJob::preparePage(int pageNumber)
{
    // The printing of shapes is done by KoPrintingDialog, which needs the
    // QPainter set up properly. Otherwise, the separation into preparePage()
    // and printPage() would not have been necessary - at least for KSpread.
    // In printPage() the painting of the sheet contents is done; some of the
    // QPainter settings from preparePage() get reverted.

    int sheetPageNumber = pageNumber;
    Sheet* sheet = d->getSheetPageNumber(&sheetPageNumber);
    if (!sheet)
        return QRectF();

    // Move the painter offset according to the page layout.
    const SheetPrint *const pageManager = d->pageManagers[sheet];
    const PrintSettings *const settings = pageManager->settings();
    // Everything in the painting logic is done in logical, device-independent
    // coordinates. The painting logic uses KoZoomHandler/KoViewConverter. The
    // mapping between logical and physical coordinates has to be done by
    // QPainter, merely QPaintEngine.
    // For screen painting: QWidget's paint engine does not seem to do the mapping,
    // otherwise we would not have to take the screen resolution into account by
    // KoZoomHandler.
    // Question is: Does QPrinter's engine maps the logical to physical points?
    // Seems not: KoPrintingDialog uses KoZoomHandler in conjunction with the
    // printer's resolution and a zoom factor of 1 for the shape painting.
    // QPrinter::ScreenResolution is default
    //
    // So, what's left is to apply the zoom factor.
    // And, to scale the coordinates with the logical-to-physical factor.
    //
    // POINT_TO_INCH(x) = x / 72 = x * 0.01388888888889
    //
    // Example: screen resolution = 86 dpi, zoom = 2
    // Screen: 86 / 72 * 2 = 2.39 [pixels/inch / points/inch = pixels/points = physical points/logical points]
    // Example: printer resolution 300 dpi, zoom = 2
    // Printer: 300 / 72 * 2 = 8.33 [physical points/logical points]
    const double scale = POINT_TO_INCH(printer().resolution());
    const KoPageLayout pageLayout = settings->pageLayout();
    painter().translate(pageLayout.leftMargin * scale, pageLayout.topMargin * scale);

    // Apply the print zoom factor,
    const double zoom = settings->zoom();
    painter().scale(zoom, zoom);

    // Prepare the page for shape printing.
    const QRect cellRange = pageManager->cellRange(sheetPageNumber);
    QRectF pageRect = pageManager->documentArea(sheetPageNumber);
    painter().translate(-pageRect.left() * scale, -pageRect.top() * scale);

    // Calculate the dimensions of the repeated columns/rows first.
    double repeatedWidth = 0.0;
    const QPair<int, int> repeatedColumns = settings->repeatedColumns();
    if (repeatedColumns.first != 0 && cellRange.left() > repeatedColumns.second) {
        for (int col = repeatedColumns.first; col <= repeatedColumns.second; ++col) {
            repeatedWidth += sheet->columnFormat(col)->visibleWidth();
        }
    }
    double repeatedHeight = 0.0;
    const QPair<int, int> repeatedRows = settings->repeatedRows();
    if (repeatedRows.first != 0 && cellRange.top() > repeatedRows.second) {
        for (int row = repeatedRows.first; row <= repeatedRows.second; ++row) {
            repeatedHeight += sheet->rowFormat(row)->visibleHeight();
        }
    }

    // Center the table on the page.
    if (settings->centerHorizontally()) {
        const double pageWidth = pageRect.width() + repeatedWidth;
        const double printWidth = settings->printWidth();
        const double offset = 0.5 * (printWidth / zoom - pageWidth);
        painter().translate(offset * scale, 0.0);
        pageRect.moveLeft(offset); // scale will be applied below
    }
    if (settings->centerVertically()) {
        const double pageHeight = pageRect.height() + repeatedHeight;
        const double printHeight = settings->printHeight();
        const double offset = 0.5 * (printHeight / zoom - pageHeight);
        painter().translate(0.0, offset * scale);
        pageRect.moveTop(offset); // scale will be applied below
    }
    return QRectF(pageRect.left() * scale, pageRect.top() * scale,
                  pageRect.width() * scale, pageRect.height() * scale);
}

void PrintJob::printPage(int pageNumber, QPainter &painter)
{
    // See first comment in preparePage() about the distinction between the
    // printing of the sheet contents and shapes.

    kDebug(36004) << "Printing page" << pageNumber;
    int sheetPageNumber = pageNumber;
    Sheet* sheet = d->getSheetPageNumber(&sheetPageNumber);

    // Print the cells.
    if (!sheet) {
        return;
    }

    // Reset the offset made for shape printing.
    const double scale = POINT_TO_INCH(printer().resolution());
    const QRect cellRange = d->pageManagers[sheet]->cellRange(sheetPageNumber);
    const QRectF pageRect = d->pageManagers[sheet]->documentArea(sheetPageNumber);
    painter.translate(pageRect.left() * scale, pageRect.top() * scale);

    // Scale according to the printer's resolution.
    painter.scale(scale, scale);

    const SheetPrint *const pageManager = d->pageManagers[sheet];
    const PrintSettings *const settings = pageManager->settings();
    const KoPageLayout pageLayout = settings->pageLayout();
    const double zoom = settings->zoom();
    kDebug() << "printing page" << sheetPageNumber << "; cell range" << cellRange;

    if (settings->printHeaders()) {
        painter.save();
        painter.resetMatrix();
        painter.scale(scale, scale); // no zooming; just resolution
        painter.setClipping(false);
        d->printHeaderFooter(painter, sheet, pageNumber);
        painter.restore();
    }

    // setup the QPainter
    painter.save();
    painter.setClipRect(0.0, 0.0, pageLayout.width / zoom, pageLayout.height / zoom);

    // setup the SheetView
    SheetView *const sheetView = d->view->sheetView(sheet);
    const KoViewConverter *const origViewConverter = sheetView->viewConverter();

    KoZoomHandler zoomHandler;
    zoomHandler.setZoom(zoom);
    sheetView->setViewConverter(&zoomHandler);

    // save and set painting flags
    const bool grid = sheet->getShowGrid();
    const bool commentIndicator = sheet->getShowCommentIndicator();
    const bool formulaIndicator = sheet->getShowFormulaIndicator();
    sheet->setShowGrid(settings->printGrid());
    sheet->setShowCommentIndicator(settings->printCommentIndicator());
    sheet->setShowFormulaIndicator(settings->printFormulaIndicator());

    // Calculate the dimensions of the repeated columns/rows first.
    double repeatedWidth = 0.0;
    const QPair<int, int> repeatedColumns = settings->repeatedColumns();
    if (repeatedColumns.first != 0 && cellRange.left() > repeatedColumns.second) {
        for (int col = repeatedColumns.first; col <= repeatedColumns.second; ++col) {
            repeatedWidth += sheet->columnFormat(col)->visibleWidth();
        }
    }
    double repeatedHeight = 0.0;
    const QPair<int, int> repeatedRows = settings->repeatedRows();
    if (repeatedRows.first != 0 && cellRange.top() > repeatedRows.second) {
        for (int row = repeatedRows.first; row <= repeatedRows.second; ++row) {
            repeatedHeight += sheet->rowFormat(row)->visibleHeight();
        }
    }

    // Paint top left part of the repeated columns/rows, if both are present.
    if (repeatedWidth > 0.0 && repeatedHeight > 0.0) {
        const QPointF topLeft(0, 0);
        const QRect range = QRect(QPoint(repeatedColumns.first, repeatedRows.first),
                                  QPoint(repeatedColumns.second, repeatedRows.second));
        const QRectF paintRect(0.0, 0.0, repeatedWidth, repeatedHeight);
        sheetView->setPaintCellRange(range);
        sheetView->paintCells(painter, paintRect, topLeft);
    }

    // Paint top part: the repeated rows, if present.
    if (repeatedHeight > 0.0) {
        const QPointF topLeft(repeatedWidth, 0);
        const QRect range = QRect(QPoint(cellRange.left(), repeatedRows.first),
                                  QPoint(cellRange.right(), repeatedRows.second));
        const QRectF paintRect(repeatedWidth, 0.0, pageRect.width(), repeatedHeight);
        sheetView->setPaintCellRange(range);
        sheetView->paintCells(painter, paintRect, topLeft);
    }

    // Paint left part: the repeated columns, if present.
    if (repeatedWidth > 0.0) {
        const QPointF topLeft(0, repeatedHeight);
        const QRect range = QRect(QPoint(repeatedColumns.first, cellRange.top()),
                                  QPoint(repeatedColumns.second, cellRange.bottom()));
        const QRectF paintRect(0.0, repeatedHeight, repeatedWidth, pageRect.height());
        sheetView->setPaintCellRange(range);
        sheetView->paintCells(painter, paintRect, topLeft);
    }

    // Paint the actual cell range.
    const QPointF topLeft(repeatedWidth, repeatedHeight);
    const QRectF paintRect(topLeft, pageRect.size());
    sheetView->setPaintCellRange(cellRange);
    sheetView->paintCells(painter, paintRect, topLeft);

    // restore painting flags
    sheet->setShowGrid(grid);
    sheet->setShowCommentIndicator(commentIndicator);
    sheet->setShowFormulaIndicator(formulaIndicator);
    sheetView->setViewConverter(origViewConverter);
    painter.restore();
}

QList<KoShape*> PrintJob::shapesOnPage(int pageNumber)
{
    // This method is called only for page preparation; to determine the shapes to wait for.
    int sheetPageNumber = pageNumber;
    Sheet* sheet = d->getSheetPageNumber(&sheetPageNumber);
    if (!sheet)
        return QList<KoShape*>();

    const QRectF documentArea = d->pageManagers[sheet]->documentArea(sheetPageNumber);
    return shapeManager()->shapesAt(documentArea);
}

QList<QWidget*> PrintJob::createOptionWidgets() const
{
    return QList<QWidget*>() << d->sheetSelectPage;
}

QAbstractPrintDialog::PrintDialogOptions PrintJob::printDialogOptions() const
{
    return QAbstractPrintDialog::PrintToFile |
           QAbstractPrintDialog::PrintSelection |
           QAbstractPrintDialog::PrintPageRange |
           QAbstractPrintDialog::PrintCollateCopies |
           QAbstractPrintDialog::DontUseSheet;
}
