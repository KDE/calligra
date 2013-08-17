/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 - 2001 Waldo Bastian <bastian@kde.org>
 *  Copyright (C) 1999 - 2005 David Faure   <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "kservice.h"
#include "kservice_p.h"
#include "kmimetypefactory.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

#include <QtCore/QCharRef>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QMap>
#include <qmimedatabase.h>

#include <kcoreauthorized.h>
#include <kdesktopfile.h>
#include <kconfiggroup.h>

#include <qstandardpaths.h>
#include <qdebug.h>

#include "kservicefactory.h"
#include "kservicetypefactory.h"

QDataStream &operator<<(QDataStream &s, const KService::ServiceTypeAndPreference &st)
{
    s << st.preference << st.serviceType;
    return s;
}
QDataStream &operator>>(QDataStream &s, KService::ServiceTypeAndPreference &st)
{
    s >> st.preference >> st.serviceType;
    return s;
}

void KServicePrivate::init( const KDesktopFile *config, KService* q )
{
    const QString entryPath = q->entryPath();
    bool absPath = !QDir::isRelativePath(entryPath);

    // TODO: it makes sense to have a KConstConfigGroup I guess
    const KConfigGroup desktopGroup = const_cast<KDesktopFile*>(config)->desktopGroup();
    QMap<QString, QString> entryMap = desktopGroup.entryMap();

    entryMap.remove(QLatin1String("Encoding")); // reserved as part of Desktop Entry Standard
    entryMap.remove(QLatin1String("Version"));  // reserved as part of Desktop Entry Standard

    q->setDeleted( desktopGroup.readEntry("Hidden", false) );
    entryMap.remove(QLatin1String("Hidden"));
    if ( q->isDeleted() ) {
        m_bValid = false;
        return;
    }

    m_strName = config->readName();
    entryMap.remove(QLatin1String("Name"));
    if ( m_strName.isEmpty() )
    {
        // Try to make up a name.
        m_strName = entryPath;
        int i = m_strName.lastIndexOf(QLatin1Char('/'));
        m_strName = m_strName.mid(i+1);
        i = m_strName.lastIndexOf(QLatin1Char('.'));
        if (i != -1)
            m_strName = m_strName.left(i);
    }

    m_strType = config->readType();
    entryMap.remove(QLatin1String("Type"));
    if ( m_strType.isEmpty() )
    {
        /*kWarning(servicesDebugArea()) << "The desktop entry file " << entryPath
          << " has no Type=... entry."
          << " It should be \"Application\" or \"Service\"";
          m_bValid = false;
          return;*/
        m_strType = QString::fromLatin1("Application");
    } else if (m_strType != QLatin1String("Application") && m_strType != QLatin1String("Service")) {
        qWarning() << "The desktop entry file " << entryPath
                       << " has Type=" << m_strType
                       << " instead of \"Application\" or \"Service\"";
        m_bValid = false;
        return;
    }

    // NOT readPathEntry, it is not XDG-compliant. Path entries written by
    // KDE4 will be still treated as such, though.
    m_strExec = desktopGroup.readEntry( "Exec", QString() );
    entryMap.remove(QLatin1String("Exec"));

    if (m_strType == QLatin1String("Application")) {
        // It's an application? Should have an Exec line then, otherwise we can't run it
        if (m_strExec.isEmpty()) {
            qWarning() << "The desktop entry file" << entryPath << "has Type=" << m_strType << "but no Exec line";
            m_bValid = false;
            return;
        }
    }

    // In case Try Exec is set, check if the application is available
    if (!config->tryExec()) {
        q->setDeleted( true );
        m_bValid = false;
        return;
    }

    const QStandardPaths::StandardLocation resource = config->resource();

    if ( (m_strType == QLatin1String("Application")) &&
         (resource != QStandardPaths::ApplicationsLocation) &&
         !absPath)
    {
        qWarning() << "The desktop entry file" << entryPath
            << "has Type=" << m_strType << "but is located under \"" << QStandardPaths::displayName(resource)
            << "\" instead of \"apps\"";
        m_bValid = false;
        return;
    }

    if ( (m_strType == QLatin1String("Service")) &&
         (resource != QStandardPaths::GenericDataLocation) &&
         !absPath)
    {
        qWarning() << "The desktop entry file" << entryPath
                       << "has Type=" << m_strType << "but is located under \"" << QStandardPaths::displayName(resource)
                       << "\" instead of \"shared data\"";
        m_bValid = false;
        return;
    }

    QString _name = entryPath;
    int pos = _name.lastIndexOf(QLatin1Char('/'));
    if (pos != -1)
        _name = _name.mid(pos+1);
    pos = _name.indexOf(QLatin1Char('.'));
    if (pos != -1)
        _name = _name.left(pos);

    m_strIcon = config->readIcon();
    entryMap.remove(QLatin1String("Icon"));
    m_bTerminal = desktopGroup.readEntry( "Terminal", false); // should be a property IMHO
    entryMap.remove(QLatin1String("Terminal"));
    m_strTerminalOptions = desktopGroup.readEntry( "TerminalOptions" ); // should be a property IMHO
    entryMap.remove(QLatin1String("TerminalOptions"));
    m_strPath = config->readPath();
    entryMap.remove(QLatin1String("Path"));
    m_strComment = config->readComment();
    entryMap.remove(QLatin1String("Comment"));
    m_strGenName = config->readGenericName();
    entryMap.remove(QLatin1String("GenericName"));
    QString _untranslatedGenericName = desktopGroup.readEntryUntranslated( "GenericName" );
    if (!_untranslatedGenericName.isEmpty())
        entryMap.insert(QLatin1String("UntranslatedGenericName"), _untranslatedGenericName);

    m_lstKeywords = desktopGroup.readXdgListEntry("Keywords", QStringList());
    entryMap.remove(QLatin1String("Keywords"));
    m_lstKeywords += desktopGroup.readEntry("X-KDE-Keywords", QStringList());
    entryMap.remove(QLatin1String("X-KDE-Keywords"));
    categories = desktopGroup.readXdgListEntry("Categories");
    entryMap.remove(QLatin1String("Categories"));
    // TODO KDE5: only care for X-KDE-Library in Type=Service desktop files
    // This will prevent people defining a part and an app in the same desktop file
    // which makes user-preference handling difficult.
    m_strLibrary = desktopGroup.readEntry( "X-KDE-Library" );
    entryMap.remove(QLatin1String("X-KDE-Library"));
    if (!m_strLibrary.isEmpty() && m_strType == QLatin1String("Application")) {
        qWarning() << "The desktop entry file" << entryPath << "has Type=" << m_strType
                       << "but also has a X-KDE-Library key. This works for now,"
                          " but makes user-preference handling difficult, so support for this might"
                          " be removed at some point. Consider splitting it into two desktop files.";
    }

    QStringList lstServiceTypes = desktopGroup.readEntry( "ServiceTypes", QStringList() );
    entryMap.remove(QLatin1String("ServiceTypes"));
    lstServiceTypes += desktopGroup.readEntry( "X-KDE-ServiceTypes", QStringList() );
    entryMap.remove(QLatin1String("X-KDE-ServiceTypes"));
    lstServiceTypes += desktopGroup.readXdgListEntry( "MimeType" );
    entryMap.remove(QLatin1String("MimeType"));

    if ( m_strType == QLatin1String("Application") && !lstServiceTypes.contains(QLatin1String("Application")) )
        // Applications implement the service type "Application" ;-)
        lstServiceTypes += QString::fromLatin1("Application");

    m_initialPreference = desktopGroup.readEntry( "InitialPreference", 1 );
    entryMap.remove(QLatin1String("InitialPreference"));

    // Assign the "initial preference" to each mimetype/servicetype
    // (and to set such preferences in memory from kbuildsycoca)
    m_serviceTypes.reserve(lstServiceTypes.size());
    QListIterator<QString> st_it(lstServiceTypes);
    while ( st_it.hasNext() ) {
        const QString st = st_it.next();
        if (st.isEmpty()) {
            qWarning() << "The desktop entry file" << entryPath << "has an empty mimetype!";
            continue;
        }
        int initialPreference = m_initialPreference;
        if ( st_it.hasNext() ) {
            // TODO better syntax - separate group with mimetype=number entries?
            bool isNumber;
            const int val = st_it.peekNext().toInt(&isNumber);
            if (isNumber) {
                initialPreference = val;
                st_it.next();
            }
        }
        m_serviceTypes.push_back(KService::ServiceTypeAndPreference(initialPreference, st));
    }

    if (entryMap.contains(QLatin1String("Actions"))) {
        parseActions(config, q);
    }

    QString dbusStartupType = desktopGroup.readEntry("X-DBUS-StartupType").toLower();
    entryMap.remove(QLatin1String("X-DBUS-StartupType"));
    if (dbusStartupType == QLatin1String("unique"))
        m_DBUSStartusType = KService::DBusUnique;
    else if (dbusStartupType == QLatin1String("multi"))
        m_DBUSStartusType = KService::DBusMulti;
    else
        m_DBUSStartusType = KService::DBusNone;

    m_strDesktopEntryName = _name.toLower();

    m_bAllowAsDefault = desktopGroup.readEntry("AllowDefault", true);
    entryMap.remove(QLatin1String("AllowDefault"));

    // allow plugin users to translate categories without needing a separate key
    QMap<QString,QString>::Iterator entry = entryMap.find(QString::fromLatin1("X-KDE-PluginInfo-Category"));
    if (entry != entryMap.end()) {
        const QString& key = entry.key();
        m_mapProps.insert(key, QVariant(desktopGroup.readEntryUntranslated(key)));
        m_mapProps.insert(key + QLatin1String("-Translated"), QVariant(*entry));
        entryMap.erase(entry);
    }

    // Store all additional entries in the property map.
    // A QMap<QString,QString> would be easier for this but we can't
    // break BC, so we have to store it in m_mapProps.
//  qDebug("Path = %s", entryPath.toLatin1().constData());
    QMap<QString,QString>::ConstIterator it = entryMap.constBegin();
    for( ; it != entryMap.constEnd();++it) {
        const QString key = it.key();
        // do not store other translations like Name[fr]; kbuildsycoca will rerun if we change languages anyway
        if (!key.contains(QLatin1Char('['))) {
            //qDebug() << "  Key =" << key << " Data =" << *it;
            m_mapProps.insert(key, QVariant(*it));
        }
    }
}

