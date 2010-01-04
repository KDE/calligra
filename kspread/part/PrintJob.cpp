/* This file is part of the KDE project
 * Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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
#include "Map.h"
#include "PrintManager.h"
#include "PrintSettings.h"
#include "Selection.h"
#include "Sheet.h"
#include "View.h"
#include "SheetPrint.h"

#include "part/dialogs/SheetSelectPage.h"

#include <KoGlobal.h>
#include <KoShapeManager.h>

#include <KMessageBox>

#include <QHash>
#include <QPainter>
#include <QPrintPreviewDialog>

using namespace KSpread;

class PrintJob::Private
{
public:
    View* view;
    SheetSelectPage* sheetSelectPage;
    QList<Sheet*> selectedSheets;
    QHash<Sheet*, PrintManager*> printManagers;

public:
    int setupPages(const QPrinter& printer, bool forceRecreation = false);
    Sheet* getSheetPageNumber(int* sheetPageNumber);
    PrintManager* printManager(Sheet* sheet);
};

int PrintJob::Private::setupPages(const QPrinter& printer, bool forceRecreation)
{
    // Create the list of sheet, that should be printed.
    selectedSheets.clear();
    if (printer.printRange() == QPrinter::Selection)
        selectedSheets.append(view->activeSheet());
    else if (sheetSelectPage->allSheetsButton->isChecked())
        selectedSheets = view->doc()->map()->sheetList();
    else if (sheetSelectPage->activeSheetButton->isChecked())
        selectedSheets.append(view->activeSheet());
    else if (sheetSelectPage->selectedSheetsButton->isChecked()) {
        const QStringList sheetNames = sheetSelectPage->selectedSheets();
        for (int i = 0; i < sheetNames.count(); ++i) {
            Sheet* sheet = view->doc()->map()->findSheet(sheetNames[i]);
            if (sheet == 0) {
                kWarning(36005) << i18n("Sheet %1 could not be found for printing", sheetNames[i]);
                continue;
            }
            selectedSheets.append(sheet);
        }
    }

    // (Re-)Create the pages of the sheets.
    int pageCount = 0;
    for (int i = 0; i < selectedSheets.count(); ++i) {
        PrintSettings settings = *selectedSheets[i]->printSettings();
        // Set the print region, if the selection should be painted.
        if (printer.printRange() == QPrinter::Selection)
            settings.setPrintRegion(*view->selection());
        printManager(selectedSheets[i])->setPrintSettings(settings, forceRecreation);
        pageCount += printManager(selectedSheets[i])->pageCount();
    }
    return pageCount;
}

Sheet* PrintJob::Private::getSheetPageNumber(int* sheetPageNumber)
{
    Q_ASSERT(sheetPageNumber);
    // Find the sheet specific page number.
    Sheet* sheet = 0;
    for (int i = 0; i < selectedSheets.count(); ++i) {
        sheet = selectedSheets[i];
        if (*sheetPageNumber <= printManager(sheet)->pageCount())
            break;
        *sheetPageNumber -= printManager(sheet)->pageCount();
    }
    return sheet;
}

PrintManager* PrintJob::Private::printManager(Sheet* sheet)
{
    if (!printManagers.contains(sheet))
        printManagers.insert(sheet, new PrintManager(sheet));
    return printManagers[sheet];
}


PrintJob::PrintJob(View *view)
        : KoPrintingDialog(view)
        , d(new Private)
{
    d->view = view;
    d->sheetSelectPage = new SheetSelectPage();

    setShapeManager(static_cast<Canvas*>(d->view->canvas())->shapeManager());

    //apply page layout parameters
    const PrintSettings* settings = d->view->activeSheet()->printSettings();
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

    // Setup the pages.
    // Force the creation of pages.
    const int pageCount = d->setupPages(printer(), true);
    printer().setFromTo(1, pageCount);
}

PrintJob::~PrintJob()
{
    qDeleteAll(d->printManagers);
//     delete d->sheetSelectPage; // QPrintDialog takes ownership
    delete d;
}

int PrintJob::documentFirstPage() const
{
    return d->selectedSheets.isEmpty() ? 0 : 1;
}

int PrintJob::documentLastPage() const
{
    const QList<Sheet*> sheets = d->selectedSheets;
    int pageCount = 0;
    for (int i = 0; i < sheets.count(); ++i)
        pageCount += d->printManager(sheets[i])->pageCount();
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
        for (int i = 0; i < d->selectedSheets.count(); ++i)
            sheetNames.append(d->selectedSheets[i]->sheetName());
        KMessageBox::information(d->view, i18n("Nothing to print for sheet(s) %1.", sheetNames.join(", ")));
        return;
    }

    // Start the printing.
    KoPrintingDialog::startPrinting(removePolicy);
}

QRectF PrintJob::preparePage(int pageNumber)
{
    int sheetPageNumber = pageNumber;
    Sheet* sheet = d->getSheetPageNumber(&sheetPageNumber);
    if (!sheet)
        return QRectF();

    // Move the painter offset according to the page layout.
    const double scale = POINT_TO_INCH(printer().resolution());
    const KoPageLayout pageLayout = sheet->printSettings()->pageLayout();
    painter().translate(pageLayout.leftMargin * scale, pageLayout.topMargin * scale);

    // Apply the print zoom factor,
    const double zoom = d->printManager(sheet)->zoom();
    painter().scale(zoom, zoom);

    // Prepare the page for shape printing.
    const QRect cellRange = d->printManager(sheet)->cellRange(sheetPageNumber);
    QRectF pageRect = sheet->cellCoordinatesToDocument(cellRange);
    painter().translate(-pageRect.left() * scale, -pageRect.top() * scale);

    // Center the table on the page.
    if (sheet->printSettings()->centerHorizontally()) {
        const double printWidth = sheet->printSettings()->printWidth(); // FIXME respect repeated columns
        const double offset = 0.5 * (printWidth / zoom - pageRect.width());
        painter().translate(offset * scale, 0.0);
        pageRect.moveLeft(offset); // scale will be applied below
    }
    if (sheet->printSettings()->centerVertically()) {
        const double printHeight = sheet->printSettings()->printHeight(); // FIXME respect repeated rows
        const double offset = 0.5 * (printHeight / zoom - pageRect.height());
        painter().translate(0.0, offset * scale);
        pageRect.moveTop(offset); // scale will be applied below
    }
    sheet->print()->printHeaderFooter(painter(), sheetPageNumber);
    return QRectF(pageRect.left() * scale, pageRect.top() * scale,
                  pageRect.width() * scale, pageRect.height() * scale);
}

void PrintJob::printPage(int pageNumber, QPainter &painter)
{
    kDebug(36004) << "Printing page" << pageNumber;
    int sheetPageNumber = pageNumber;
    Sheet* sheet = d->getSheetPageNumber(&sheetPageNumber);

    // Print the cells.
    if (sheet) {
        // Reset the offset made for shape printing.
        const double scale = POINT_TO_INCH(printer().resolution());
        const QRect cellRange = d->printManager(sheet)->cellRange(sheetPageNumber);
        const QRectF pageRect = sheet->cellCoordinatesToDocument(cellRange);
        painter.translate(pageRect.left() * scale, pageRect.top() * scale);

        // Scale according to the printer's resolution.
        painter.scale(scale, scale);

        d->printManager(sheet)->printPage(sheetPageNumber, painter);
    }
}

QList<KoShape*> PrintJob::shapesOnPage(int pageNumber)
{
    // This method is called only for page preparation; to determine the shapes to wait for.
    int sheetPageNumber = pageNumber;
    Sheet* sheet = d->getSheetPageNumber(&sheetPageNumber);
    if (!sheet)
        return QList<KoShape*>();

    const QRect cellRange = d->printManager(sheet)->cellRange(sheetPageNumber);
    return shapeManager()->shapesAt(sheet->cellCoordinatesToDocument(cellRange));
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
