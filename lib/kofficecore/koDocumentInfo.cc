/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include <koDocumentInfo.h>

#include <qdom.h>
#include <qobjectlist.h>
#include <kconfig.h>

/*****************************************
 *
 * KoDocumentInfo
 *
 *****************************************/

KoDocumentInfo::KoDocumentInfo( QObject* parent, const char* name )
    : QObject( parent, name )
{
    (void)new KoDocumentInfoLog( this );
    (void)new KoDocumentInfoAuthor( this );
    (void)new KoDocumentInfoAbout( this );
}

bool KoDocumentInfo::load( const QDomDocument& doc )
{
    QStringList lst = pages();
    QStringList::ConstIterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
        KoDocumentInfoPage* p = page( *it );
        ASSERT( p );
        if ( !p->load( doc.documentElement() ) )
            return false;
    }

    return true;
}

QDomDocument KoDocumentInfo::save()
{
    QDomDocument doc( "document-info" );
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement e = doc.createElement( "document-info" );
    doc.appendChild( e );

    QStringList lst = pages();
    QStringList::ConstIterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
        KoDocumentInfoPage* p = page( *it );
        ASSERT( p );
        QDomElement s = p->save( doc );
        if ( s.isNull() )
            return QDomDocument();
        e.appendChild( s );
    }

    return doc;
}

KoDocumentInfoPage* KoDocumentInfo::page( const QString& name )
{
    QObject* obj = child( name.latin1() );

    return (KoDocumentInfoPage*)obj;
}

QStringList KoDocumentInfo::pages()
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
 * KoDocumentInfoLog
 *
 *****************************************/

KoDocumentInfoLog::KoDocumentInfoLog( KoDocumentInfo* info )
    : KoDocumentInfoPage( info, "log" )
{
}

bool KoDocumentInfoLog::load( const QDomElement& e )
{
    m_newLog = QString::null;

    QDomElement n = e.namedItem( "log" ).firstChild().toElement();
    for( ; !n.isNull(); n = n.nextSibling().toElement() )
    {
        if ( n.tagName() == "text" )
            m_oldLog = n.text();
    }

    return true;
}

QDomElement KoDocumentInfoLog::save( QDomDocument& doc )
{
    QString text = m_oldLog;
    if ( !m_newLog.isEmpty() )
    {
        text += "\n";
        text += m_newLog;
    }

    QDomElement e = doc.createElement( "log" );
    QDomElement t = doc.createElement( "text" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_newLog ) );

    return e;
}

void KoDocumentInfoLog::setNewLog( const QString& log )
{
    m_newLog = log;
}

void KoDocumentInfoLog::setOldLog( const QString& log )
{
    m_oldLog = log;
}

QString KoDocumentInfoLog::oldLog() const
{
    return m_oldLog;
}

QString KoDocumentInfoLog::newLog() const
{
    return m_newLog;
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
    KConfig config("kofficerc");
    if( config.hasGroup( "Author" ))
    {
        config.setGroup( "Author" );
        m_fullName=config.readEntry("full-name","");
        m_company=config.readEntry("company", "");
        m_email=config.readEntry("email", "");
        m_telephone=config.readEntry("telephone", "");
        m_fax=config.readEntry("fax", "");
        m_country=config.readEntry("country", "");
        m_postalCode=config.readEntry("postal-code", "");
        m_city=config.readEntry("city", "");
        m_street=config.readEntry("street", "");
    }
}

bool KoDocumentInfoAuthor::load( const QDomElement& e )
{
    QDomElement n = e.namedItem( "author" ).firstChild().toElement();
    for( ; !n.isNull(); n = n.nextSibling().toElement() )
    {
        if ( n.tagName() == "full-name" )
            m_fullName = n.text();
        else if ( n.tagName() == "title" )
            m_title = n.text();
        else if ( n.tagName() == "company" )
            m_company = n.text();
        else if ( n.tagName() == "email" )
            m_email = n.text();
        else if ( n.tagName() == "telephone" )
            m_telephone = n.text();
        else if ( n.tagName() == "fax" )
            m_fax = n.text();
        else if ( n.tagName() == "country" )
            m_country = n.text();
        else if ( n.tagName() == "postal-code" )
            m_postalCode = n.text();
        else if ( n.tagName() == "city" )
            m_city = n.text();
        else if ( n.tagName() == "street" )
            m_street = n.text();
    }
    return true;
}

QDomElement KoDocumentInfoAuthor::save( QDomDocument& doc )
{
    QDomElement e = doc.createElement( "author" );

    QDomElement t = doc.createElement( "full-name" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_fullName ) );

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
    t.appendChild( doc.createTextNode( m_telephone ) );

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

    return e;
}

QString KoDocumentInfoAuthor::fullName() const
{
    return m_fullName;
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

QString KoDocumentInfoAuthor::telephone() const
{
    return m_telephone;
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

void KoDocumentInfoAuthor::setFullName( const QString& n )
{
    m_fullName = n;
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

void KoDocumentInfoAuthor::setTelephone( const QString& n )
{
    m_telephone = n;
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

/*****************************************
 *
 * KoDocumentInfoAbout
 *
 *****************************************/

KoDocumentInfoAbout::KoDocumentInfoAbout( KoDocumentInfo* info )
    : KoDocumentInfoPage( info, "about" )
{
}

bool KoDocumentInfoAbout::load( const QDomElement& e )
{
    QDomElement n = e.namedItem( "about" ).firstChild().toElement();
    for( ; !n.isNull(); n = n.nextSibling().toElement() )
    {
        if ( n.tagName() == "abstract" )
            m_abstract = n.text();
        else if ( n.tagName() == "title" )
            m_title = n.text();
    }

    return true;
}

QDomElement KoDocumentInfoAbout::save( QDomDocument& doc )
{
    QDomElement e = doc.createElement( "about" );

    QDomElement t = doc.createElement( "abstract" );
    e.appendChild( t );
    t.appendChild( doc.createCDATASection( m_abstract ) );

    t = doc.createElement( "title" );
    e.appendChild( t );
    t.appendChild( doc.createTextNode( m_title ) );

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

#include <koDocumentInfo.moc>