void KServicePrivate::parseActions(const KDesktopFile *config, KService* q)
{
    const QStringList keys = config->readActions();
    if (keys.isEmpty())
        return;

    QStringList::ConstIterator it = keys.begin();
    const QStringList::ConstIterator end = keys.end();
    for ( ; it != end; ++it ) {
        const QString group = *it;
        if (group == QLatin1String("_SEPARATOR_")) {
            m_actions.append(KServiceAction(group, QString(), QString(), QString(), false));
            continue;
        }

        if (config->hasActionGroup(group)) {
            const KConfigGroup cg = config->actionGroup(group);
            if ( !cg.hasKey( "Name" ) || !cg.hasKey( "Exec" ) ) {
                qWarning() << "The action" << group << "in the desktop file" << q->entryPath() << "has no Name or no Exec key";
            } else {
                m_actions.append(KServiceAction(group,
                                                cg.readEntry("Name"),
                                                cg.readEntry("Icon"),
                                                cg.readEntry("Exec"),
                                                cg.readEntry("NoDisplay", false)));
            }
        } else {
            qWarning() << "The desktop file" << q->entryPath() << "references the action" << group << "but doesn't define it";
        }
    }
}

void KServicePrivate::load(QDataStream& s)
{
    qint8 def, term;
    qint8 dst, initpref;
    QStringList dummyList; // KDE4: you can reuse this for another QStringList. KDE5: remove

    // WARNING: THIS NEEDS TO REMAIN COMPATIBLE WITH PREVIOUS KDE 4.x VERSIONS!
    // !! This data structure should remain binary compatible at all times !!
    // You may add new fields at the end. Make sure to update the version
    // number in ksycoca.h
    s >> m_strType >> m_strName >> m_strExec >> m_strIcon
      >> term >> m_strTerminalOptions
      >> m_strPath >> m_strComment >> dummyList >> def >> m_mapProps
      >> m_strLibrary
      >> dst
      >> m_strDesktopEntryName
      >> initpref
      >> m_lstKeywords >> m_strGenName
      >> categories >> menuId >> m_actions >> m_serviceTypes;

    m_bAllowAsDefault = (bool)def;
    m_bTerminal = (bool)term;
    m_DBUSStartusType = (KService::DBusStartupType) dst;
    m_initialPreference = initpref;

    m_bValid = true;
}

