/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2004 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koDocumentInfo.h"
#include "kodom.h"
#include "koDocument.h"
#include "kofficeversion.h"
#include "koApplication.h"

#include <koStoreDevice.h>
#include <koxmlwriter.h>

#include <kconfig.h>
#include <kdebug.h>

#include <qobjectlist.h>
#include "koxmlns.h"

/*****************************************
 *
 * KoDocumentInfo
 *
 *****************************************/

KoDocumentInfo::KoDocumentInfo( QObject* parent, const char* name )
    : QObject( parent, name )
{
    (void)new KoDocumentInfoAuthor( this );
    (void)new KoDocumentInfoAbout( this );
}

KoDocumentInfo::~KoDocumentInfo()
{
}

// KOffice-1.3 format
bool KoDocumentInfo::load( const QDomDocument& doc )
{
    QStringList lst = pages();
    QStringList::ConstIterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
        KoDocumentInfoPage* p = page( *it );
        Q_ASSERT( p );
        if ( !p->load( doc.documentElement() ) )
            return false;
    }

    return true;
}

bool KoDocumentInfo::loadOasis( const QDomDocument& metaDoc )
{
    //kdDebug()<<" metaDoc.toString() :"<<metaDoc.toString()<<endl;
    QStringList lst = pages();
    QStringList::ConstIterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
        KoDocumentInfoPage* p = page( *it );
        Q_ASSERT( p );

        QDomNode meta   = KoDom::namedItemNS( metaDoc, KoXmlNS::office, "document-meta" );
        QDomNode office = KoDom::namedItemNS( meta, KoXmlNS::office, "meta" );

        if ( office.isNull() )
            return false;

        if ( !p->loadOasis( office ) )
            return false;
    }
    return true;
}

// KOffice-1.3 format
QDomDocument KoDocumentInfo::save()
{
    QDomDocument doc = KoDocument::createDomDocument( "document-info" /*DTD name*/, "document-info" /*tag name*/, "1.1" );
    QDomElement e = doc.documentElement();

    QStringList lst = pages();
    QStringList::ConstIterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
        KoDocumentInfoPage* p = page( *it );
        Q_ASSERT( p );
        QDomElement s = p->save( doc );
        if ( s.isNull() )
            return QDomDocument();
        e.appendChild( s );
    }

    return doc;
}

bool KoDocumentInfo::saveOasis( KoStore* store )
{
    KoStoreDevice dev( store );
    KoXmlWriter* xmlWriter = KoDocument::createOasisXmlWriter( &dev, "office:document-meta" );
    xmlWriter->startElement( "office:meta" );

    xmlWriter->startElement( "meta:generator");
    xmlWriter->addTextNode( QString( "KOffice/%1" ).arg( KOFFICE_VERSION_STRING ) );
    xmlWriter->endElement();
    QStringList lst = pages();
    QStringList::ConstIterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
        KoDocumentInfoPage* p = page( *it );
        Q_ASSERT( p );
        if ( !p->saveOasis( *xmlWriter ) )
            return false;
    }
    xmlWriter->endElement();
    xmlWriter->endElement(); // root element
    xmlWriter->endDocument();
    delete xmlWriter;
    return true;
}

KoDocumentInfoPage* KoDocumentInfo::page( const QString& name ) const
{
    QObject* obj = const_cast<KoDocumentInfo*>(this)->child( name.latin1() );

    return (KoDocumentInfoPage*)obj;
}

QStringList KoDocumentInfo::pages() const
{
    QStringList ret;

    const QObjectList *list = children();
    if ( list )
    {
        QObjectListIt it( *list );
        QObject *obj;
        while ( ( obj = it.current() ) )
        {
            ret.append( obj->name() );
            ++it;
        }
    }

    return ret;
}

