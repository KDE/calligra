/* This file is part of the KDE project
 Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
 Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>
 Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "kptpart.h"
#include "kptview.h"
#include "kptfactory.h"
#include "kptproject.h"
#include "kptmainprojectdialog.h"
#include "kptresource.h"
#include "kptcontext.h"
#include "kptganttview.h"
//#include "KDGanttViewTaskLink.h"

#include <KoZoomHandler.h>
#include <KoStore.h>
#include <KoXmlReader.h>
#include <KoStore.h>
#include <KoStoreDevice.h>

#include <qpainter.h>
#include <qfileinfo.h>
#include <QTimer>

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <k3command.h>
#include <k3command.h>
#include <kparts/partmanager.h>
#include <kmimetype.h>

#include <KoGlobal.h>

#define CURRENT_SYNTAX_VERSION "0.6"

namespace KPlato
{

Part::Part( QWidget *parentWidget, QObject *parent, bool singleViewMode )
        : KoDocument( parentWidget, parent, singleViewMode ),
        m_project( 0 ), m_projectDialog( 0 ), m_parentWidget( parentWidget ), m_view( 0 ),
        m_context( 0 ), m_xmlLoader()
{
    m_update = m_calculate = false;
    m_commandHistory = new K3CommandHistory( actionCollection() );

    setComponentData( Factory::global() );
    setTemplateType( "kplato_template" );
    m_config.setReadWrite( isReadWrite() || !isEmbedded() );
    m_config.load();

    delete m_project;
    m_project = new Project(); // after config is loaded

    connect( m_commandHistory, SIGNAL( commandExecuted( K3Command * ) ), SLOT( slotCommandExecuted( K3Command * ) ) );
    connect( m_commandHistory, SIGNAL( documentRestored() ), SLOT( slotDocumentRestored() ) );

}


Part::~Part()
{
    m_config.save();
    delete m_commandHistory; // before project, in case of dependencies...
    delete m_project;
    delete m_projectDialog;
}


KoView *Part::createViewInstance( QWidget *parent )
{
    m_view = new View( this, parent );
    connect( m_view, SIGNAL( destroyed() ), this, SLOT( slotViewDestroyed() ) );

    // If there is a project dialog this should be deleted so it will
    // use the m_view as parent. If the dialog will be needed again,
    // it will be made at that point
    if ( m_projectDialog != 0 ) {
        kDebug() <<"Deleting m_projectDialog because of new ViewInstance";
        delete m_projectDialog;
        m_projectDialog = 0;
    }
/*    if ( m_context ) {
        m_view->loadContext( m_context->context() );
    }*/
    //m_view->setBaselineMode(getProject().isBaselined()); FIXME: Removed for this release
    return m_view;
}


void Part::editProject()
{

    QWidget * parent = m_parentWidget;
    if ( m_view )
        parent = m_view;

    if ( m_projectDialog == 0 )
        // Make the dialog
        m_projectDialog = new MainProjectDialog( *m_project, parent );

    m_projectDialog->exec();
}

bool Part::loadOasis( const KoXmlDocument &doc, KoOasisStyles &, const KoXmlDocument&, KoStore * )
{
    kDebug();
    return loadXML( 0, doc ); // We have only one format, so try to load that!
}

bool Part::loadXML( QIODevice *, const KoXmlDocument &document )
{
    kDebug();
    QTime dt;
    dt.start();
    emit sigProgress( 0 );

    QString value;
    KoXmlElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        kError() << "No mime type specified!" << endl;
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value != "application/x-vnd.kde.kplato" ) {
        kError() << "Unknown mime type " << value << endl;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.kplato, got %1", value ) );
        return false;
    }
    QString m_syntaxVersion = plan.attribute( "version", CURRENT_SYNTAX_VERSION );
    m_xmlLoader.setVersion( m_syntaxVersion );
    if ( m_syntaxVersion > CURRENT_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document was created with a newer version of KPlato (syntax version: %1)\n"
                               "Opening it in this version of KPlato will lose some information.", m_syntaxVersion ),
                      i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            setErrorMessage( "USER_CANCELED" );
            return false;
        }
    }
    emit sigProgress( 5 );