void KServicePrivate::save(QDataStream& s)
{
    KSycocaEntryPrivate::save( s );
    qint8 def = m_bAllowAsDefault, initpref = m_initialPreference;
    qint8 term = m_bTerminal;
    qint8 dst = (qint8) m_DBUSStartusType;

    // WARNING: THIS NEEDS TO REMAIN COMPATIBLE WITH PREVIOUS KDE 4.x VERSIONS!
    // !! This data structure should remain binary compatible at all times !!
    // You may add new fields at the end. Make sure to update the version
    // number in ksycoca.h
    s << m_strType << m_strName << m_strExec << m_strIcon
      << term << m_strTerminalOptions
      << m_strPath << m_strComment << QStringList() << def << m_mapProps
      << m_strLibrary
      << dst
      << m_strDesktopEntryName
      << initpref
      << m_lstKeywords << m_strGenName
      << categories << menuId << m_actions << m_serviceTypes;
}

////

KService::KService( const QString & _name, const QString &_exec, const QString &_icon)
    : KSycocaEntry(*new KServicePrivate(QString()))
{
    Q_D(KService);
    d->m_strType = QString::fromLatin1("Application");
    d->m_strName = _name;
    d->m_strExec = _exec;
    d->m_strIcon = _icon;
    d->m_bTerminal = false;
    d->m_bAllowAsDefault = true;
    d->m_initialPreference = 10;
}


