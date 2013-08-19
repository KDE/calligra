/*
 * This file is part of the KDE Libraries
 * Copyright (C) 2000 Espen Sand (espen@kde.org)
 * Copyright (C) 2006 Nicolas GOUTTE <goutte@kde.org>
 * Copyright (C) 2008 Friedrich W. H. Kossebau <kossebau@kde.org>
 * Copyright (C) 2010 Teo Mrnjavac <teo@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "kaboutdata.h"

#include <QCoreApplication>
#include "qstandardpaths.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QSharedData>
#include <QtCore/QVariant>
#include <QtCore/QList>
#include <QHash>

// PORTING HACK (KDE5 TODO: clean up)
#define i18nc(a,b) QObject::tr(b, a)

class KAboutPerson::Private
{
public:
   QString _name;
   QString _task;
   QString _emailAddress;
   QString _webAddress;
   QString _ocsUsername;
};

KAboutPerson::KAboutPerson( const QString &_name,
                            const QString &_task,
                            const QString &_emailAddress,
                            const QString &_webAddress,
                            const QString &_ocsUsername )
  : d(new Private)
{
   d->_name = _name;
   d->_task = _task;
   d->_emailAddress = _emailAddress;
   d->_webAddress = _webAddress;
   d->_ocsUsername = _ocsUsername;
}

KAboutPerson::KAboutPerson(const QString &_name, const QString &_email, bool)
  : d(new Private)
{
   d->_name = _name;
   d->_emailAddress = _email;
}

KAboutPerson::KAboutPerson(const KAboutPerson& other): d(new Private)
{
    *d = *other.d;
}

KAboutPerson::~KAboutPerson()
{
   delete d;
}

QString KAboutPerson::name() const
{
   return d->_name;
}

QString KAboutPerson::task() const
{
    return d->_task;
}

QString KAboutPerson::emailAddress() const
{
   return d->_emailAddress;
}


QString KAboutPerson::webAddress() const
{
   return d->_webAddress;
}

QString KAboutPerson::ocsUsername() const
{
    return d->_ocsUsername;
}

KAboutPerson &KAboutPerson::operator=(const KAboutPerson& other)
{
   *d = *other.d;
   return *this;
}



class KAboutLicense::Private : public QSharedData
{
public:
    Private( enum KAboutData::LicenseKey licenseType, const KAboutData *aboutData );
    Private(const KAboutData *aboutData);
    Private( const Private& other);
public:
    enum KAboutData::LicenseKey  _licenseKey;
    QString                      _licenseText;
    QString                      _pathToLicenseTextFile;
    // needed for access to the possibly changing copyrightStatement()
    const KAboutData *           _aboutData;
};

KAboutLicense::Private::Private( enum KAboutData::LicenseKey licenseType, const KAboutData *aboutData )
  : QSharedData(),
    _licenseKey( licenseType ),
    _aboutData( aboutData )
{
}

KAboutLicense::Private::Private(const KAboutData *aboutData)
  : QSharedData(),
    _aboutData( aboutData )
{
}

KAboutLicense::Private::Private(const KAboutLicense::Private& other)
  : QSharedData(other),
    _licenseKey( other._licenseKey ),
    _licenseText( other._licenseText ),
    _pathToLicenseTextFile( other._pathToLicenseTextFile ),
    _aboutData( other._aboutData )
{}


KAboutLicense::KAboutLicense( enum KAboutData::LicenseKey licenseType, const KAboutData *aboutData )
  : d(new Private(licenseType,aboutData))
{
}

KAboutLicense::KAboutLicense(const KAboutData *aboutData )
  : d(new Private(aboutData))
{
}

KAboutLicense::KAboutLicense(const KAboutLicense& other)
  : d(other.d)
{
}

KAboutLicense::~KAboutLicense()
{}

void KAboutLicense::setLicenseFromPath(const QString& pathToFile)
{
    d->_licenseKey = KAboutData::License_File;
    d->_pathToLicenseTextFile = pathToFile;
}

void KAboutLicense::setLicenseFromText(const QString& licenseText)
{
    d->_licenseKey = KAboutData::License_Custom;
    d->_licenseText = licenseText;
}

QString KAboutLicense::text() const
{
    QString result;

    const QString lineFeed = QString::fromLatin1( "\n\n" );

    if (d->_aboutData && !d->_aboutData->copyrightStatement().isEmpty()) {
        result = d->_aboutData->copyrightStatement() + lineFeed;
    }

    bool knownLicense = false;
    QString pathToFile;
    switch ( d->_licenseKey )
    {
    case KAboutData::License_File:
        pathToFile = d->_pathToLicenseTextFile;
        break;
    case KAboutData::License_GPL_V2:
        knownLicense = true;
        pathToFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QString::fromLatin1("LICENSES/GPL_V2"));
        break;
    case KAboutData::License_LGPL_V2:
        knownLicense = true;
        pathToFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QString::fromLatin1("LICENSES/LGPL_V2"));
        break;
    case KAboutData::License_BSD:
        knownLicense = true;
        pathToFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QString::fromLatin1("LICENSES/BSD"));
        break;
    case KAboutData::License_Artistic:
        knownLicense = true;
        pathToFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QString::fromLatin1("LICENSES/ARTISTIC"));
        break;
    case KAboutData::License_QPL_V1_0:
        knownLicense = true;
        pathToFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QString::fromLatin1("LICENSES/QPL_V1.0"));
        break;
    case KAboutData::License_GPL_V3:
        knownLicense = true;
        pathToFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QString::fromLatin1("LICENSES/GPL_V3"));
        break;
    case KAboutData::License_LGPL_V3:
        knownLicense = true;
        pathToFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QString::fromLatin1("LICENSES/LGPL_V3"));
        break;
    case KAboutData::License_Custom:
        if (!d->_licenseText.isEmpty()) {
            result = d->_licenseText;
            break;
        }
        // fall through
    default:
        result += QObject::tr("No licensing terms for this program have been specified.\n"
                       "Please check the documentation or the source for any\n"
                       "licensing terms.\n");
    }

    if (knownLicense) {
        result += QObject::tr("This program is distributed under the terms of the %1.").arg(name(KAboutData::ShortName));
        if (!pathToFile.isEmpty()) {
            result += lineFeed;
        }
    }

    if (!pathToFile.isEmpty()) {
        QFile file(pathToFile);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream str(&file);
            result += str.readAll();
        }
    }

    return result;
}


QString KAboutLicense::name(KAboutData::NameFormat formatName) const
{
    QString licenseShort;
    QString licenseFull;

    switch (d->_licenseKey) {
    case KAboutData::License_GPL_V2:
        licenseShort = i18nc("@item license (short name)","GPL v2");
        licenseFull = i18nc("@item license","GNU General Public License Version 2");
        break;
    case KAboutData::License_LGPL_V2:
        licenseShort = i18nc("@item license (short name)","LGPL v2");
        licenseFull = i18nc("@item license","GNU Lesser General Public License Version 2");
        break;
    case KAboutData::License_BSD:
        licenseShort = i18nc("@item license (short name)","BSD License");
        licenseFull = i18nc("@item license","BSD License");
        break;
    case KAboutData::License_Artistic:
        licenseShort = i18nc("@item license (short name)","Artistic License");
        licenseFull = i18nc("@item license","Artistic License");
        break;
    case KAboutData::License_QPL_V1_0:
        licenseShort = i18nc("@item license (short name)","QPL v1.0");
        licenseFull = i18nc("@item license","Q Public License");
        break;
    case KAboutData::License_GPL_V3:
        licenseShort = i18nc("@item license (short name)","GPL v3");
        licenseFull = i18nc("@item license","GNU General Public License Version 3");
        break;
    case KAboutData::License_LGPL_V3:
        licenseShort = i18nc("@item license (short name)","LGPL v3");
        licenseFull = i18nc("@item license","GNU Lesser General Public License Version 3");
        break;
    case KAboutData::License_Custom:
    case KAboutData::License_File:
        licenseShort = licenseFull = i18nc("@item license","Custom");
        break;
    default:
        licenseShort = licenseFull = i18nc("@item license","Not specified");
    }

    const QString result =
        (formatName == KAboutData::ShortName ) ? licenseShort :
        (formatName == KAboutData::FullName ) ?  licenseFull :
                                                 QString();

    return result;
}


KAboutLicense &KAboutLicense::operator=(const KAboutLicense& other)
{
   d = other.d;
   return *this;
}

KAboutData::LicenseKey KAboutLicense::key() const
{
    return d->_licenseKey;
}

KAboutLicense KAboutLicense::byKeyword(const QString &rawKeyword)
{
    // Setup keyword->enum dictionary on first call.
    // Use normalized keywords, by the algorithm below.
    static QHash<QByteArray, KAboutData::LicenseKey> ldict;
    if (ldict.isEmpty()) {
        ldict.insert("gpl", KAboutData::License_GPL);
        ldict.insert("gplv2", KAboutData::License_GPL_V2);
        ldict.insert("gplv2+", KAboutData::License_GPL_V2);
        ldict.insert("lgpl", KAboutData::License_LGPL);
        ldict.insert("lgplv2", KAboutData::License_LGPL_V2);
        ldict.insert("lgplv2+", KAboutData::License_LGPL_V2);
        ldict.insert("bsd", KAboutData::License_BSD);
        ldict.insert("artistic", KAboutData::License_Artistic);
        ldict.insert("qpl", KAboutData::License_QPL);
        ldict.insert("qplv1", KAboutData::License_QPL_V1_0);
        ldict.insert("qplv10", KAboutData::License_QPL_V1_0);
        ldict.insert("gplv3", KAboutData::License_GPL_V3);
        ldict.insert("gplv3+", KAboutData::License_GPL_V3);
        ldict.insert("lgplv3", KAboutData::License_LGPL_V3);
        ldict.insert("lgplv3+", KAboutData::License_LGPL_V3);
    }

    // Normalize keyword.
    QString keyword = rawKeyword;
    keyword = keyword.toLower();
    keyword.remove(QLatin1Char(' '));
    keyword.remove(QLatin1Char('.'));

    KAboutData::LicenseKey license = ldict.value(keyword.toLatin1(),
                                                 KAboutData::License_Custom);
    return KAboutLicense(license, 0);
}


class KAboutData::Private
{
public:
    Private()
        : customAuthorTextEnabled(false)
        {}
    QString _componentName;
    QString _displayName;
    QString _shortDescription;
    QString _catalogName;
    QString _copyrightStatement;
    QString _otherText;
    QString _homepageAddress;
    QList<KAboutPerson> _authorList;
    QList<KAboutPerson> _creditList;
    QList<KAboutLicense> _licenseList;
    QString translatorName;
    QString translatorEmail;
    QString productName;
    QString programIconName;
    QVariant programLogo;
    QString customAuthorPlainText, customAuthorRichText;
    bool customAuthorTextEnabled;

    QString organizationDomain;
    QString _ocsProviderUrl;

    // Everything dr.konqi needs, we store as utf-8, so we
    // can just give it a pointer, w/o any allocations.
    QByteArray _internalProgramName;
    QByteArray _version;
    QByteArray _bugEmailAddress;
};


KAboutData::KAboutData( const QString &_componentName,
                        const QString &_catalogName,
                        const QString &_displayName,
                        const QString &_version,
                        const QString &_shortDescription,
                        enum LicenseKey licenseType,
                        const QString &_copyrightStatement,
                        const QString &text,
                        const QString &homePageAddress,
                        const QString &bugsEmailAddress
                      )
  : d(new Private)
{
    d->_componentName = _componentName;
    int p = d->_componentName.indexOf(QLatin1Char('/'));
    if (p >= 0) {
        d->_componentName = d->_componentName.mid(p + 1);
    }

    d->_catalogName = _catalogName;
    d->_displayName = _displayName;
    if (!d->_displayName.isEmpty()) // KComponentData("klauncher") gives empty program name
        d->_internalProgramName = _displayName.toUtf8();
    d->_version = _version.toUtf8();
    d->_shortDescription = _shortDescription;
    d->_licenseList.append(KAboutLicense(licenseType,this));
    d->_copyrightStatement = _copyrightStatement;
    d->_otherText = text;
    d->_homepageAddress = homePageAddress;
    d->_bugEmailAddress = bugsEmailAddress.toUtf8();

    if (d->_homepageAddress.contains(QLatin1String("http://"))) {
        const int dot = d->_homepageAddress.indexOf(QLatin1Char('.'));
        if (dot >= 0) {
            d->organizationDomain = d->_homepageAddress.mid(dot + 1);
            const int slash = d->organizationDomain.indexOf(QLatin1Char('/'));
            if (slash >= 0)
                d->organizationDomain.truncate(slash);
        }
        else {
            d->organizationDomain = QString::fromLatin1("kde.org");
        }
    }
    else {
        d->organizationDomain = QString::fromLatin1("kde.org");
    }
}

KAboutData::~KAboutData()
{
    delete d;
}

KAboutData::KAboutData(const KAboutData& other): d(new Private)
{
    *d = *other.d;
    QList<KAboutLicense>::iterator it = d->_licenseList.begin(), itEnd = d->_licenseList.end();
    for ( ; it != itEnd; ++it) {
        KAboutLicense& al = *it;
        al.d.detach();
        al.d->_aboutData = this;
    }
}

KAboutData &KAboutData::operator=(const KAboutData& other)
{
    if (this != &other) {
        *d = *other.d;
        QList<KAboutLicense>::iterator it = d->_licenseList.begin(), itEnd = d->_licenseList.end();
        for ( ; it != itEnd; ++it) {
            KAboutLicense& al = *it;
            al.d.detach();
            al.d->_aboutData = this;
        }
    }
    return *this;
}

KAboutData &KAboutData::addAuthor(const QString &name,
                                   const QString &task,
                                   const QString &emailAddress,
                                   const QString &webAddress,
                                   const QString &ocsUsername )
{
  d->_authorList.append(KAboutPerson(name,task,emailAddress,webAddress,ocsUsername));
  return *this;
}

KAboutData &KAboutData::addCredit( const QString &name,
                                   const QString &task,
                                   const QString &emailAddress,
                                   const QString &webAddress,
                                   const QString &ocsUsername )
{
  d->_creditList.append(KAboutPerson(name,task,emailAddress,webAddress,ocsUsername));
  return *this;
}

KAboutData &KAboutData::setTranslator( const QString& name,
                                       const QString& emailAddress )
{
  d->translatorName = name;
  d->translatorEmail = emailAddress;
  return *this;
}

KAboutData &KAboutData::setLicenseText( const QString &licenseText )
{
    d->_licenseList[0] = KAboutLicense(this);
    d->_licenseList[0].setLicenseFromText(licenseText);
    return *this;
}

KAboutData &KAboutData::addLicenseText( const QString &licenseText )
{
    // if the default license is unknown, overwrite instead of append
    KAboutLicense &firstLicense = d->_licenseList[0];
    KAboutLicense newLicense(this);
    newLicense.setLicenseFromText(licenseText);
    if (d->_licenseList.count() == 1 && firstLicense.d->_licenseKey == License_Unknown) {
        firstLicense = newLicense;
    } else {
        d->_licenseList.append(newLicense);
    }

    return *this;
}

KAboutData &KAboutData::setLicenseTextFile( const QString &pathToFile )
{
    d->_licenseList[0] = KAboutLicense(this);
    d->_licenseList[0].setLicenseFromPath(pathToFile);
    return *this;
}

KAboutData &KAboutData::addLicenseTextFile( const QString &pathToFile )
{
    // if the default license is unknown, overwrite instead of append
    KAboutLicense &firstLicense = d->_licenseList[0];
    KAboutLicense newLicense(this);
    newLicense.setLicenseFromPath(pathToFile);
    if (d->_licenseList.count() == 1 && firstLicense.d->_licenseKey == License_Unknown) {
        firstLicense = newLicense;
    } else {
        d->_licenseList.append(newLicense);
    }
    return *this;
}

KAboutData &KAboutData::setComponentName(const QString &componentName)
{
  d->_componentName = componentName;
  return *this;
}

KAboutData &KAboutData::setDisplayName( const QString &_displayName )
{
    d->_displayName = _displayName;
    d->_internalProgramName = _displayName.toUtf8();
    return *this;
}

KAboutData &KAboutData::setOcsProvider(const QString &_ocsProviderUrl )
{
    d->_ocsProviderUrl = _ocsProviderUrl;
    return *this;
}

KAboutData &KAboutData::setVersion( const QByteArray &_version )
{
  d->_version = _version;
  return *this;
}

KAboutData &KAboutData::setShortDescription( const QString &_shortDescription )
{
  d->_shortDescription = _shortDescription;
  return *this;
}

KAboutData &KAboutData::setCatalogName(const QString &catalogName)
{
  d->_catalogName = catalogName;
  return *this;
}

KAboutData &KAboutData::setLicense( LicenseKey licenseKey)
{
    d->_licenseList[0] = KAboutLicense(licenseKey,this);
    return *this;
}

KAboutData &KAboutData::addLicense( LicenseKey licenseKey)
{
    // if the default license is unknown, overwrite instead of append
    KAboutLicense &firstLicense = d->_licenseList[0];
    if (d->_licenseList.count() == 1 && firstLicense.d->_licenseKey == License_Unknown) {
        firstLicense = KAboutLicense(licenseKey,this);
    } else {
        d->_licenseList.append(KAboutLicense(licenseKey,this));
    }
    return *this;
}

KAboutData &KAboutData::setCopyrightStatement( const QString &_copyrightStatement )
{
  d->_copyrightStatement = _copyrightStatement;
  return *this;
}

KAboutData &KAboutData::setOtherText( const QString &_otherText )
{
  d->_otherText = _otherText;
  return *this;
}

KAboutData &KAboutData::setHomepage(const QString &homepage)
{
  d->_homepageAddress = homepage;
  return *this;
}

KAboutData &KAboutData::setBugAddress( const QByteArray &_bugAddress )
{
  d->_bugEmailAddress = _bugAddress;
  return *this;
}

KAboutData &KAboutData::setOrganizationDomain( const QByteArray &domain )
{
  d->organizationDomain = QString::fromLatin1(domain.data());
  return *this;
}

KAboutData &KAboutData::setProductName( const QByteArray &_productName )
{
  d->productName = QString::fromUtf8(_productName.data());
  return *this;
}

QString KAboutData::componentName() const
{
  return d->_componentName;
}

QString KAboutData::productName() const
{
   if (!d->productName.isEmpty())
      return d->productName;
   return componentName();
}

QString KAboutData::displayName() const
{
    if (!d->_displayName.isEmpty())
        return d->_displayName;
    return componentName();
}

/// @internal
/// Return the program name. It is always pre-allocated.
/// Needed for KCrash in particular.
const char* KAboutData::internalProgramName() const
{
   return d->_internalProgramName.constData();
}

QString KAboutData::programIconName() const
{
    return d->programIconName.isEmpty() ? componentName() : d->programIconName;
}

KAboutData &KAboutData::setProgramIconName( const QString &iconName )
{
    d->programIconName = iconName;
    return *this;
}

QVariant KAboutData::programLogo() const
{
    return d->programLogo;
}

KAboutData &KAboutData::setProgramLogo(const QVariant& image)
{
    d->programLogo = image ;
    return *this;
}

QString KAboutData::ocsProviderUrl() const
{
    return d->_ocsProviderUrl;
}

QString KAboutData::version() const
{
    return QString::fromUtf8(d->_version.data());
}

/// @internal
/// Return the untranslated and uninterpreted (to UTF8) string
/// for the version information. Used in particular for KCrash.
const char* KAboutData::internalVersion() const
{
   return d->_version.constData();
}

QString KAboutData::shortDescription() const
{
    return d->_shortDescription;
}

QString KAboutData::catalogName() const
{
   if (!d->_catalogName.isEmpty())
     return d->_catalogName;
   // Fallback to appname for catalog name if empty.
   return d->_componentName;
}

QString KAboutData::homepage() const
{
   return d->_homepageAddress;
}

QString KAboutData::bugAddress() const
{
   return QString::fromUtf8(d->_bugEmailAddress.constData());
}

QString KAboutData::organizationDomain() const
{
    return d->organizationDomain;
}


/// @internal
/// Return the untranslated and uninterpreted (to UTF8) string
/// for the bug mail address. Used in particular for KCrash.
const char* KAboutData::internalBugAddress() const
{
   if (d->_bugEmailAddress.isEmpty())
      return 0;
   return d->_bugEmailAddress.constData();
}

QList<KAboutPerson> KAboutData::authors() const
{
   return d->_authorList;
}

QList<KAboutPerson> KAboutData::credits() const
{
   return d->_creditList;
}

#define NAME_OF_TRANSLATORS "Your names"
#define EMAIL_OF_TRANSLATORS "Your emails"
QList<KAboutPerson> KAboutData::translators() const
{
    QList<KAboutPerson> personList;
#pragma message("KDE5 TODO: What about this code ?")
#if 0
    KLocale *tmpLocale = NULL;
    if (KLocale::global()) {
        // There could be many catalogs loaded into the global locale,
        // e.g. in systemsettings. The tmp locale is needed to make sure we
        // use the translators name from this aboutdata's catalog, rather than
        // from any other loaded catalog.
        tmpLocale = new KLocale(*KLocale::global());
        tmpLocale->setActiveCatalog(catalogName());
    }
#endif
    QString translatorName;
    if (!d->translatorName.isEmpty()) {
        translatorName = d->translatorName;
    }
    else {
        translatorName = i18nc("NAME OF TRANSLATORS", NAME_OF_TRANSLATORS); //toString(tmpLocale);
    }

    QString translatorEmail;
    if (!d->translatorEmail.isEmpty()) {
        translatorEmail = d->translatorEmail;
    }
    else {
        translatorEmail = i18nc("EMAIL OF TRANSLATORS", EMAIL_OF_TRANSLATORS); //toString(tmpLocale);
    }
#if 0
    delete tmpLocale;
#endif
    if ( translatorName.isEmpty() || translatorName == QString::fromUtf8( NAME_OF_TRANSLATORS ) )
        return personList;

    const QStringList nameList(translatorName.split(QString(QLatin1Char(','))));

    QStringList emailList;
    if( !translatorEmail.isEmpty() && translatorEmail != QString::fromUtf8( EMAIL_OF_TRANSLATORS ) )
    {
       emailList = translatorEmail.split(QString(QLatin1Char(',')), QString::KeepEmptyParts);
    }

    QStringList::const_iterator nit;
    QStringList::const_iterator eit = emailList.constBegin();

    for( nit = nameList.constBegin(); nit != nameList.constEnd(); ++nit )
    {
        QString email;
        if ( eit != emailList.constEnd() )
        {
            email = *eit;
            ++eit;
        }

        personList.append(KAboutPerson((*nit).trimmed(), email.trimmed(), true));
    }

    return personList;
}

QString KAboutData::aboutTranslationTeam()
{
    return i18nc("replace this with information about your translation team",
            "<p>KDE is translated into many languages thanks to the work "
            "of the translation teams all over the world.</p>"
            "<p>For more information on KDE internationalization "
            "visit <a href=\"http://l10n.kde.org\">http://l10n.kde.org</a></p>"
            );
}

QString KAboutData::otherText() const
{
    return d->_otherText;
}

QString KAboutData::license() const
{
    return d->_licenseList.at(0).text();
}

QString KAboutData::licenseName( NameFormat formatName ) const
{
    return d->_licenseList.at(0).name(formatName);
}

QList<KAboutLicense> KAboutData::licenses() const
{
    return d->_licenseList;
}

QString KAboutData::copyrightStatement() const
{
    return d->_copyrightStatement;
}

QString KAboutData::customAuthorPlainText() const
{
    return d->customAuthorPlainText;
}

QString KAboutData::customAuthorRichText() const
{
    return d->customAuthorRichText;
}

bool KAboutData::customAuthorTextEnabled() const
{
  return d->customAuthorTextEnabled;
}

KAboutData &KAboutData::setCustomAuthorText( const QString &plainText,
                                             const QString &richText )
{
  d->customAuthorPlainText = plainText;
  d->customAuthorRichText = richText;

  d->customAuthorTextEnabled = true;

  return *this;
}

KAboutData &KAboutData::unsetCustomAuthorText()
{
  d->customAuthorPlainText = QString();
  d->customAuthorRichText = QString();

  d->customAuthorTextEnabled = false;

  return *this;
}

class KAboutDataRegistry
{
public:
    KAboutDataRegistry() : m_appData(0) {}
    ~KAboutDataRegistry() {
        delete m_appData;
        qDeleteAll(m_pluginData);
    }
    KAboutData *m_appData;
    QHash<QString, KAboutData*> m_pluginData;
};

Q_GLOBAL_STATIC(KAboutDataRegistry, s_registry);

KAboutData KAboutData::applicationData()
{
    if (!s_registry->m_appData) {
        s_registry->m_appData = new KAboutData(QCoreApplication::applicationName(),
                                               QCoreApplication::applicationName(),
                                               QString(),
                                               QString());
    }
    if (s_registry->m_appData->displayName().isEmpty() && QCoreApplication::instance()) {
        // ## add a bool to avoid doing this too often?
        s_registry->m_appData->setDisplayName(QCoreApplication::instance()->property("applicationDisplayName").toString());
    }
    return *s_registry->m_appData;
}

void KAboutData::setApplicationData(const KAboutData &aboutData)
{
    if (s_registry->m_appData)
        *s_registry->m_appData = aboutData;
    else
        s_registry->m_appData = new KAboutData(aboutData);

    if (QCoreApplication::instance())
        QCoreApplication::instance()->setProperty("applicationIconName", s_registry->m_appData->programIconName());
}

void KAboutData::registerPluginData(const KAboutData &aboutData)
{
    s_registry->m_pluginData.insert(aboutData.componentName(), new KAboutData(aboutData));
}

KAboutData* KAboutData::pluginData(const QString &componentName)
{
    KAboutData *ad = s_registry->m_pluginData.value(componentName);
    return ad;
}

// only for KCrash (no memory allocation allowed)
const KAboutData* KAboutData::applicationDataPointer()
{
    if (s_registry.exists())
        return s_registry->m_appData;
    return 0;
}
