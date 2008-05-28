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

#include "PrintJob.h"

#include "Canvas.h"
#include "Doc.h"
#include "Map.h"
#include "PrintManager.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "View.h"

#include <dialogs/SheetSelectPage.h>

#include <KoShapeManager.h>

#include <QPainter>

using namespace KSpread;

class PrintJob::Private
{
public:
    View* view;
    SheetSelectPage* sheetSelectPage;
    QStringList selectedSheets;

public:
    int setupPages();
};

int PrintJob::Private::setupPages()
{
    // Setup the pages.
    // TODO Stefan: Only perform layouting, if necessary, i.e. after page layout changes.
    int pageCount = 0;
    const QStringList sheets = sheetSelectPage->selectedSheets();
    if (sheets.isEmpty())
        pageCount = view->activeSheet()->printManager()->setupPages();
    else
    {
        for (int i = 0; i < sheets.count(); ++i)
        {
            Sheet* sheet = view->doc()->map()->findSheet(sheets[i]);
            if (sheet == 0)
            {
                kWarning(36005) << i18n("Sheet %1 could not be found for printing", sheets[i]);
                continue;
            }
            pageCount += sheet->printManager()->setupPages();
        }
    }
    return pageCount;
}

PrintJob::PrintJob(View *view)
    : KoPrintingDialog(view)
    , d(new Private)
{
    d->view = view;
    d->sheetSelectPage = new SheetSelectPage();

    setShapeManager(static_cast<Canvas*>(d->view->canvas())->shapeManager());

    PrintSettings* settings = d->view->activeSheet()->printSettings();

    //apply page layout parameters
    KoPageFormat::Format pageFormat = settings->pageLayout().format;

    printer().setPageSize( static_cast<QPrinter::PageSize>( KoPageFormat::printerPageSize( pageFormat ) ) );

    if (settings->pageLayout().orientation == KoPageFormat::Landscape || pageFormat == KoPageFormat::ScreenSize)
        printer().setOrientation( QPrinter::Landscape );
    else
        printer().setOrientation( QPrinter::Portrait );

    printer().setFullPage( true );

    //add possibility to select the sheets to print:
    //kDebug(36005) <<"Adding sheet selection page.";

    //kDebug(36005) <<"Iterating through available sheets and initializing list of available sheets.";
    QList<Sheet*> sheetList = d->view->doc()->map()->sheetList();
    for ( int i = sheetList.count()-1; i >= 0; --i )
    {
        Sheet* sheet = sheetList[ i ];
        //kDebug(36005) <<"Adding" << sheet->sheetName();
        d->sheetSelectPage->prependAvailableSheet(sheet->sheetName());
    }

    const int pageCount = d->setupPages();
    printer().setFromTo(1, pageCount);
}

PrintJob::~PrintJob()
{
//     delete d->sheetSelectPage; // QPrintDialog takes ownership
    delete d;
}

int PrintJob::documentFirstPage() const
{
    return 1;
}

int PrintJob::documentLastPage() const
{
    const QStringList sheets = d->selectedSheets;
    int pageCount = 0;
    if (sheets.isEmpty())
        pageCount = d->view->activeSheet()->printManager()->pageCount();
    else
    {
        for (int i = 0; i < sheets.count(); ++i)
        {
            Sheet* sheet = d->view->doc()->map()->findSheet(sheets[i]);
            if (sheet == 0)
            {
                kWarning(36005) << i18n("Sheet %1 could not be found for printing", sheets[i]);
                continue;
            }
            pageCount += sheet->printManager()->pageCount();
        }
    }
    return pageCount;
}

void PrintJob::startPrinting(RemovePolicy removePolicy)
{
    // Setup the pages.
    // TODO Stefan: Use the current page layout.
    const int pageCount = d->setupPages();
    printer().setFromTo(1, pageCount);
    // Store the selected sheets.
    // The printing is done in threads; one for each page.
    // At that time the dialog is already deleted.
    d->selectedSheets = d->sheetSelectPage->selectedSheets();
    // Start the printing.
    KoPrintingDialog::startPrinting(removePolicy);
}