KService::KService( const QString & _fullpath )
    : KSycocaEntry(*new KServicePrivate(_fullpath))
{
    Q_D(KService);

    KDesktopFile config( _fullpath );
    d->init(&config, this);
}

KService::KService(const KDesktopFile *config, const QString& entryPath)
    : KSycocaEntry(*new KServicePrivate(entryPath.isEmpty() ? config->fileName() : entryPath))
{
    Q_D(KService);

    d->init(config, this);
}

KService::KService( QDataStream& _str, int _offset )
    : KSycocaEntry(*new KServicePrivate(_str, _offset))
{
}

KService::~KService()
{
}

bool KService::hasServiceType( const QString& serviceType ) const
{
    Q_D(const KService);

    if (!d->m_bValid) return false; // (useless) safety test
    const KServiceType::Ptr ptr = KServiceType::serviceType( serviceType );
    if (!ptr)
        return false;
    const int serviceOffset = offset();
    // doesn't seem to work:
    //if ( serviceOffset == 0 )
    //    serviceOffset = serviceByStorageId( storageId() );
    if ( serviceOffset )
        return KServiceFactory::self()->hasOffer( ptr->offset(), ptr->serviceOffersOffset(), serviceOffset );

    // fall-back code for services that are NOT from ksycoca
    // For each service type we are associated with, if it doesn't
    // match then we try its parent service types.
    QVector<ServiceTypeAndPreference>::ConstIterator it = d->m_serviceTypes.begin();
    for( ; it != d->m_serviceTypes.end(); ++it ) {
        const QString& st = (*it).serviceType;
        //qDebug() << "    has " << (*it);
        if ( st == ptr->name() )
            return true;
        // also the case of parent servicetypes
        KServiceType::Ptr p = KServiceType::serviceType( st );
        if ( p && p->inherits( ptr->name() ) )
            return true;
    }
    return false;
}

#ifndef KDE_NO_DEPRECATED
bool KService::hasMimeType( const KServiceType* ptr ) const
{
    if (!ptr) return false;

    return hasMimeType(ptr->name());
}
#endif

