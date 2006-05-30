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
#include <Q3PtrList>
#include <kglobal.h>
#include <kiconloader.h>
#include <QWidget>
#include <kfiledialog.h>
#include <kdebug.h>
#include <KoStore.h>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "kugar_part.h"
#include "kugar_view.h"
#include "kugar_factory.h"

#include <KoFilterManager.h>

KugarPart::KugarPart( QWidget *parentWidget,
                      QObject* parent, bool singleViewMode )
    : KoDocument( parentWidget, parent, singleViewMode ),
    m_templateOk( false )
{
    setInstance( KugarFactory::global(), false );
    m_reportEngine = new Kugar::MReportEngine();
    connect( m_reportEngine, SIGNAL( preferedTemplate( const QString & ) ),
             SLOT( slotPreferredTemplate( const QString & ) ) );
}

KugarPart::~KugarPart()
{
    m_reportEngine->removeRef();
    //  closeURL();
}

bool KugarPart::loadOasis( const QDomDocument&, KoOasisStyles&,
                           const QDomDocument&, KoStore* )
{
    return false;
}

bool KugarPart::saveOasis( KoStore*, KoXmlWriter* )
{
    return false;
}

bool KugarPart::loadXML( QIODevice *file, const QDomDocument & /*doc*/ )
{
    m_docURL = url();
    bool ok = true;
    if ( file )
    {
        file->reset();
        m_reportData=QString(file->readAll());

        if ( m_reportData.length() != 0 )
        {
            ok = m_reportEngine->setReportData( m_reportData );

            if ( m_templateOk )
            {
                m_reportEngine->renderReport();
                if ( ok )
                {
                    foreach ( KoView* view, views() )
                    {
                        ok = static_cast<KugarView*>( qobject_cast<KugarView*>(view))->renderReport();
                        if ( !ok )
                            break;
                    }
                }
            }
            if ( !ok )
                KMessageBox::sorry( 0, i18n( "Invalid data file %1" , m_file ) );
        }
        else
        {
            ok = false;
            KMessageBox::sorry( 0, i18n( "The zero sized data file %1 can't be rendered" , m_file ) );
        }

    }
    else
    {
        ok = false;
        KMessageBox::sorry( 0, i18n( "Unable to open data file: %1" , m_file ) );
    }

    return ok;
}

KoView* KugarPart::createViewInstance( QWidget* parent, const char* name )
{
    KugarView * v = new KugarView( this, parent, name );
    if ( m_templateOk )
        v->renderReport();
    return v;
}


void KugarPart::slotPreferredTemplate( const QString &tpl )
{
    KUrl url( tpl );
    QString localtpl;
    bool isTemp = false;

    if ( !url.isValid() )
    {
/*        kDebug() << "mailformed url" << endl;*/
        if ( tpl.find( '/' ) >= 0 )
        {
            if ( tpl.startsWith( "." ) )
            {
                KUrl tmpURL( m_docURL );
                tmpURL.setFileName( "" );
                tmpURL.addPath( tpl );
                if ( KIO::NetAccess::download( tmpURL, localtpl,0L) )
                    isTemp = true;
                else
                    KMessageBox::sorry( 0, i18n( "Unable to download template file: %1" , url.prettyUrl() ) );
            }
            else
                localtpl = tpl;
        }
        else
        {
            QString former_localtpl = localtpl;
            localtpl = kapp -> dirs() -> findResource( "data", "kugar/templates/" + tpl );
            if ( localtpl.isEmpty() )
            {
                KUrl tmpURL( m_docURL );
                tmpURL.setFileName( "" );
                tmpURL.addPath( tpl );
                if ( KIO::NetAccess::download( tmpURL, localtpl,0L ) )
                    isTemp = true;
                else
                    KMessageBox::sorry( 0, i18n( "Unable to download template file: %1" , url.prettyUrl() ) );
            }
        }
    }
    else
    {
        if ( KIO::NetAccess::download( url, localtpl,0L ) )
            isTemp = true;
        else
            KMessageBox::sorry( 0, i18n( "Unable to download template file: %1" ,url.prettyUrl() ) );
    }

/*    kDebug() << "localtpl: " << localtpl.latin1() << endl;*/
    if ( !localtpl.isEmpty() )
    {
        QFile f( localtpl );

        if ( f.open( QIODevice::ReadOnly ) )
        {
/*            kDebug() << "localtpl opened" << endl;*/
            // Try to find out whether it is a mime multi part file
            char buf[ 5 ];
            if ( f.read( buf, 4 ) == 4 )
            {
                bool isRawXML = ( strncasecmp( buf, "<?xm", 4 ) == 0 );
                f.close();

                if ( isRawXML )
                {
/*                    kDebug() << "RawXML" << endl;*/
                    f.open( QIODevice::ReadOnly );
                    if ( !m_reportEngine -> setReportTemplate( &f ) )
                        KMessageBox::sorry( 0, i18n( "Invalid template file: %1" , localtpl ) );
                    else
                    {
                        m_templateOk = true;
/*                        kDebug() << "Setting m_templateOk" << endl;*/
                    }
                    f.close();
                }
                else
                {
                    KoStore *tmpStore = KoStore::createStore( localtpl, KoStore::Read );
                    if ( tmpStore->open( "maindoc.xml" ) )
                    {
                        if ( !m_reportEngine -> setReportTemplate( tmpStore->device() ) )
                            KMessageBox::sorry( 0, i18n( "%1 is not a valid Kugar Designer template file." , localtpl ) );
                        else
                            m_templateOk = true;
                        tmpStore->close();
                    }
                    else
                        KMessageBox::sorry( 0, i18n( "%1 is not a valid Kugar Designer template file." , localtpl ) );

                    delete tmpStore;
                }

            }
            else
            {
                f.close();
                KMessageBox::sorry( 0, i18n( "Couldn't read the beginning of the template file: %1" , localtpl ) );
            }

        }
        else
            KMessageBox::sorry( 0, i18n( "Unable to open template file: %1" , localtpl ) );

        if ( isTemp )
            KIO::NetAccess::removeTempFile( localtpl );
    }
}

#include "kugar_part.moc"