#ifdef KOXML_USE_QDOM
    int numNodes = plan.childNodes().count();
#else
    int numNodes = plan.childNodesCount();
#endif
    if ( numNodes > 2 ) {
        //TODO: Make a proper bitching about this
        kDebug() <<"*** Error ***";
        kDebug() <<"  Children count should be maximum 2, but is" << numNodes;
        return false;
    }
    m_xmlLoader.startLoad();
    KoXmlNode n = plan.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "project" ) {
            Project * newProject = new Project();
            m_xmlLoader.setProject( newProject );
            if ( newProject->load( e, m_xmlLoader ) ) {
                // The load went fine. Throw out the old project
                delete m_project;
                m_project = newProject;
                delete m_projectDialog;
                m_projectDialog = 0;
            } else {
                delete newProject;
                m_xmlLoader.addMsg( XMLLoaderObject::Errors, "Loading of project failed" );
                //TODO add some ui here
            }
        } else if ( e.tagName() == "objects" ) {
            kDebug()<<"loadObjects";
            loadObjects( e );
        }
    }
    m_xmlLoader.stopLoad();
    emit sigProgress( 100 ); // the rest is only processing, not loading

    kDebug() <<"Loading took" << ( float ) ( dt.elapsed() ) / 1000 <<" seconds";

    // do some sanity checking on document.
    emit sigProgress( -1 );

    m_commandHistory->clear();
    m_commandHistory->documentSaved();
    setModified( false );
    if ( m_view )
        m_view->slotUpdate();
    return true;
}

QDomDocument Part::saveXML()
{
    kDebug();
    QDomDocument document( "kplato" );

    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "kplato" );
    doc.setAttribute( "editor", "KPlato" );
    doc.setAttribute( "mime", "application/x-vnd.kde.kplato" );
    doc.setAttribute( "version", CURRENT_SYNTAX_VERSION );
    document.appendChild( doc );

    // Save the project
    m_project->save( doc );
    
    if ( ! children().isEmpty() ) {
        QDomElement el = document.createElement( "objects" );
        foreach ( KoDocumentChild *ch, children() ) {
            if ( ch->isDeleted() ) {
                continue;
            }
            QDomElement e = ch->save( document, false );
            el.appendChild( e );
        }
        if ( el.childNodes().count() > 0 ) {
            doc.appendChild( el );
        }
    }
    
    m_commandHistory->documentSaved();
    return document;
}


void Part::slotDocumentRestored()
{
    //kDebug();
    setModified( false );
}


void Part::paintContent( QPainter &, const QRect &)
{
    // Don't embed this app!!!
}


void Part::addCommand( K3Command * cmd, bool execute )
{
    m_commandHistory->addCommand( cmd, execute );
}

void Part::slotCommandExecuted( K3Command * )
{
    //kDebug();
    setModified( true );
    kDebug() <<"------- KPlato, is embedded:" << isEmbedded();
    if ( m_view == NULL )
        return ;

    if ( m_update )
        m_view->slotUpdate();

    m_update = m_calculate = false;
}

void Part::slotViewDestroyed()
{
    m_view = NULL;
}

void Part::setCommandType( int type )
{
    //kDebug()<<"type="<<type;
    if ( type == 0 )
        m_update = true;
    else if ( type == 1 )
        m_calculate = true;
}

void Part::generateWBS()
{
    m_project->generateWBS( 1, m_wbsDefinition );
}

void Part::activate( QWidget *w )
{
    if ( manager() )
        manager()->setActivePart( this, w );
}

DocumentChild *Part::createChild( KoDocument *doc, const QRect& geometry )
{
    DocumentChild *ch = new DocumentChild( this, doc, geometry );
    insertChild( ch );
    return ch;
}