bool KService::hasMimeType(const QString& mimeType) const
{
    Q_D(const KService);
    QMimeDatabase db;
    const QString mime = db.mimeTypeForName(mimeType).name();
    if (mime.isEmpty())
        return false;
    int serviceOffset = offset();
    if ( serviceOffset ) {
        KMimeTypeFactory *factory = KMimeTypeFactory::self();
        const int mimeOffset = factory->entryOffset(mime);
        const int serviceOffersOffset = factory->serviceOffersOffset(mime);
        if (serviceOffersOffset == -1)
            return false;
        return KServiceFactory::self()->hasOffer(mimeOffset, serviceOffersOffset, serviceOffset);
    }

    // fall-back code for services that are NOT from ksycoca
    QVector<ServiceTypeAndPreference>::ConstIterator it = d->m_serviceTypes.begin();
    for( ; it != d->m_serviceTypes.end(); ++it ) {
        const QString& st = (*it).serviceType;
        //qDebug() << "    has " << (*it);
        if ( st == mime )
            return true;
        // TODO: should we handle inherited mimetypes here?
        // KMimeType was in kio when this code was written, this is the only reason it's not done.
        // But this should matter only in a very rare case, since most code gets KServices from ksycoca.
        // Warning, change hasServiceType if you implement this here (and check kbuildservicefactory).
    }
    return false;
}

QVariant KServicePrivate::property( const QString& _name) const
{
    return property( _name, QVariant::Invalid);
}

// Return a string QVariant if string isn't null, and invalid variant otherwise
// (the variant must be invalid if the field isn't in the .desktop file)
// This allows trader queries like "exist Library" to work.
static QVariant makeStringVariant( const QString& string )
{
    // Using isEmpty here would be wrong.
    // Empty is "specified but empty", null is "not specified" (in the .desktop file)
    return string.isNull() ? QVariant() : QVariant( string );
}

QVariant KService::property( const QString& _name, QVariant::Type t ) const
{
    Q_D(const KService);
    return d->property(_name, t);
}

QVariant KServicePrivate::property( const QString& _name, QVariant::Type t ) const
{
    if ( _name == QLatin1String("Type") )
        return QVariant( m_strType ); // can't be null
    else if ( _name == QLatin1String("Name") )
        return QVariant( m_strName ); // can't be null
    else if ( _name == QLatin1String("Exec") )
        return makeStringVariant( m_strExec );
    else if ( _name == QLatin1String("Icon") )
        return makeStringVariant( m_strIcon );
    else if ( _name == QLatin1String("Terminal") )
        return QVariant( m_bTerminal );
    else if ( _name == QLatin1String("TerminalOptions") )
        return makeStringVariant( m_strTerminalOptions );
    else if ( _name == QLatin1String("Path") )
        return makeStringVariant( m_strPath );
    else if ( _name == QLatin1String("Comment") )
        return makeStringVariant( m_strComment );
    else if ( _name == QLatin1String("GenericName") )
        return makeStringVariant( m_strGenName );
    else if ( _name == QLatin1String("ServiceTypes") )
        return QVariant( serviceTypes() );
    else if ( _name == QLatin1String("AllowAsDefault") )
        return QVariant( m_bAllowAsDefault );
    else if ( _name == QLatin1String("InitialPreference") )
        return QVariant( m_initialPreference );
    else if ( _name == QLatin1String("Library") )
        return makeStringVariant( m_strLibrary );
    else if ( _name == QLatin1String("DesktopEntryPath") ) // can't be null
        return QVariant( path );
    else if ( _name == QLatin1String("DesktopEntryName"))
        return QVariant( m_strDesktopEntryName ); // can't be null
    else if ( _name == QLatin1String("Categories"))
        return QVariant( categories );
    else if ( _name == QLatin1String("Keywords"))
        return QVariant( m_lstKeywords );

    // Ok we need to convert the property from a QString to its real type.
    // Maybe the caller helped us.
    if (t == QVariant::Invalid)
    {
        // No luck, let's ask KServiceTypeFactory what the type of this property
        // is supposed to be.
        t = KServiceTypeFactory::self()->findPropertyTypeByName(_name);
        if (t == QVariant::Invalid)
        {
            qDebug() << "Request for unknown property '" << _name << "'\n";
            return QVariant(); // Unknown property: Invalid variant.
        }
    }

    QMap<QString,QVariant>::ConstIterator it = m_mapProps.find( _name );
    if ( (it == m_mapProps.end()) || (!it->isValid()))
    {
        //qDebug() << "Property not found " << _name;
        return QVariant(); // No property set.
    }

    switch(t)
    {
    case QVariant::String:
        return *it; // no conversion necessary
    default:
        // All others
        // For instance properties defined as StringList, like MimeTypes.
        // XXX This API is accessible only through a friend declaration.
        return KConfigGroup::convertToQVariant(_name.toUtf8().constData(), it->toString().toUtf8(), t);
    }
}