void PrintJob::printPage(int pageNumber, QPainter &painter)
{
    kDebug(36004) << "Printing page" << pageNumber;
    // Find the sheet specific page number.
    Sheet* sheet = 0;
    int sheetPageNumber = pageNumber;
    const QStringList sheets = d->selectedSheets;
    if (sheets.isEmpty())
    {
        if (pageNumber > d->view->activeSheet()->printManager()->pageCount())
            return;
        // sheetPageNumber = pageNumber;
        sheet = d->view->activeSheet();
    }
    else
    {
        for (int i = 0; i < sheets.count(); ++i)
        {
            sheet = d->view->doc()->map()->findSheet(sheets[i]);
            if (sheet == 0)
            {
                kWarning(36005) << i18n("Sheet %1 could not be found for printing", sheets[i]);
                continue;
            }

            if (pageNumber <= sheet->printManager()->pageCount())
                break;
            sheetPageNumber -= sheet->printManager()->pageCount();
        }
    }

    // Print the page.
    if (sheet)
        sheet->printManager()->printPage(sheetPageNumber, painter);

#if 0
    Q_UNUSED(pageNumber);
    // kDebug(36005) <<"Entering KSpread print.";
    //save the current active sheet for later, so we can restore it at the end
    Sheet* selectedsheet = d->view->activeSheet();

    //print all sheets in the order given by the print dialog (Sheet Selection)
    QStringList sheetlist = d->sheetSelectPage->selectedSheets();

    if (sheetlist.empty())
    {
      // kDebug(36005) <<"No sheet for printing selected, printing active sheet";
      sheetlist.append(d->view->activeSheet()->sheetName());
    }

    bool firstpage = true;

    QStringList::iterator sheetlistiterator;
    for (sheetlistiterator = sheetlist.begin(); sheetlistiterator != sheetlist.end(); ++sheetlistiterator)
    {
        // kDebug(36005) <<"  printing sheet" << *sheetlistiterator;
        Sheet* sheet = d->view->doc()->map()->findSheet(*sheetlistiterator);
        if (sheet == 0)
        {
          kWarning(36005) << i18n("Sheet %1 could not be found for printing",*sheetlistiterator);
          continue;
        }

        d->view->setActiveSheet(sheet,false);

        PrintSettings* settings = sheet->printSettings();

        if (firstpage)
          firstpage=false;
        else
        {
          // kDebug(36005) <<" inserting new page";
          printer().newPage();
        }

        if ( d->view->canvasWidget()->editor() )
        {
            d->view->canvasWidget()->deleteEditor( true ); // save changes
        }

        //store the current setting in a temporary variable
        KoPageFormat::Orientation _orient = settings->pageLayout().orientation;

        //use the current orientation from print dialog
        if ( printer().orientation() == QPrinter::Landscape )
        {
            settings->setPageOrientation( KoPageFormat::Landscape );
        }
        else
        {
            settings->setPageOrientation( KoPageFormat::Portrait );
        }

        kDebug() << "QPainter.device() =" << painter.device();

#if 0
        SheetPrint* print = sheet->print();
        bool result = print->print( painter, &printer() );
#else
        PrintManager printManager = (d->view->activeSheet());
        bool result = printManager.print(painter, &printer());
#endif

        //Restore original orientation
        settings->setPageOrientation( _orient );

        // Nothing to print
        if( !result )
        {
            // not required in Qt
            //if( !printer.previewOnly() )
            //{
                KMessageBox::information( 0,
                i18n("Nothing to print for sheet %1.",
                d->view->activeSheet()->sheetName()) );
                //@todo: make sure we really can comment this out,
                //       what to do with partially broken printouts?
//                 printer.abort();
            //}
        }
    }

    d->view->setActiveSheet(selectedsheet);
#endif
}

QList<KoShape*> PrintJob::shapesOnPage(int pageNumber)
{
    Q_UNUSED(pageNumber);
    return shapeManager()->shapes();
}

QList<QWidget*> PrintJob::createOptionWidgets() const
{
    return QList<QWidget*>() << d->sheetSelectPage;
}
