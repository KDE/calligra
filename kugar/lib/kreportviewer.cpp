/***************************************************************************
            kreportviewer.cpp  -  Kugar KDE report viewer widget
            -------------------
  begin     : Tue Jun 04 2002
  copyright : (C) 2002 Alexander Dymo
  email     : cloudtemple@mksat.net
***************************************************************************/

#include <qapplication.h>
#include <kmessagebox.h>
#include <kprinter.h>
#include <klocale.h>

#include "kreportviewer.h"


/** Prints the rendered report to the selected printer - displays KDE print dialog */
void KReportViewer::printReport()
{
    // Check for a report
    if ( report == 0 )
        return ;

    // Get the page count
    int cnt = report->pageCount();

    // Check if there is a report or any pages to print
    if ( cnt == 0 )
    {
        KMessageBox::error( this, i18n( "There are no pages in the\nreport to print." ) );
        return ;
    }

    // Set the printer dialog
    KPrinter printer;

    setupPrinter( printer );
    if ( printer.setup( this ) )
        printReport( printer );
}

void KReportViewer::setupPrinter( KPrinter &printer )
{
    int cnt = report->pageCount();

    printer.setPageSize( ( KPrinter::PageSize ) report->pageSize() );
    printer.setOrientation( ( KPrinter::Orientation ) report->pageOrientation() );
    printer.setMinMax( 1, cnt );
    printer.setFromTo( 1, cnt );
    printer.setFullPage( true );
}

void KReportViewer::printReport( KPrinter &printer )
{
    // Check for a report
    if ( report == 0 )
        return ;

    // Get the page count
    int cnt = report->pageCount();

    // Check if there is a report or any pages to print
    if ( cnt == 0 )
    {
        KMessageBox::error( this, i18n( "There are no pages in the\nreport to print." ) );
        return ;
    }


    QPicture* page;
    QPainter painter;
    bool printRev;

    // Save the viewer's page index
    int viewIdx = report->getCurrentIndex();

    // Check the order we are printing the pages
    if ( printer.pageOrder() == QPrinter::FirstPageFirst )
        printRev = false;
    else
        printRev = true;

    // Get the count of pages and copies to print
    int printFrom = printer.fromPage() - 1;
    int printTo = printer.toPage();
    int printCnt = ( printTo - printFrom );
    int printCopies = printer.numCopies();
    int totalSteps = printCnt * printCopies;
    int currentStep = 1;

    // Set copies to 1, KPrinter copies does not appear to work ...
    printer.setNumCopies( 1 );

    // Setup the progress dialog
    QProgressDialog progress( i18n( "Printing report..." ),
                              i18n( "Cancel" ),
                              totalSteps, this, "progress", true );
    progress.setMinimumDuration( M_PROGRESS_DELAY );
    QObject::connect( &progress, SIGNAL( cancelled() ), this, SLOT( slotCancelPrinting() ) );
    progress.setProgress( 0 );
    qApp->processEvents();

    // Start the printer
    painter.begin( &printer );

    // Print each copy
    for ( int j = 0; j < printCopies; j++ )
    {
        // Print each page in the collection
        for ( int i = printFrom ; i < printTo; i++, currentStep++ )
        {
            if ( !printer.aborted() )
            {
                progress.setProgress( currentStep );
                qApp->processEvents();

                if ( printRev )
                    report->setCurrentPage( ( printCnt == 1 ) ? i : ( printCnt - 1 ) - i );
                else
                    report->setCurrentPage( i );

                page = report->getCurrentPage();
                page->play( &painter );
                if ( i < printCnt - 1 )
                    printer.newPage();
            }
            else
            {
                j = printCopies;
                break;
            }
        }
        if ( j < printCopies - 1 )
            printer.newPage();
    }

    // Cleanup printing
    setCursor( arrowCursor );
    painter.end();
    report->setCurrentPage( viewIdx );
}

#include "kreportviewer.moc"