QString KoDocumentInfo::title() const
{
    KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(page( "about" ));
    if ( !aboutPage ) {
        kdWarning() << "'About' page not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return aboutPage->title();
}

/*****************************************
 *
 * KoDocumentInfoPage
 *
 *****************************************/

KoDocumentInfoPage::KoDocumentInfoPage( QObject* parent, const char* name )
    : QObject( parent, name )
{
}

/*****************************************
 *
 * KoDocumentInfoAuthor
 *
 *****************************************/

KoDocumentInfoAuthor::KoDocumentInfoAuthor( KoDocumentInfo* info )
    : KoDocumentInfoPage( info, "author" )
{
    initParameters();
}

void KoDocumentInfoAuthor::initParameters()
{
    KConfig* config = KoGlobal::kofficeConfig();
    if ( config->hasGroup( "Author" ) ) {
        KConfigGroupSaver cgs( config, "Author" );
        m_telephoneHome=config->readEntry( "telephone" );
        m_telephoneWork=config->readEntry( "telephone-work" );
        m_fax=config->readEntry( "fax" );
        m_country=config->readEntry( "country" );
        m_postalCode=config->readEntry( "postal-code" );
        m_city=config->readEntry( "city" );
        m_street=config->readEntry( "street" );
    }
}

bool KoDocumentInfoAuthor::saveOasis( KoXmlWriter &xmlWriter )
{
    if ( !m_fullName.isEmpty() )
    {
     xmlWriter.startElement( "dc:creator");
     xmlWriter.addTextNode( m_fullName );
     xmlWriter.endElement();
    }
    if ( !m_initial.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "initial" );
     xmlWriter.addTextNode( m_initial );
     xmlWriter.endElement();
    }
    if ( !m_title.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "author-title" );
     xmlWriter.addTextNode( m_title );
     xmlWriter.endElement();
    }
    if ( !m_company.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "company" );
     xmlWriter.addTextNode( m_company );
     xmlWriter.endElement();
    }
    if ( !m_email.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "email" );
     xmlWriter.addTextNode( m_email );
     xmlWriter.endElement();
    }
    if ( !m_telephoneHome.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "telephone" );
     xmlWriter.addTextNode( m_telephoneHome );
     xmlWriter.endElement();
    }
    if ( !m_telephoneWork.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "telephone-work" );
     xmlWriter.addTextNode( m_telephoneWork );
     xmlWriter.endElement();
    }
    if ( !m_fax.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "fax" );
     xmlWriter.addTextNode( m_fax );
     xmlWriter.endElement();
    }
    if ( !m_country.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "country" );
     xmlWriter.addTextNode( m_country );
     xmlWriter.endElement();
    }
    if ( !m_postalCode.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "postal-code" );
     xmlWriter.addTextNode( m_postalCode );
     xmlWriter.endElement();
    }
    if ( !m_city.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "city" );
     xmlWriter.addTextNode( m_city );
     xmlWriter.endElement();
    }
    if ( !m_street.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "street" );
     xmlWriter.addTextNode( m_street );
     xmlWriter.endElement();
    }
    if ( !m_position.isEmpty() )
    {
     xmlWriter.startElement( "meta:user-defined");
     xmlWriter.addAttribute( "meta:name", "position" );
     xmlWriter.addTextNode( m_position );
     xmlWriter.endElement();
    }
    return true;
}

bool KoDocumentInfoAuthor::loadOasis( const QDomNode& metaDoc )
{
    QDomElement e = KoDom::namedItemNS( metaDoc, KoXmlNS::dc, "creator" );
    if ( !e.isNull() && !e.text().isEmpty() )
        m_fullName = e.text();
    QDomNode n = metaDoc.firstChild();
    for ( ; !n.isNull(); n = n.nextSibling() )
    {
        if (n.isElement())
        {
            QDomElement e = n.toElement();
            if ( e.namespaceURI() == KoXmlNS::meta && e.localName() == "user-defined" && !e.text().isEmpty() )
            {
                QString name = e.attributeNS( KoXmlNS::meta, "name", QString::null );
                if ( name == "initial" )
                    m_initial = e.text();
                else if ( name == "author-title" )
                    m_title = e.text();
                else if ( name == "company" )
                    m_company = e.text();
                else if ( name == "email" )
                    m_email = e.text();
                else if ( name == "telephone" )
                    m_telephoneHome = e.text();
                else if ( name == "telephone-work" )
                    m_telephoneWork = e.text();
                else if ( name == "fax" )
                    m_fax = e.text();
                else if ( name == "country" )
                    m_country = e.text();
                else if ( name == "postal-code" )
                    m_postalCode = e.text();
                else if ( name == "city" )
                    m_city = e.text();
                else if ( name == "street" )
                    m_street = e.text();
                else if ( name == "position" )
                    m_position = e.text();
            }
        }
    }
    return true;
}

