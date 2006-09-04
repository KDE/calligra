// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the implementation of the Kugar KPart.


#include <kapplication.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <QFile>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <kglobal.h>
#include <kiconloader.h>
#include <QLayout>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "kugar_part.h"
#include "kugar_view.h"
#include "kugar_factory.h"


// The view ctor.

KugarView::KugarView( KugarPart *part, QWidget *parent)
        : KoView( part, parent)
{
    setInstance( KugarFactory::global() );

    ( new Q3VBoxLayout( this ) ) ->setAutoAdd( true );
    view = new Kugar::MReportViewer( part->reportEngine(), this );

    view -> setFocusPolicy( Qt::ClickFocus );
    view -> show();

    //  setWidget(view);

    //  m_extension = new KugarBrowserExtension(this);


    // Define the actions.

    KStdAction::prior( view, SLOT( slotPrevPage() ), actionCollection(), "kuPrevPage" );
    KStdAction::next( view, SLOT( slotNextPage() ), actionCollection(), "kuNextPage" );
    KStdAction::firstPage( view, SLOT( slotFirstPage() ), actionCollection(), "kuFirstPage" );
    KStdAction::lastPage( view, SLOT( slotLastPage() ), actionCollection(), "kuLastPage" );

    setXMLFile( "kugarpart.rc" );

}


// The view dtor.

KugarView::~KugarView()
{}

void KugarView::setupPrinter( KPrinter &printer )
{
    view->setupPrinter( printer );
}

void KugarView::print( KPrinter &printer )
{
    view->printReport( printer );
}




#if 0
// Open a data file.

bool KugarPart::openFile()
{
    bool ok = false;
    QFile f( m_file );

    if ( f.open( QIODevice::ReadOnly ) )
    {
        if ( view -> setReportData( &f ) )
        {
            if ( view -> renderReport() )
                ok = true;
        }
        else
            KMessageBox::sorry( this, i18n( "Invalid data file: %1" , m_file ) );

        f.close();
    }
    else
        KMessageBox::sorry( this, i18n( "Unable to open data file: %1" , m_file ) );

    return ok;
}


// Close the data file.

bool KugarPart::closeUrl()
{
    // Nothing to do.

    return true;
}


// Print the report.

void KugarPart::print()
{
    view -> printReport();
}

#endif


bool KugarView::renderReport()
{
    return ( view -> renderReport() );
}



#include "kugar_view.moc"
