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

#ifndef KO_DOCUMENT_INFO_H
#define KO_DOCUMENT_INFO_H

#include <qobject.h>

class QString;
class QStringList;
class QDomDocument;
class QDomElement;
class QDomNode;
class KoStore;
class KoDocumentInfoPage;
class KoXmlWriter;

class KoDocumentInfo : public QObject
{
    Q_OBJECT
public:
    KoDocumentInfo( QObject* parent = 0, const char* name = 0 );
    virtual ~KoDocumentInfo();

    bool load( const QDomDocument& doc );
    bool loadOasis( const QDomDocument& metaDoc );

    QDomDocument save();
    bool saveOasis( KoStore* store );

    /**
     * This info has an accessor because it's the most commonly used.
     * Equivalent to page("about")->title() (but checking that the page exists)
     */
    QString title() const;

    KoDocumentInfoPage* page( const QString& name ) const;
    QStringList pages() const;
    void documentInfochanged() { emit sigDocumentInfoModifed();}
 signals:
    void sigDocumentInfoModifed();
};

class KoDocumentInfoPage : public QObject
{
public:
    KoDocumentInfoPage( QObject *parent, const char* name );

    virtual bool load( const QDomElement& e ) = 0;
    virtual QDomElement save( QDomDocument& doc ) = 0;
    virtual bool loadOasis( const QDomNode& metaDoc ) = 0;
    virtual bool saveOasis( KoXmlWriter &xmlWriter ) = 0;

};

class KoDocumentInfoAuthor : public KoDocumentInfoPage
{
    Q_OBJECT
public:
    KoDocumentInfoAuthor( KoDocumentInfo* info );

    virtual bool load( const QDomElement& e );
    virtual QDomElement save( QDomDocument& doc );
    virtual bool loadOasis( const QDomNode& metaDoc );
    virtual bool saveOasis( KoXmlWriter &xmlWriter );

    QString fullName() const;
    QString initial() const;
    QString title() const;
    QString company() const;
    QString email() const;
    QString telephoneHome() const;
    QString telephoneWork() const;
    QString fax() const;
    QString country() const;
    QString postalCode() const;
    QString city() const;
    QString street() const;
    QString position() const;

    void setFullName( const QString& n );
    void setTitle( const QString& n );
    void setCompany( const QString& n );
    void setEmail( const QString& n );
    void setTelephoneHome( const QString& n );
    void setTelephoneWork( const QString& n );
    void setFax( const QString& n );
    void setCountry( const QString& n );
    void setPostalCode( const QString& n );
    void setCity( const QString& n );
    void setStreet( const QString& n );
    void setInitial( const QString& n );
    void setPosition( const QString& n );
    void initParameters();

private:
    QString m_fullName;
    QString m_title;
    QString m_company;
    QString m_email;
    QString m_telephoneHome;
    QString m_telephoneWork;
    QString m_fax;
    QString m_country;
    QString m_postalCode;
    QString m_city;
    QString m_street;
    QString m_initial;
    QString m_position;
};

class KoDocumentInfoAbout : public KoDocumentInfoPage
{
    Q_OBJECT
public:
    KoDocumentInfoAbout( KoDocumentInfo* info );

    virtual bool load( const QDomElement& e );
    virtual QDomElement save( QDomDocument& doc );
    virtual bool loadOasis( const QDomNode& metaDoc );
    virtual bool saveOasis( KoXmlWriter &xmlWriter );

    QString title() const;
    QString abstract() const;
    QString subject() const;
    QString keywords() const;

    void setKeywords( const QString &n );
    void setSubject( const QString& n );

    void setTitle( const QString& n );
    void setAbstract( const QString& n );

private:
    QString m_title;
    QString m_abstract;
    QString m_keywords;
    QString m_subject;
};

#endif