QStringList KServicePrivate::propertyNames() const
{
    QStringList res;

    QMap<QString,QVariant>::ConstIterator it = m_mapProps.begin();
    for( ; it != m_mapProps.end(); ++it )
        res.append( it.key() );

    res.append( QString::fromLatin1("Type") );
    res.append( QString::fromLatin1("Name") );
    res.append( QString::fromLatin1("Comment") );
    res.append( QString::fromLatin1("GenericName") );
    res.append( QString::fromLatin1("Icon") );
    res.append( QString::fromLatin1("Exec") );
    res.append( QString::fromLatin1("Terminal") );
    res.append( QString::fromLatin1("TerminalOptions") );
    res.append( QString::fromLatin1("Path") );
    res.append( QString::fromLatin1("ServiceTypes") );
    res.append( QString::fromLatin1("AllowAsDefault") );
    res.append( QString::fromLatin1("InitialPreference") );
    res.append( QString::fromLatin1("Library") );
    res.append( QString::fromLatin1("DesktopEntryPath") );
    res.append( QString::fromLatin1("DesktopEntryName") );
    res.append( QString::fromLatin1("Keywords") );
    res.append( QString::fromLatin1("Categories") );

    return res;
}

KService::List KService::allServices()
{
    return KServiceFactory::self()->allServices();
}

#ifndef KDE_NO_DEPRECATED
KService::Ptr KService::serviceByName( const QString& _name )
{
    return KServiceFactory::self()->findServiceByName( _name );
}
#endif

KService::Ptr KService::serviceByDesktopPath( const QString& _name )
{
    return KServiceFactory::self()->findServiceByDesktopPath( _name );
}

KService::Ptr KService::serviceByDesktopName( const QString& _name )
{
    // Prefer kde4-konsole over kde-konsole, if both are available
    QString name = _name.toLower();
    KService::Ptr s;
    if (!_name.startsWith(QLatin1String("kde5-")))
        s = KServiceFactory::self()->findServiceByDesktopName(QString::fromLatin1("kde5-") + name);
    if (!s && !_name.startsWith(QLatin1String("kde4-")))
        s = KServiceFactory::self()->findServiceByDesktopName(QString::fromLatin1("kde4-") + name);
    if (!s)
        s = KServiceFactory::self()->findServiceByDesktopName( name );

    return s;
}

KService::Ptr KService::serviceByMenuId( const QString& _name )
{
    return KServiceFactory::self()->findServiceByMenuId( _name );
}

KService::Ptr KService::serviceByStorageId( const QString& _storageId )
{
    KService::Ptr service = KService::serviceByMenuId( _storageId );
    if (service)
        return service;

    service = KService::serviceByDesktopPath(_storageId);
    if (service)
        return service;

    if (!QDir::isRelativePath(_storageId) && QFile::exists(_storageId))
        return KService::Ptr(new KService(_storageId));

    QString tmp = _storageId;
    tmp = tmp.mid(tmp.lastIndexOf(QLatin1Char('/'))+1); // Strip dir

    if (tmp.endsWith(QLatin1String(".desktop")))
        tmp.truncate(tmp.length()-8);

    if (tmp.endsWith(QLatin1String(".kdelnk")))
        tmp.truncate(tmp.length()-7);

    service = KService::serviceByDesktopName(tmp);

    return service;
}

bool KService::substituteUid() const {
    QVariant v = property(QLatin1String("X-KDE-SubstituteUID"), QVariant::Bool);
    return v.isValid() && v.toBool();
}

QString KService::username() const {
    // See also KDesktopFile::tryExec()
    QString user;
    QVariant v = property(QLatin1String("X-KDE-Username"), QVariant::String);
    user = v.isValid() ? v.toString() : QString();
    if (user.isEmpty())
        user = QString::fromLocal8Bit(qgetenv("ADMIN_ACCOUNT"));
    if (user.isEmpty())
        user = QString::fromLatin1("root");
    return user;
}

