/* This file is part of the KDE project
   Copyright (C) 2007, 2012 Dag Andersen <danders@get2net.dk>

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
#include "kptnode.h"
#include "kptxmlloaderobject.h"
#include <kptdebug.h>

#include "KoXmlReader.h"
#include <KoStore.h>

#include "QDomDocument"


namespace KPlato
{
    
Document::Document()
    : m_type( Type_None ),
    m_url( KUrl() ),
    m_sendAs( SendAs_None ),
    parent ( 0 )
{
    //kDebug(planDbg())<<this;
}

Document::Document( const KUrl &url, Document::Type type, Document::SendAs sendAs )
    : m_type( type ),
    m_sendAs( sendAs ),
    parent ( 0 )
{
    setUrl( url );
    //kDebug(planDbg())<<this;
}

Document::~Document()
{
    //kDebug(planDbg())<<this;
}

bool Document::operator==( const Document &doc ) const
{
    bool res = ( m_url == doc.url() &&
                 m_name == doc.m_name &&
                 m_type == doc.type() && 
                 m_status == doc.status() &&
                 m_sendAs == doc.sendAs() 
               );
    return res;
}

bool Document::isValid() const
{
    return m_url.isValid();
}

QStringList Document::typeList( bool trans )
{
    return QStringList()
            << ( trans ? i18nc( "@item", "Unknown" ) : "Unknown" )
            << ( trans ? i18nc( "@item The produced document", "Product" ) : "Product" )
            << ( trans ? i18nc( "@item Document is used for refrence", "Reference" ) : "Reference" );
}

QString Document::typeToString( Document::Type type, bool trans )
{
    return typeList( trans ).at( type );
}

QStringList Document::sendAsList( bool trans )
{
    return QStringList()
            << ( trans ? i18nc( "@item", "Unknown" ) : "Unknown" )
            << ( trans ? i18nc( "@item Send a copy of the document", "Copy" ) : "Copy" )
            << ( trans ? i18nc( "@item Send the reference (url) of the document", "Reference" ) : "Reference" );
}

QString Document::sendAsToString( Document::SendAs snd, bool trans )
{
    return sendAsList( trans ).at( snd );
}

void Document::setName( const QString &name )
{
    if ( m_name != name ) {
        m_name = name;
        if ( parent ) {
            parent->documentChanged( this );
        }
    }
}

void Document::setType( Type type )
{
    if ( type != m_type ) {
        m_type = type;
        if ( parent ) {
            parent->documentChanged( this );
        }
    }
}

void Document::setSendAs( SendAs snd )
{
    if ( m_sendAs != snd ) {
        m_sendAs = snd;
        if ( parent ) {
            parent->documentChanged( this );
        }
    }
}

void Document::setUrl( const KUrl &url )
{
    if ( m_url != url ) {
        m_url = url;
        if ( m_name.isEmpty() ) {
            m_name = url.fileName();
        }
        if ( parent ) {
            parent->documentChanged( this );
        }
    }
}

void Document::setStatus( const QString &sts )
{
    if ( m_status != sts ) {
        m_status = sts;
        if ( parent ) {
            parent->documentChanged( this );
        }
    }
}

bool Document::load( KoXmlElement &element, XMLLoaderObject &status )
{
    Q_UNUSED(status);
    m_url = KUrl( element.attribute( "url" ) );
    m_name = element.attribute( "name", m_url.fileName() );
    m_type = ( Type )( element.attribute( "type" ).toInt() );
    m_status = element.attribute( "status" );
    m_sendAs = ( SendAs )( element.attribute( "sendas" ).toInt() );
    return true;
}

void Document::save(QDomElement &element) const
{
    element.setAttribute("url", m_url.url() );
    element.setAttribute("name", m_name );
    element.setAttribute("type", m_type );
    element.setAttribute("status", m_status );
    element.setAttribute("sendas", m_sendAs );
}

//----------------
Documents::Documents()
    : node( 0 )
{
    //kDebug(planDbg())<<this;
}

Documents::Documents( const Documents &docs )
    : node( 0 )
{
    //kDebug(planDbg())<<this;
    foreach ( Document *doc, docs.documents() ) {
        m_docs.append( new Document( *doc ) );
    }
}

Documents::~Documents()
{
    //kDebug(planDbg())<<this;
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
    doc->parent = this;
    if ( node ) {
        node->emitDocumentAdded( node, doc, m_docs.count() - 1 );
    }
}

void Documents::addDocument( const KUrl &url, Document::Type type )
{
    addDocument( new Document( url, type ) );
}

Document *Documents::takeDocument( int index )
{
    if ( index >= 0 && index < m_docs.count() ) {
        Document *doc = m_docs.takeAt( index );
        if ( doc ) {
            doc->parent = 0;
            if ( node ) {
                node->emitDocumentRemoved( node, doc, index );
            }
        }
        return doc;
    }
    return 0;
}

Document *Documents::takeDocument( Document *doc )
{
    Q_ASSERT( m_docs.contains( doc ) );
    int idx = m_docs.indexOf( doc );
    if ( idx >= 0 ) {
        takeDocument( idx );
        doc->parent = 0;
        if ( node ) {
            node->emitDocumentRemoved( node, doc, idx );
        }
        return doc;
    }
    return 0;
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
    kDebug(planDbg());
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
                status.addMsg( i18n( "Document loaded, URL=%1",  doc->url().url() ) );
            }
        }
    }
    return true;
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

void Documents::saveToStore( KoStore *store ) const
{
    foreach ( Document *doc, m_docs ) {
        if ( doc->sendAs() == Document::SendAs_Copy ) {
            QString path = doc->url().url();
            if ( doc->url().isLocalFile() ) {
                path = doc->url().toLocalFile();
            }
            kDebug(planDbg())<<"Copy file to store: "<<path<<doc->url().fileName();
            store->addLocalFile( path, doc->url().fileName() );

        }
    }
}

void Documents::documentChanged( Document *doc )
{
    if ( node ) {
        node->emitDocumentChanged( node, doc, indexOf( doc ) );
    }
}

} //namespace KPlato
