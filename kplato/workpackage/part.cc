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

#include "part.h"
#include "view.h"
#include "factory.h"

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

namespace KPlatoWork
{

Part::Part( QWidget *parentWidget, QObject *parent, bool singleViewMode )
        : KoDocument( parentWidget, parent, singleViewMode )
{
    m_commandHistory = new K3CommandHistory( actionCollection() );

    setComponentData( Factory::global() );
    setTemplateType( "kplatowork_template" );
//     m_config.setReadWrite( isReadWrite() || !isEmbedded() );
//     m_config.load();

    connect( m_commandHistory, SIGNAL( commandExecuted( K3Command * ) ), SLOT( slotCommandExecuted( K3Command * ) ) );
    connect( m_commandHistory, SIGNAL( documentRestored() ), SLOT( slotDocumentRestored() ) );

}

Part::~Part()
{
//    m_config.save();
    delete m_commandHistory; // before project, in case of dependencies...
}

KoView *Part::createViewInstance( QWidget *parent )
{
    View *view = new View( this, parent );
    connect( view, SIGNAL( destroyed() ), this, SLOT( slotViewDestroyed() ) );
    return view;
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
    emit sigProgress( 100 ); // the rest is only processing, not loading

    kDebug() <<"Loading took" << ( float ) ( dt.elapsed() ) / 1000 <<" seconds";

    // do some sanity checking on document.
    emit sigProgress( -1 );

    m_commandHistory->clear();
    m_commandHistory->documentSaved();
    setModified( false );
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
    doc.setAttribute( "editor", "KPlatoWP" );
    doc.setAttribute( "mime", "application/x-vnd.kde.kplato.workpackage" );
    doc.setAttribute( "version", CURRENT_SYNTAX_VERSION );
    document.appendChild( doc );

    // Save the project
    
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
}

void Part::slotViewDestroyed()
{
}

}  //KPlato namespace

#include "part.moc"
