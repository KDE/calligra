/* This file is part of the KDE project
   Copyright (C) 2005, 2007, 2011 Dag Andersen <danders@get2net.dk>

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

#include "kptcontext.h"
#include "kptview.h"

#include <KoXmlReader.h>

#include <QDomDocument>

#include <kdebug.h>

namespace KPlato
{

Context::Context()
    : currentEstimateType(0),
      currentSchedule(0),
      m_contextLoaded( false )
{
    ganttview.ganttviewsize = -1;
    ganttview.taskviewsize = -1;

    accountsview.accountsviewsize = -1;
    accountsview.periodviewsize = -1;


}

Context::~Context() {
}

const KoXmlElement &Context::context() const
{
    return m_context;
}

bool Context::setContent( const QString &str )
{
    KoXmlDocument doc;
    if ( doc.setContent( str ) ) {
        return load( doc );
    }
    return false;
}

bool Context::load( const KoXmlDocument &document ) {
    m_document = document; // create a copy, document is deleted under our feet

    // Check if this is the right app
    KoXmlElement elm = m_document.documentElement();
    QString value = elm.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        kError() << "No mime type specified!";
//        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value != "application/x-vnd.kde.plan" ) {
        if ( value == "application/x-vnd.kde.kplato" ) {
            // accept, since we forgot to change kplato to plan for so long...
        } else {
            kError() << "Unknown mime type " << value;
//        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.kplato, got %1", value ) );
            return false;
        }
    }
/*    QString m_syntaxVersion = elm.attribute( "version", "0.0" );
    if ( m_syntaxVersion > "0.0" ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document was created with a newer version of Plan (syntax version: %1)\n"
                               "Opening it in this version of Plan will lose some information.", m_syntaxVersion ),
                      i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            setErrorMessage( "USER_CANCELED" );
            return false;
        }
    }
*/
/*
#ifdef KOXML_USE_QDOM
    int numNodes = elm.childNodes().count();
#else
    int numNodes = elm.childNodesCount();
#endif
*/
    KoXmlNode n = elm.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement element = n.toElement();
        if ( element.tagName() == "context" ) {
            m_context = element;
            m_contextLoaded = true;
        }
    }
    return true;
}

QDomDocument Context::save( const View *view ) const {
    QDomDocument document( "plan.context" );

    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "context" );
    doc.setAttribute( "editor", "Plan" );
    doc.setAttribute( "mime", "application/x-vnd.kde.plan" );
    doc.setAttribute( "version", 0.0 );
    document.appendChild( doc );

    QDomElement e = doc.ownerDocument().createElement("context");
    doc.appendChild( e );
    view->saveContext( e );

    return document;
}

}  //KPlato namespace
