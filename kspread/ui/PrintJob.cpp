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
#include "View.h"
#include "Map.h"
#include "Canvas.h"
#include "../Sheet.h"
#include "../SheetPrint.h"
#include "../Doc.h"
#include <dialogs/SheetSelectPage.h>

#include <KoShapeManager.h>

#include <QPainter>

namespace KSpread
{

PrintJob::PrintJob(View *view)
    : KoPrintingDialog(view),
    m_view(view),
    m_sheetSelectPage(new SheetSelectPage())
{
    setShapeManager(static_cast<Canvas*>(m_view->canvas())->shapeManager());
    printer().setFromTo(1, 1);

    SheetPrint* print = m_view->activeSheet()->print();

    //apply page layout parameters
    KoPageFormat::Format pageFormat = print->paperFormat();

    printer().setPageSize( static_cast<QPrinter::PageSize>( KoPageFormat::printerPageSize( pageFormat ) ) );

    if ( print->orientation() == KoPageFormat::Landscape || pageFormat == KoPageFormat::ScreenSize )
        printer().setOrientation( QPrinter::Landscape );
    else
        printer().setOrientation( QPrinter::Portrait );

    printer().setFullPage( true );

    //add possibility to select the sheets to print:
    //kDebug(36005) <<"Adding sheet selection page.";

    //kDebug(36005) <<"Iterating through available sheets and initializing list of available sheets.";
    QList<Sheet*> sheetList = m_view->doc()->map()->sheetList();
    for ( int i = sheetList.count()-1; i >= 0; --i )
    {
        Sheet* sheet = sheetList[ i ];
        //kDebug(36005) <<"Adding" << sheet->sheetName();
        m_sheetSelectPage->prependAvailableSheet(sheet->sheetName());
    }
}

void PrintJob::printPage(int pageNumber, QPainter &painter)
{
    Q_UNUSED(pageNumber);
    // kDebug(36005) <<"Entering KSpread print.";
    //save the current active sheet for later, so we can restore it at the end
    Sheet* selectedsheet = m_view->activeSheet();

    //print all sheets in the order given by the print dialog (Sheet Selection)
    QStringList sheetlist = m_sheetSelectPage->selectedSheets();

    if (sheetlist.empty())
    {
      // kDebug(36005) <<"No sheet for printing selected, printing active sheet";
      sheetlist.append(m_view->activeSheet()->sheetName());
    }

    bool firstpage = true;

    QStringList::iterator sheetlistiterator;
    for (sheetlistiterator = sheetlist.begin(); sheetlistiterator != sheetlist.end(); ++sheetlistiterator)
    {
        // kDebug(36005) <<"  printing sheet" << *sheetlistiterator;
        Sheet* sheet = m_view->doc()->map()->findSheet(*sheetlistiterator);
        if (sheet == 0)
        {
          kWarning(36005) << i18n("Sheet %1 could not be found for printing",*sheetlistiterator);
          continue;
        }

        m_view->setActiveSheet(sheet,false);

        SheetPrint* print = m_view->activeSheet()->print();

        if (firstpage)
          firstpage=false;
        else
        {
          // kDebug(36005) <<" inserting new page";
          printer().newPage();
        }

        if ( m_view->canvasWidget()->editor() )
        {
            m_view->canvasWidget()->deleteEditor( true ); // save changes
        }

        //store the current setting in a temporary variable
        KoPageFormat::Orientation _orient = print->orientation();

        //use the current orientation from print dialog
        if ( printer().orientation() == QPrinter::Landscape )
        {
            print->setPaperOrientation( KoPageFormat::Landscape );
        }
        else
        {
            print->setPaperOrientation( KoPageFormat::Portrait );
        }

        bool result = print->print( painter, &printer() );

        //Restore original orientation
        print->setPaperOrientation( _orient );

        // Nothing to print
        if( !result )
        {
            // not required in Qt
            //if( !printer.previewOnly() )
            //{
                KMessageBox::information( 0,
                i18n("Nothing to print for sheet %1.",
                m_view->activeSheet()->sheetName()) );
                //@todo: make sure we really can comment this out,
                //       what to do with partially broken printouts?
//                 printer.abort();
            //}
        }
    }

    m_view->setActiveSheet(selectedsheet);
}

QList<KoShape*> PrintJob::shapesOnPage(int pageNumber)
{
    Q_UNUSED(pageNumber);
    return shapeManager()->shapes();
}

QList<QWidget*> PrintJob::createOptionWidgets() const
{
    return QList<QWidget*>() << m_sheetSelectPage;
}

} // namespace KSpread
