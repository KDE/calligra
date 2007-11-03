/* This file is part of the KDE project
 Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
 Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>
 Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
 Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#include "part.h"
#include "view.h"
#include "factory.h"

#include "kptnode.h"
#include "kptproject.h"

#include <KoZoomHandler.h>
#include <KoStore.h>
#include <KoXmlReader.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoOdfReadStore.h>

#include <qpainter.h>
#include <qfileinfo.h>
#include <QTimer>

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kparts/partmanager.h>
#include <kmimetype.h>

#include <KoGlobal.h>

#define CURRENT_SYNTAX_VERSION "0.1"

using namespace KPlato;

namespace KPlatoWork
{

Part::Part( QWidget *parentWidget, QObject *parent, bool singleViewMode )
        : KoDocument( parentWidget, parent, singleViewMode ),
        m_project( 0 ), m_xmlLoader()
{
    setComponentData( Factory::global() );
    setTemplateType( "kplatowork_template" );
//     m_config.setReadWrite( isReadWrite() || !isEmbedded() );
//     m_config.load();

    setProject( new Project() ); // after config is loaded
}

Part::~Part()
{
//    m_config.save();
    delete m_project;
}

void Part::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( changed() ), this, SIGNAL( changed() ) );
        delete m_project;
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( changed() ), this, SIGNAL( changed() ) );
    }
    emit changed();
}


KoView *Part::createViewInstance( QWidget *parent )
{
    View *view = new View( this, parent );
    connect( view, SIGNAL( destroyed() ), this, SLOT( slotViewDestroyed() ) );
    return view;
}

bool Part::loadOdf( KoOdfReadStore & odfStore )
{
    kDebug();
    return loadXML( 0, odfStore.contentDoc() ); // We have only one format, so try to load that!
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
    } else if ( value != "application/x-vnd.kde.kplato.work" ) {
        kError() << "Unknown mime type " << value << endl;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.kplato.work, got %1", value ) );
        return false;
    }
    QString m_syntaxVersion = plan.attribute( "version", CURRENT_SYNTAX_VERSION );
    if ( m_syntaxVersion > CURRENT_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document was created with a newer version of KPlatoWork (syntax version: %1)\n"
                               "Opening it in this version of KPlatoWork will lose some information.", m_syntaxVersion ),
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
    emit sigProgress( 100 ); // the rest is only processing, not loading

    kDebug() <<"Loading took" << ( float ) ( dt.elapsed() ) / 1000 <<" seconds";

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
                setProject( newProject );
            } else {
                delete newProject;
                m_xmlLoader.addMsg( XMLLoaderObject::Errors, "Loading of work package failed" );
                //TODO add some ui here
            }
        } else if ( e.tagName() == "objects" ) {
            kDebug()<<"loadObjects";
            //loadObjects( e );
        }
    }
    m_xmlLoader.stopLoad();
    emit sigProgress( 100 ); // the rest is only processing, not loading

    kDebug() <<"Loading took" << ( float ) ( dt.elapsed() ) / 1000 <<" seconds";

    // do some sanity checking on document.
    emit sigProgress( -1 );

    setModified( false );
    emit changed();
    return true;
}

QDomDocument Part::saveXML()
{
    kDebug();
    QDomDocument document( "kplato-workpackage" );

    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "kplato-workpackage" );
    doc.setAttribute( "editor", "KPlatoWork" );
    doc.setAttribute( "mime", "application/x-vnd.kde.kplato.workpackage" );
    doc.setAttribute( "version", CURRENT_SYNTAX_VERSION );
    document.appendChild( doc );

    // Save the project
    
    return document;
}

void Part::paintContent( QPainter &, const QRect &)
{
    // Don't embed this app!!!
}

void Part::slotViewDestroyed()
{
}

}  //KPlatoWork namespace

#include "part.moc"
