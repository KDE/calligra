/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PrintJob.h"
#include "Canvas.h"
#include "Doc.h"
#include "View.h"
#include "dialogs/SheetSelectPage.h"

#include "core/Cell.h"
#include "core/ColFormatStorage.h"
#include "core/HeaderFooter.h"
#include "core/Map.h"
#include "core/PrintSettings.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "core/SheetPrint.h"
#include "engine/SheetsDebug.h"

#include "ui/Selection.h"
#include "ui/SheetView.h"

#include <KoGlobal.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoUnit.h>
#include <KoZoomHandler.h>

#include <KMessageBox>

#include <QPainter>
#include <QPrintPreviewDialog>

using namespace Calligra::Sheets;

class PrintJob::Private
{
public:
    View *view;
    SheetSelectPage *sheetSelectPage;
    QVector<Sheet *> sheets;

public:
    int setupPages(const QPrinter &printer, bool forceRecreation = false);
    Sheet *getSheetPageNumber(int *sheetPageNumber) const;

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

int PrintJob::Private::setupPages(const QPrinter &printer, bool forceRecreation)
{
    // Create the list of sheet, that should be printed.
    sheets.clear();
    if (printer.printRange() == QPrinter::Selection)
        sheets.append(view->activeSheet());
    else if (sheetSelectPage->m_ui.allSheetsButton->isChecked()) {
        const QList<SheetBase *> sheetList = view->doc()->map()->sheetList();
        for (int i = 0; i < sheetList.count(); ++i) {
            Sheet *sheet = dynamic_cast<Sheet *>(sheetList[i]);
            if (sheet)
                sheets.append(sheet);
        }
    } else if (sheetSelectPage->m_ui.activeSheetButton->isChecked()) {
        sheets.append(view->activeSheet());
    } else if (sheetSelectPage->m_ui.selectedSheetsButton->isChecked()) {
        const QStringList sheetNames = sheetSelectPage->selectedSheets();
        for (int i = 0; i < sheetNames.count(); ++i) {
            SheetBase *bsheet = view->doc()->map()->findSheet(sheetNames[i]);
            Sheet *sheet = bsheet ? dynamic_cast<Sheet *>(bsheet) : nullptr;
            if (!sheet) {
                warnSheetsUI << i18n("Sheet %1 could not be found for printing", sheetNames[i]);
                continue;
            }
            sheets.append(sheet);
        }
    }

    // (Re-)Create the pages of the sheets.
    int pageCount = 0;
    for (Sheet *sheet : sheets) {
        SheetPrint *const pageManager = sheet->print();
        PrintSettings *settings = sheet->printSettings();
        // Set the print region, if the selection should be painted.
        // Temporarily! The print region is solely used for the page creation
        // of the current printout, but we are working with the permanent
        // SheetPrint object in this case.
        const Region printRegion = settings->printRegion();
        if (printer.printRange() == QPrinter::Selection)
            settings->setPrintRegion(*view->selection());
        sheet->setPrintSettings(*settings, forceRecreation);
        pageCount += pageManager->pageCount();
        if (printer.printRange() == QPrinter::Selection) {
            // Restore the former print region.
            settings->setPrintRegion(printRegion);
            sheet->setPrintSettings(*settings, true);
        }
    }
    return pageCount;
}

Sheet *PrintJob::Private::getSheetPageNumber(int *sheetPageNumber) const
{
    Q_ASSERT(sheetPageNumber);
    // Find the sheet specific page number.
    for (Sheet *sheet : sheets) {
        SheetPrint *const pageManager = sheet->print();
        if (*sheetPageNumber <= pageManager->pageCount())
            return sheet;
        *sheetPageNumber -= pageManager->pageCount();
    }
    return nullptr;
}

bool PrintJob::Private::pageNeedsPrinting(Sheet *sheet, const QRect &cellRange) const
{
    // TODO Stefan: Is there a better, faster approach?
    for (int row = cellRange.top(); row <= cellRange.bottom(); ++row) {
        for (int col = cellRange.left(); col <= cellRange.right(); ++col) {
            if (Cell(sheet, col, row).needsPrinting()) {
                return true;
            }
        }
    }

    QRectF shapesBoundingRect;
    const QList<KoShape *> shapes = sheet->shapes();
    for (int i = 0; i < shapes.count(); ++i) {
        shapesBoundingRect |= shapes[i]->boundingRect();
    }
    const QRect shapesCellRange = sheet->documentToCellCoordinates(shapesBoundingRect);
    return !(cellRange & shapesCellRange).isEmpty();
}

void PrintJob::Private::printHeaderFooter(QPainter &painter, Sheet *sheet, int pageNo) const
{
    const SheetPrint *const pageManager = sheet->print();
    const PrintSettings *const settings = sheet->printSettings();
    const KoPageLayout pageLayout = settings->pageLayout();

    const HeaderFooter *const headerFooter = sheet->headerFooter();
    int pageCount = pageManager->pageCount();
#if 1 // debug header/footer
    const QString headLeft = headerFooter->headLeft(pageNo, pageCount, sheet->sheetName());
    const QString headMid = headerFooter->headMid(pageNo, pageCount, sheet->sheetName());
    const QString headRight = headerFooter->headRight(pageNo, pageCount, sheet->sheetName());
    const QString footLeft = headerFooter->footLeft(pageNo, pageCount, sheet->sheetName());
    const QString footMid = headerFooter->footMid(pageNo, pageCount, sheet->sheetName());
    const QString footRight = headerFooter->footRight(pageNo, pageCount, sheet->sheetName());
#else // debug header/footer
    const QString headLeft = "HeaderLeft";
    const QString headMid = "HeaderMid";
    const QString headRight = "HeaderRight";
    const QString footLeft = "FooterLeft";
    const QString footMid = "FooterMid";
    const QString footRight = "FooterRight";
#endif // debug header/footer
    debugSheets << headLeft << headMid << headRight << footLeft << footMid << footRight;

    qreal textWidth;
    const qreal headFootDistance = MM_TO_POINT(5.0 /*mm*/);
    const qreal leftMarginDistance = MM_TO_POINT(5.0 /*mm*/);
    painter.setFont(KoGlobal::defaultFont());
    const QFontMetricsF fontMetrics = painter.fontMetrics();
    const qreal ascent = fontMetrics.ascent();

    // print head line left
    textWidth = fontMetrics.boundingRect(headLeft).width();
    if (textWidth > 0) {
        painter.drawText(leftMarginDistance, headFootDistance + ascent, headLeft);
    }

    // print head line middle
    textWidth = fontMetrics.boundingRect(headMid).width();
    if (textWidth > 0) {
        painter.drawText((pageLayout.width - textWidth) / 2.0, headFootDistance + ascent, headMid);
    }

    // print head line right
    textWidth = fontMetrics.boundingRect(headRight).width();
    if (textWidth > 0) {
        painter.drawText(pageLayout.width - textWidth - leftMarginDistance, headFootDistance + ascent, headRight);
    }

    // print foot line left
    textWidth = fontMetrics.boundingRect(footLeft).width();
    if (textWidth > 0) {
        painter.drawText(leftMarginDistance, pageLayout.height - headFootDistance, footLeft);
    }

    // print foot line middle
    textWidth = fontMetrics.boundingRect(footMid).width();
    if (textWidth > 0) {
        painter.drawText((pageLayout.width - textWidth) / 2.0, pageLayout.height - headFootDistance, footMid);
    }

    // print foot line right
    textWidth = fontMetrics.boundingRect(footRight).width();
    if (textWidth > 0) {
        painter.drawText(pageLayout.width - textWidth - leftMarginDistance, pageLayout.height - headFootDistance, footRight);
    }
}

PrintJob::PrintJob(View *view)
    : KoPrintingDialog(view)
    , d(new Private)
{
    d->view = view;
    d->sheetSelectPage = new SheetSelectPage();

    setShapeManager(static_cast<Canvas *>(d->view->canvas())->shapeManager());

    // Setup the pages.
    // Force the creation of pages.
    const int pageCount = d->setupPages(printer(), true);
    printer().setFromTo(1, pageCount);

    // apply page layout parameters
    Sheet *const sheet = d->view->activeSheet();
    const PrintSettings *settings = sheet->printSettings();
    const KoPageLayout pageLayout = settings->pageLayout();
    const KoPageFormat::Format pageFormat = pageLayout.format;
    printer().setPageSize(KoPageFormat::printerPageSize(pageFormat));
    if (pageLayout.orientation == KoPageFormat::Landscape || pageFormat == KoPageFormat::ScreenSize)
        printer().pageLayout().setOrientation(QPageLayout::Landscape);
    else
        printer().pageLayout().setOrientation(QPageLayout::Portrait);
    printer().setPageMargins({pageLayout.leftMargin, pageLayout.topMargin, pageLayout.rightMargin, pageLayout.bottomMargin}, QPageLayout::Point);
    printer().setFullPage(true);

    // debugSheetsUI <<"Iterating through available sheets and initializing list of available sheets.";
    QList<SheetBase *> sheetList = d->view->doc()->map()->sheetList();
    for (int i = sheetList.count() - 1; i >= 0; --i) {
        SheetBase *sheet = sheetList[i];
        // debugSheetsUI <<"Adding" << sheet->sheetName();
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
    return d->sheets.isEmpty() ? 0 : 1;
}

int PrintJob::documentLastPage() const
{
    int pageCount = 0;
    for (Sheet *sheet : d->sheets)
        pageCount += sheet->print()->pageCount();
    return pageCount;
}

void PrintJob::startPrinting(RemovePolicy removePolicy)
{
    // Setup the pages.
    // No recreation forced, because the sheet contents remained the same since the dialog was created.
    const int pageCount = d->setupPages(printer());

    // If there's nothing to print and this slot was not called by the print preview dialog ...
    if (pageCount == 0 && (!sender() || !qobject_cast<QPrintPreviewDialog *>(sender()))) {
        QStringList sheetNames;
        for (Sheet *sheet : d->sheets)
            sheetNames.append(sheet->sheetName());
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
    // and printPage() would not have been necessary - at least for Calligra Sheets.
    // In printPage() the painting of the sheet contents is done; some of the
    // QPainter settings from preparePage() get reverted.

    int sheetPageNumber = pageNumber;
    Sheet *sheet = d->getSheetPageNumber(&sheetPageNumber);
    if (!sheet)
        return QRectF();

    // Move the painter offset according to the page layout.
    const SheetPrint *const pageManager = sheet->print();
    const PrintSettings *const settings = sheet->printSettings();
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
        repeatedWidth += sheet->columnFormats()->totalVisibleColWidth(repeatedColumns.first, repeatedColumns.second);
    }
    double repeatedHeight = 0.0;
    const QPair<int, int> repeatedRows = settings->repeatedRows();
    if (repeatedRows.first != 0 && cellRange.top() > repeatedRows.second) {
        repeatedHeight += sheet->rowFormats()->totalVisibleRowHeight(repeatedRows.first, repeatedRows.second);
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
    return QRectF(pageRect.left() * scale, pageRect.top() * scale, pageRect.width() * scale, pageRect.height() * scale);
}

void PrintJob::printPage(int pageNumber, QPainter &painter)
{
    // See first comment in preparePage() about the distinction between the
    // printing of the sheet contents and shapes.

    debugSheetsRender << "Printing page" << pageNumber;
    int sheetPageNumber = pageNumber;
    Sheet *sheet = d->getSheetPageNumber(&sheetPageNumber);

    // Print the cells.
    if (!sheet) {
        return;
    }

    // Reset the offset made for shape printing.
    const double scale = POINT_TO_INCH(printer().resolution());
    const QRect cellRange = sheet->print()->cellRange(sheetPageNumber);
    const QRectF pageRect = sheet->print()->documentArea(sheetPageNumber);
    painter.translate(pageRect.left() * scale, pageRect.top() * scale);

    // Scale according to the printer's resolution.
    painter.scale(scale, scale);

    const PrintSettings *const settings = sheet->printSettings();
    const KoPageLayout pageLayout = settings->pageLayout();
    const double zoom = settings->zoom();
    debugSheets << "printing page" << sheetPageNumber << "; cell range" << cellRange;

    if (settings->printHeaders()) {
        painter.save();
        painter.resetTransform();
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
        repeatedWidth += sheet->columnFormats()->totalVisibleColWidth(repeatedColumns.first, repeatedColumns.second);
    }
    double repeatedHeight = 0.0;
    const QPair<int, int> repeatedRows = settings->repeatedRows();
    if (repeatedRows.first != 0 && cellRange.top() > repeatedRows.second) {
        repeatedHeight += sheet->rowFormats()->totalVisibleRowHeight(repeatedRows.first, repeatedRows.second);
    }

    // Paint top left part of the repeated columns/rows, if both are present.
    if (repeatedWidth > 0.0 && repeatedHeight > 0.0) {
        const QPointF topLeft(0, 0);
        const QRect range = QRect(QPoint(repeatedColumns.first, repeatedRows.first), QPoint(repeatedColumns.second, repeatedRows.second));
        const QRectF paintRect(0.0, 0.0, repeatedWidth, repeatedHeight);
        sheetView->setPaintCellRange(range);
        sheetView->paintCells(painter, paintRect, topLeft);
    }

    // Paint top part: the repeated rows, if present.
    if (repeatedHeight > 0.0) {
        const QPointF topLeft(repeatedWidth, 0);
        const QRect range = QRect(QPoint(cellRange.left(), repeatedRows.first), QPoint(cellRange.right(), repeatedRows.second));
        const QRectF paintRect(repeatedWidth, 0.0, pageRect.width(), repeatedHeight);
        sheetView->setPaintCellRange(range);
        sheetView->paintCells(painter, paintRect, topLeft);
    }

    // Paint left part: the repeated columns, if present.
    if (repeatedWidth > 0.0) {
        const QPointF topLeft(0, repeatedHeight);
        const QRect range = QRect(QPoint(repeatedColumns.first, cellRange.top()), QPoint(repeatedColumns.second, cellRange.bottom()));
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

QList<KoShape *> PrintJob::shapesOnPage(int pageNumber)
{
    // This method is called only for page preparation; to determine the shapes to wait for.
    int sheetPageNumber = pageNumber;
    Sheet *sheet = d->getSheetPageNumber(&sheetPageNumber);
    if (!sheet)
        return QList<KoShape *>();

    const QRectF documentArea = sheet->print()->documentArea(sheetPageNumber);
    return shapeManager()->shapesAt(documentArea);
}

QList<QWidget *> PrintJob::createOptionWidgets() const
{
    return QList<QWidget *>() << d->sheetSelectPage;
}

QAbstractPrintDialog::PrintDialogOptions PrintJob::printDialogOptions() const
{
    return QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintSelection | QAbstractPrintDialog::PrintPageRange
        | QAbstractPrintDialog::PrintCollateCopies;
}