// KOffice-1.3 format
bool KoDocumentInfoAuthor::load( const QDomElement& e )
{
    QDomNode n = e.namedItem( "author" ).firstChild();
    for( ; !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();
        if ( e.isNull() ) continue;
        if ( e.tagName() == "full-name" )
            m_fullName = e.text();
        else if ( e.tagName() == "initial" )
            m_initial = e.text();
        else if ( e.tagName() == "title" )
            m_title = e.text();
        else if ( e.tagName() == "company" )
            m_company = e.text();
        else if ( e.tagName() == "email" )
            m_email = e.text();
        else if ( e.tagName() == "telephone" )
            m_telephoneHome = e.text();
        else if ( e.tagName() == "telephone-work" )
            m_telephoneWork = e.text();
        else if ( e.tagName() == "fax" )
            m_fax = e.text();
        else if ( e.tagName() == "country" )
            m_country = e.text();
        else if ( e.tagName() == "postal-code" )
            m_postalCode = e.text();
        else if ( e.tagName() == "city" )
            m_city = e.text();
        else if ( e.tagName() == "street" )
            m_street = e.text();
        else if ( e.tagName() == "position" )
            m_position = e.text();
    }
    return true;
}

QDomElement KoDocumentInfoAuthor::save( QDomDocument& doc )
{
    QDomElement e = doc.createElement( "author" );

    QDomElement t = doc.createElement( "full-name" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_fullName ) );

    t = doc.createElement( "initial" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_initial ) );


    t = doc.createElement( "title" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_title ) );

    t = doc.createElement( "company" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_company ) );

    t = doc.createElement( "email" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_email ) );

    t = doc.createElement( "telephone" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_telephoneHome ) );

    t = doc.createElement( "telephone-work" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_telephoneWork ) );

    t = doc.createElement( "fax" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_fax ) );

    t = doc.createElement( "country" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_country ) );

    t = doc.createElement( "postal-code" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_postalCode ) );

    t = doc.createElement( "city" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_city ) );

    t = doc.createElement( "street" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_street ) );

    t = doc.createElement( "position" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_position ) );

    return e;
}

QString KoDocumentInfoAuthor::fullName() const
{
    return m_fullName;
}

QString KoDocumentInfoAuthor::initial() const
{
    return m_initial;
}

QString KoDocumentInfoAuthor::title() const
{
    return m_title;
}

QString KoDocumentInfoAuthor::company() const
{
    return m_company;
}

QString KoDocumentInfoAuthor::email() const
{
    return m_email;
}

QString KoDocumentInfoAuthor::telephoneHome() const
{
    return m_telephoneHome;
}

QString KoDocumentInfoAuthor::telephoneWork() const
{
    return m_telephoneWork;
}

QString KoDocumentInfoAuthor::fax() const
{
    return m_fax;
}

QString KoDocumentInfoAuthor::country() const
{
    return m_country;
}

QString KoDocumentInfoAuthor::postalCode() const
{
    return m_postalCode;
}

QString KoDocumentInfoAuthor::city() const
{
    return m_city;
}

QString KoDocumentInfoAuthor::street() const
{
    return m_street;
}

QString KoDocumentInfoAuthor::position() const
{
    return m_position;
}

void KoDocumentInfoAuthor::setFullName( const QString& n )
{
    m_fullName = n;
}

void KoDocumentInfoAuthor::setInitial( const QString& n )
{
    m_initial = n;
}

void KoDocumentInfoAuthor::setTitle( const QString& n )
{
    m_title = n;
}

void KoDocumentInfoAuthor::setCompany( const QString& n )
{
    m_company = n;
}

void KoDocumentInfoAuthor::setEmail( const QString& n )
{
    m_email = n;
}

void KoDocumentInfoAuthor::setTelephoneHome( const QString& n )
{
    m_telephoneHome = n;
}

void KoDocumentInfoAuthor::setTelephoneWork( const QString& n )
{
    m_telephoneWork = n;
}

void KoDocumentInfoAuthor::setFax( const QString& n )
{
    m_fax = n;
}

void KoDocumentInfoAuthor::setCountry( const QString& n )
{
    m_country = n;
}

void KoDocumentInfoAuthor::setPostalCode( const QString& n )
{
    m_postalCode = n;
}

