/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptdocuments.h"
#include "KoXmlReader.h"
#include "kptxmlloaderobject.h"

#include "qdom.h"

#include <kdebug.h>

namespace KPlato
{
    
Document::Document()
    : m_type( Type_None ),
    m_url( KUrl() )
{
    kDebug()<<this;
}

bool Document::operator==( const Document &doc ) const
{
    bool res = ( m_url == doc.url() && m_type == doc.type() && m_status == doc.status() );
    return res;
}

Document::Document( const KUrl &url, Document::Type type )
    : m_type( type ),
    m_url( url )
{
    kDebug()<<this;
}

Document::~Document()
{
    kDebug()<<this;
}

bool Document::isValid() const
{
    return m_url.url().isEmpty();
}

QString Document::typeToString( bool trans ) const
{
    switch ( m_type ) {
        case Type_Product: return trans ? i18n( "Product" ) : "Product";
        case Type_Reference: return trans ? i18n( "Reference" ) : "Reference";
        default: break;
    }
    return trans ? i18n( "Unknown" ) : "Unknown";
}

bool Document::load( KoXmlElement &element, XMLLoaderObject &status )
{
    m_url = KUrl( element.attribute( "url" ) );
    m_type = ( Type )( element.attribute( "type" ).toInt() );
    m_status = element.attribute( "status" );
    return true;
}

void Document::save(QDomElement &element) const
{
    element.setAttribute("url", m_url.url() );
    element.setAttribute("type", m_type );
    element.setAttribute("status", m_status );
}

//----------------
Documents::Documents()
{
    kDebug()<<this;
}

Documents::Documents( const Documents &docs )
{
    kDebug()<<this;
    foreach ( Document *doc, docs.documents() ) {
        m_docs.append( new Document( *doc ) );
    }
}

Documents::~Documents()
{
    kDebug()<<this;
    deleteAll();
}

bool Documents::operator==( const Documents &docs ) const
{
    int cnt = m_docs.count();
    if ( cnt != docs.count() ) {
        return false;
    }
    for ( int i = 0; i < cnt; ++i ) {
        if ( *(m_docs.at( i ) ) != *( docs.at( i ) ) ) {
            return false;
        }
    }
    return true;
}

void Documents::deleteAll()
{
    while ( ! m_docs.isEmpty() ) {
        delete m_docs.takeFirst();
    }
}

void Documents::addDocument( Document *doc )
{
    Q_ASSERT( doc );
    m_docs.append( doc );
}

void Documents::addDocument( const KUrl &url, Document::Type type )
{
    m_docs.append( new Document( url, type ) );
}

Document *Documents::takeDocument( int index )
{
    if ( index >= 0 && index < m_docs.count() ) {
        return m_docs.takeAt( index );
    }
    return 0;
}

Document *Documents::takeDocument( Document *doc )
{
    Q_ASSERT( m_docs.contains( doc ) );
    return takeDocument( m_docs.indexOf( doc ) );
}

Document *Documents::findDocument( const Document *doc ) const
{
    return findDocument( doc->url() );
}

Document *Documents::findDocument( const KUrl &url ) const
{
    for ( int i = 0; i < m_docs.count(); ++i ) {
        if ( m_docs.at( i )->url() == url ) {
            return m_docs.at( i );
        }
    }
    return 0;
}

bool Documents::load( KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug();
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "document") {
            Document *doc = new Document();
            if ( !doc->load( e, status ) ) {
                kWarning()<<"Failed to load document";
                status.addMsg( XMLLoaderObject::Errors, "Failed to load document" );
                delete doc;
            } else {
                addDocument( doc );
                status.addMsg( i18n( "Document loaded, url=%1").arg( doc->url().url() ) );
            }
        }
    }
}

void Documents::save(QDomElement &element) const
{
    if ( m_docs.isEmpty() ) {
        return;
    }
    QDomElement e = element.ownerDocument().createElement("documents");
    element.appendChild(e);
    foreach ( Document *d, m_docs) {
        QDomElement me = element.ownerDocument().createElement("document");
        e.appendChild(me);
        d->save( me );
    }
}

} //namespace KPlato