bool KService::showInKDE() const
{
    Q_D(const KService);

    QMap<QString,QVariant>::ConstIterator it = d->m_mapProps.find( QString::fromLatin1("OnlyShowIn") );
    if ( (it != d->m_mapProps.end()) && (it->isValid()))
    {
        const QStringList aList = it->toString().split(QLatin1Char(';'));
        if (!aList.contains(QString::fromLatin1("KDE")))
            return false;
    }

    it = d->m_mapProps.find( QString::fromLatin1("NotShowIn") );
    if ( (it != d->m_mapProps.end()) && (it->isValid()))
    {
        const QStringList aList = it->toString().split(QLatin1Char(';'));
        if (aList.contains(QString::fromLatin1("KDE")))
            return false;
    }
    return true;
}

bool KService::noDisplay() const {
    if ( qvariant_cast<bool>(property(QString::fromLatin1("NoDisplay"), QVariant::Bool)) )
        return true;

    if (!showInKDE())
        return true;

    if (!KAuthorized::authorizeControlModule( storageId() ) )
        return true;

    return false;
}

QString KService::untranslatedGenericName() const {
    QVariant v = property(QString::fromLatin1("UntranslatedGenericName"), QVariant::String);
    return v.isValid() ? v.toString() : QString();
}

QString KService::parentApp() const {
    Q_D(const KService);
    QMap<QString,QVariant>::ConstIterator it = d->m_mapProps.find(QLatin1String("X-KDE-ParentApp"));
    if ( (it == d->m_mapProps.end()) || (!it->isValid()))
    {
        return QString();
    }

    return it->toString();
}

QString KService::pluginKeyword() const
{
    Q_D(const KService);
    QMap<QString,QVariant>::ConstIterator it = d->m_mapProps.find(QString::fromLatin1("X-KDE-PluginKeyword"));
    if ((it == d->m_mapProps.end()) || (!it->isValid())) {
        return QString();
    }

    return it->toString();
}

QString KService::docPath() const
{
    Q_D(const KService);
    QMap<QString,QVariant>::ConstIterator it = d->m_mapProps.find(QLatin1String("X-DocPath"));
    if ((it == d->m_mapProps.end()) || (!it->isValid())) {
        it = d->m_mapProps.find(QString::fromLatin1("DocPath"));
        if ((it == d->m_mapProps.end()) || (!it->isValid())) {
            return QString();
        }
    }

    return it->toString();
}

bool KService::allowMultipleFiles() const {
    Q_D(const KService);
    // Can we pass multiple files on the command line or do we have to start the application for every single file ?
    return (d->m_strExec.contains( QLatin1String("%F") ) || d->m_strExec.contains( QLatin1String("%U") ) ||
            d->m_strExec.contains( QLatin1String("%N") ) || d->m_strExec.contains( QLatin1String("%D") ));
}

QStringList KService::categories() const
{
    Q_D(const KService);
    return d->categories;
}

QString KService::menuId() const
{
    Q_D(const KService);
    return d->menuId;
}

void KService::setMenuId(const QString &_menuId)
{
    Q_D(KService);
    d->menuId = _menuId;
}

QString KService::storageId() const
{
    Q_D(const KService);
    return d->storageId();
}

// not sure this is still used anywhere...
QString KService::locateLocal() const
{
    Q_D(const KService);
    if (d->menuId.isEmpty() || entryPath().startsWith(QLatin1String(".hidden")) ||
        (QDir::isRelativePath(entryPath()) && d->categories.isEmpty()))
        return KDesktopFile::locateLocal(entryPath());

    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/applications/") + d->menuId;
}