void KoDocumentInfoAuthor::setCity( const QString& n )
{
    m_city = n;
}

void KoDocumentInfoAuthor::setStreet( const QString& n )
{
    m_street = n;
}

void KoDocumentInfoAuthor::setPosition( const QString& n )
{
    m_position = n;
}


/*****************************************
 *
 * KoDocumentInfoAbout
 *
 *****************************************/

KoDocumentInfoAbout::KoDocumentInfoAbout( KoDocumentInfo* info )
    : KoDocumentInfoPage( info, "about" )
{
}

bool KoDocumentInfoAbout::saveOasis( KoXmlWriter &xmlWriter )
{
    if ( !m_title.isEmpty() )
    {
     xmlWriter.startElement( "dc:title");
     xmlWriter.addTextNode( m_title );
     xmlWriter.endElement();
    }
    if ( !m_abstract.isEmpty() )
    {
     xmlWriter.startElement( "dc:description");
     xmlWriter.addTextNode( m_abstract );
     xmlWriter.endElement();
    }
    if ( !m_keywords.isEmpty() )
    {
     xmlWriter.startElement( "meta:keyword");
     xmlWriter.addTextNode( m_keywords );
     xmlWriter.endElement();
    }
    if ( !m_subject.isEmpty() )
    {
     xmlWriter.startElement( "dc:subject");
     xmlWriter.addTextNode( m_subject );
     xmlWriter.endElement();
    }

    return true;
}

bool KoDocumentInfoAbout::loadOasis( const QDomNode& metaDoc )
{
    QDomElement e  = KoDom::namedItemNS( metaDoc, KoXmlNS::dc, "title" );
    if ( !e.isNull() && !e.text().isEmpty() )
    {
        m_title = e.text();
    }
    e  = KoDom::namedItemNS( metaDoc, KoXmlNS::dc, "description" );
    if ( !e.isNull() && !e.text().isEmpty() )
    {
        m_abstract = e.text();
    }
    e  = KoDom::namedItemNS( metaDoc, KoXmlNS::dc, "subject" );
    if ( !e.isNull() && !e.text().isEmpty() )
    {
        m_subject = e.text();
    }
    e  = KoDom::namedItemNS( metaDoc, KoXmlNS::meta, "keyword" );
    if ( !e.isNull() && !e.text().isEmpty() )
    {
        m_keywords = e.text();
    }
    return true;
}

// KOffice-1.3 format
bool KoDocumentInfoAbout::load( const QDomElement& e )
{
    QDomNode n = e.namedItem( "about" ).firstChild();
    for( ; !n.isNull(); n = n.nextSibling()  )
    {
        QDomElement e = n.toElement();
        if ( e.isNull() ) continue;
        if ( e.tagName() == "abstract" )
            m_abstract = e.text();
        else if ( e.tagName() == "title" )
            m_title = e.text();
        else if ( e.tagName() == "subject" )
            m_subject = e.text();
        else if ( e.tagName() == "keyword" )
            m_keywords = e.text();
    }

    return true;
}

// KOffice-1.3 format
QDomElement KoDocumentInfoAbout::save( QDomDocument& doc )
{
    QDomElement e = doc.createElement( "about" );

    QDomElement t = doc.createElement( "abstract" );
    e.appendChild( t );
    t.appendChild( doc.createCDATASection( m_abstract ) );

    t = doc.createElement( "title" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_title ) );

    t = doc.createElement( "keyword" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_keywords ) );

    t = doc.createElement( "subject" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_subject ) );

    return e;
}

QString KoDocumentInfoAbout::title() const
{
    return m_title;
}

QString KoDocumentInfoAbout::abstract() const
{
    return m_abstract;
}

void KoDocumentInfoAbout::setTitle( const QString& n )
{
    m_title = n;
}

void KoDocumentInfoAbout::setAbstract( const QString& n )
{
    m_abstract = n;
}

QString KoDocumentInfoAbout::keywords() const
{
    return m_keywords;
}

QString KoDocumentInfoAbout::subject() const
{
    return m_subject;
}

void KoDocumentInfoAbout::setKeywords( const QString& n )
{
    m_keywords = n;
}

void KoDocumentInfoAbout::setSubject( const QString& n )
{
    m_subject = n;
}



#include <koDocumentInfo.moc>