void Part::loadObjects( const KoXmlElement &element )
{
    kDebug();
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "object" ) {
            DocumentChild *ch = new DocumentChild( this );
            if ( ch->load( e ) ) {
                kDebug()<<"loaded";
                insertChild( ch );
            } else {
                kDebug()<<"Failed to load object";
                delete ch;
            }
        }
    }
}

bool Part::loadChildren( KoStore* store )
{
    kDebug();
    foreach ( KoDocumentChild *ch, children() ) {
        ch->loadDocument( store );
    }
    return true;
}

bool Part::completeLoading( KoStore *store )
{
    if ( store == 0 ) {
        // can happen if loading a template
        kDebug()<<"No store"<<endl;
        return true; // continue anyway
    }
    delete m_context;
    m_context = new Context();
    KoXmlDocument doc;
    if ( loadAndParse( store, "context.xml", doc ) ) {
        store->close();
        m_context->load( doc );
    } else kWarning()<<"No context"<<endl;
    return true;
}

bool Part::completeSaving( KoStore *store )
{
    delete m_context;
    m_context = 0;
    if ( m_view ) {
        m_context = new Context();
        m_context->save( m_view );
        if ( store->open( "context.xml" ) )
        {
            QDomDocument doc = m_context->save( m_view );
            KoStoreDevice dev( store );

            QByteArray s = doc.toByteArray(); // this is already Utf8!
            (void)dev.write( s.data(), s.size() );
            (void)store->close();
        }
    }
    return true;
}

bool Part::loadAndParse(KoStore* store, const QString& filename, KoXmlDocument& doc)
{
    //kDebug(30003) << "oldLoadAndParse: Trying to open " << filename << endl;

    if (!store->open(filename))
    {
        kWarning() << "Entry " << filename << " not found!" << endl;
//        d->lastErrorMessage = i18n( "Could not find %1",filename );
        return false;
    }
    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    bool ok = doc.setContent( store->device(), &errorMsg, &errorLine, &errorColumn );
    if ( !ok )
    {
        kError() << "Parsing error in " << filename << "! Aborting!" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg << endl;
/*        d->lastErrorMessage = i18n( "Parsing error in %1 at line %2, column %3\nError message: %4"
                              ,filename  ,errorLine, errorColumn ,
                              QCoreApplication::translate("QXml", errorMsg.toUtf8(), 0,
                                  QCoreApplication::UnicodeUTF8));*/
        store->close();
        return false;
    }
    kDebug() << "File " << filename << " loaded and parsed" << endl;
    return true;
}

//--------------------------------

DocumentChild::DocumentChild ( KoDocument* parent )
    : KoDocumentChild ( parent )
{
}

DocumentChild::DocumentChild ( KoDocument* parent, KoDocument* doc, const QRect& geometry )
    : KoDocumentChild ( parent, doc, geometry )
{
}

void DocumentChild::setActivated( bool act, QWidget *w )
{
    if ( document()->manager() ) {
        if ( act ) {
            document()->manager()->setActivePart( document(), w );
        } else if ( parentDocument()->manager() ) {
            parentDocument()->manager()->setActivePart( parentDocument(), w );
        } else {
            document()->manager()->setActivePart( 0, w );
        }
    }
}

KoDocument* DocumentChild::hitTest( const QPoint& , KoView* view, const QMatrix& )
{
    if ( document()->views().contains( view ) ) {
        return document();
    }
    return 0;
}


QDomElement DocumentChild::save( QDomDocument &doc, bool uppercase )
{
    if ( document() == 0 ) {
        return QDomElement();
    }
    QDomElement e = KoDocumentChild::save( doc, uppercase );
    kDebug()<<m_title;
    e.setAttribute( "title", m_title );
    e.setAttribute( "icon", m_icon );
    return e;
}

bool DocumentChild::load( const KoXmlElement& element, bool uppercase )
{
    if ( KoDocumentChild::load( element, uppercase ) ) {
        m_icon = element.attribute( "icon", QString() );
        m_title = element.attribute( "title", QString() );
        kDebug()<<m_title;
        return true;
    }
    return false;
}


}  //KPlato namespace

#include "kptpart.moc"