QString KService::newServicePath(bool showInMenu, const QString &suggestedName,
                                 QString *menuId, const QStringList *reservedMenuIds)
{
    Q_UNUSED(showInMenu); // TODO KDE5: remove argument

    QString base = suggestedName;
    QString result;
    for(int i = 1; true; i++)
    {
        if (i == 1)
            result = base + QString::fromLatin1(".desktop");
        else
            result = base + QString::fromLatin1("-%1.desktop").arg(i);

        if (reservedMenuIds && reservedMenuIds->contains(result))
            continue;

        // Lookup service by menu-id
        KService::Ptr s = serviceByMenuId(result);
        if (s)
            continue;

        if (!QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("applications/") + result).isEmpty())
            continue;

        break;
    }
    if (menuId)
        *menuId = result;

    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/applications/") + result;
}

bool KService::isApplication() const
{
    Q_D(const KService);
    return d->m_strType == QLatin1String("Application");
}

#ifndef KDE_NO_DEPRECATED
QString KService::type() const
{
    Q_D(const KService);
    return d->m_strType;
}
#endif

QString KService::exec() const
{
    Q_D(const KService);
    if (d->m_strType == QLatin1String("Application") && d->m_strExec.isEmpty())
    {
        qWarning() << "The desktop entry file " << entryPath()
                       << " has Type=" << d->m_strType << " but has no Exec field.";
    }
    return d->m_strExec;
}

QString KService::library() const
{
    Q_D(const KService);
    return d->m_strLibrary;
}

QString KService::icon() const
{
    Q_D(const KService);
    return d->m_strIcon;
}

QString KService::terminalOptions() const
{
    Q_D(const KService);
    return d->m_strTerminalOptions;
}

bool KService::terminal() const
{
    Q_D(const KService);
    return d->m_bTerminal;
}

// KDE5: remove and port code to entryPath?
#ifndef KDE_NO_DEPRECATED
QString KService::desktopEntryPath() const
{
    return entryPath();
}
#endif

QString KService::desktopEntryName() const
{
    Q_D(const KService);
    return d->m_strDesktopEntryName;
}

KService::DBusStartupType KService::dbusStartupType() const
{
    Q_D(const KService);
    return d->m_DBUSStartusType;
}

QString KService::path() const
{
    Q_D(const KService);
    return d->m_strPath;
}

QString KService::comment() const
{
    Q_D(const KService);
    return d->m_strComment;
}

QString KService::genericName() const
{
    Q_D(const KService);
    return d->m_strGenName;
}

QStringList KService::keywords() const
{
    Q_D(const KService);
    return d->m_lstKeywords;
}

QStringList KServicePrivate::serviceTypes() const
{
    QStringList ret;
    QVector<KService::ServiceTypeAndPreference>::const_iterator it = m_serviceTypes.begin();
    for ( ; it < m_serviceTypes.end(); ++it ) {
        Q_ASSERT(!(*it).serviceType.isEmpty());
        ret.append((*it).serviceType);
    }
    return ret;
}

QStringList KService::serviceTypes() const
{
    Q_D(const KService);
    return d->serviceTypes();
}

QStringList KService::mimeTypes() const
{
    Q_D(const KService);
    QStringList ret;
    QMimeDatabase db;
    QVector<KService::ServiceTypeAndPreference>::const_iterator it = d->m_serviceTypes.begin();
    for ( ; it < d->m_serviceTypes.end(); ++it ) {
        const QString sv = (*it).serviceType;
        if (db.mimeTypeForName(sv).isValid()) // keep only mimetypes, filter out servicetypes
            ret.append(sv);
    }
    return ret;
}

bool KService::allowAsDefault() const
{
    Q_D(const KService);
    return d->m_bAllowAsDefault;
}

int KService::initialPreference() const
{
    Q_D(const KService);
    return d->m_initialPreference;
}

void KService::setTerminal(bool b)
{
    Q_D(KService);
    d->m_bTerminal = b;
}

void KService::setTerminalOptions(const QString &options)
{
    Q_D(KService);
    d->m_strTerminalOptions = options;
}

void KService::setExec(const QString& exec)
{
  Q_D(KService);

  if (!exec.isEmpty()) {
      d->m_strExec = exec;
      d->path.clear();
  }
}

QVector<KService::ServiceTypeAndPreference> & KService::_k_accessServiceTypes()
{
    Q_D(KService);
    return d->m_serviceTypes;
}

QList<KServiceAction> KService::actions() const
{
    Q_D(const KService);
    return d->m_actions;
}
