#ifndef KO_DOCUMENT_INFO_H
#define KO_DOCUMENT_INFO_H

#include <qobject.h>
#include <qstringlist.h>
#include <qdom.h>

class QString;

class KoDocumentInfoPage;

class KoDocumentInfo : public QObject
{
public:
    KoDocumentInfo( QObject* parent = 0, const char* name = 0 );

    bool load( const QDomDocument& doc );
    QDomDocument save();

    KoDocumentInfoPage* page( const QString& name );
    QStringList pages();
};

class KoDocumentInfoPage : public QObject
{
public:
    KoDocumentInfoPage( QObject *parent, const char* name );

    virtual bool load( const QDomElement& e ) = 0;
    virtual QDomElement save( QDomDocument& doc ) = 0;
};

class KoDocumentInfoLog : public KoDocumentInfoPage
{
    Q_OBJECT
public:
    KoDocumentInfoLog( KoDocumentInfo* info );

    virtual bool load( const QDomElement& e );
    virtual QDomElement save( QDomDocument& doc );

    void setNewLog( const QString& log );
    void setOldLog( const QString& log );

    QString oldLog() const;
    QString newLog() const;

private:
    QString m_oldLog;
    QString m_newLog;
};

class KoDocumentInfoAuthor : public KoDocumentInfoPage
{
    Q_OBJECT
public:
    KoDocumentInfoAuthor( KoDocumentInfo* info );

    virtual bool load( const QDomElement& e );
    virtual QDomElement save( QDomDocument& doc );

    QString fullName() const;
    QString title() const;
    QString company() const;
    QString email() const;
    QString telephone() const;
    QString fax() const;
    QString country() const;
    QString postalCode() const;
    QString city() const;
    QString street() const;

    void setFullName( const QString& n );
    void setTitle( const QString& n );
    void setCompany( const QString& n );
    void setEmail( const QString& n );
    void setTelephone( const QString& n );
    void setFax( const QString& n );
    void setCountry( const QString& n );
    void setPostalCode( const QString& n );
    void setCity( const QString& n );
    void setStreet( const QString& n );

private:
    QString m_fullName;
    QString m_title;
    QString m_company;
    QString m_email;
    QString m_telephone;
    QString m_fax;
    QString m_country;
    QString m_postalCode;
    QString m_city;
    QString m_street;
};

class KoDocumentInfoAbout : public KoDocumentInfoPage
{
    Q_OBJECT
public:
    KoDocumentInfoAbout( KoDocumentInfo* info );

    virtual bool load( const QDomElement& e );
    virtual QDomElement save( QDomDocument& doc );

    QString title() const;
    QString abstract() const;

    void setTitle( const QString& n );
    void setAbstract( const QString& n );

private:
    QString m_title;
    QString m_abstract;
};

#endif
