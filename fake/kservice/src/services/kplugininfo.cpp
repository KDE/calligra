/*  This file is part of the KDE project
    Copyright (C) 2003,2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "kplugininfo.h"
#include <kservicetypetrader.h>
#include <kdesktopfile.h>
#include <kservice.h>
#include <QList>
#include <QDebug>
#include <QDirIterator>
#include <kconfiggroup.h>
#include <qstandardpaths.h>
#include <qdebug.h>

//#ifndef NDEBUG
#define KPLUGININFO_ISVALID_ASSERTION \
    do { \
        if (!d) { \
            qFatal("Accessed invalid KPluginInfo object"); \
        } \
    } while (false)
//#else
//#define KPLUGININFO_ISVALID_ASSERTION
//#endif

class KPluginInfoPrivate : public QSharedData
{
    public:
        KPluginInfoPrivate()
            : hidden( false )
            , enabledbydefault( false )
            , pluginenabled( false )
            , kcmservicesCached( false )
            {}

        QString entryPath; // the filename of the file containing all the info
        QString name;
        QString comment;
        QString icon;
        QString author;
        QString email;
        QString pluginName; // the name attribute in the .rc file
        QString version;
        QString website; // URL to the website of the plugin/author
        QString category;
        QString license;
        QStringList dependencies;

        bool hidden : 1;
        bool enabledbydefault : 1;
        bool pluginenabled : 1;
        mutable bool kcmservicesCached : 1;

        KConfigGroup config;
        KService::Ptr service;
        mutable QList<KService::Ptr> kcmservices;
};

KPluginInfo::KPluginInfo(const QString & filename /*, QStandardPaths::StandardLocation resource*/)
: d( new KPluginInfoPrivate )
{
    KDesktopFile file(/*resource,*/ filename);

    d->entryPath = filename;

    KConfigGroup cg = file.desktopGroup();
    d->hidden = cg.readEntry("Hidden", false);
    if( d->hidden )
        return;

    d->name = file.readName();
    d->comment = file.readComment();
    d->icon = cg.readEntryUntranslated( "Icon" );
    d->author = cg.readEntryUntranslated( "X-KDE-PluginInfo-Author" );
    d->email = cg.readEntryUntranslated( "X-KDE-PluginInfo-Email" );
    d->pluginName = cg.readEntryUntranslated( "X-KDE-PluginInfo-Name" );
    d->version = cg.readEntryUntranslated( "X-KDE-PluginInfo-Version" );
    d->website = cg.readEntryUntranslated( "X-KDE-PluginInfo-Website" );
    d->category = cg.readEntryUntranslated( "X-KDE-PluginInfo-Category" );
    d->license = cg.readEntryUntranslated( "X-KDE-PluginInfo-License" );
    d->dependencies = cg.readEntry( "X-KDE-PluginInfo-Depends", QStringList() );
    d->enabledbydefault = cg.readEntry(
            "X-KDE-PluginInfo-EnabledByDefault", false);
}

KPluginInfo::KPluginInfo(const QVariantList &args)
: d( new KPluginInfoPrivate )
{
    QVariantMap meta;
    static const QString metaData = QStringLiteral("MetaData");

    foreach (const QVariant &v, args) {
        if (v.canConvert<QVariantMap>()) {
            const QVariantMap &m = v.toMap();
            const QVariant &_metadata = m.value(metaData);
            if (_metadata.canConvert<QVariantMap>()) {
                meta = _metadata.value<QVariantMap>();
                break;
            }
        }
    }

    d->hidden = meta.value(QStringLiteral("Hidden")).toBool();
    if (d->hidden) {
        return;
    }

    d->name = meta.value(QStringLiteral("Name")).toString();
    d->comment = meta.value(QStringLiteral("Comment")).toString();
    d->icon = meta.value(QStringLiteral("Icon")).toString();
    d->author = meta.value(QStringLiteral("X-KDE-PluginInfo-Author")).toString();
    d->email = meta.value(QStringLiteral("X-KDE-PluginInfo-Email")).toString();
    d->pluginName = meta.value(QStringLiteral("X-KDE-PluginInfo-Name")).toString();
    d->version = meta.value(QStringLiteral("X-KDE-PluginInfo-Version")).toString();
    d->website = meta.value(QStringLiteral("X-KDE-PluginInfo-Website")).toString();
    d->category = meta.value(QStringLiteral("X-KDE-PluginInfo-Category")).toString();
    d->license = meta.value(QStringLiteral("X-KDE-PluginInfo-License")).toString();
    d->dependencies = meta.value(QStringLiteral("X-KDE-PluginInfo-Depends")).toStringList();
    d->enabledbydefault = meta.value(QStringLiteral("X-KDE-PluginInfo-EnabledByDefault")).toBool();
}

#ifndef KDE_NO_DEPRECATED
KPluginInfo::KPluginInfo( const KService::Ptr service )
: d( new KPluginInfoPrivate )
{
    if (!service) {
        d = 0; // isValid() == false
        return;
    }
    d->service = service;
    d->entryPath = service->entryPath();

    if ( service->isDeleted() )
    {
        d->hidden = true;
        return;
    }

    d->name = service->name();
    d->comment = service->comment();
    d->icon = service->icon();
    d->author = service->property( QLatin1String("X-KDE-PluginInfo-Author") ).toString();
    d->email = service->property( QLatin1String("X-KDE-PluginInfo-Email") ).toString();
    d->pluginName = service->property( QLatin1String("X-KDE-PluginInfo-Name") ).toString();
    d->version = service->property( QLatin1String("X-KDE-PluginInfo-Version") ).toString();
    d->website = service->property( QLatin1String("X-KDE-PluginInfo-Website") ).toString();
    d->category = service->property( QLatin1String("X-KDE-PluginInfo-Category") ).toString();
    d->license = service->property( QLatin1String("X-KDE-PluginInfo-License") ).toString();
    d->dependencies =
        service->property( QLatin1String("X-KDE-PluginInfo-Depends") ).toStringList();
    QVariant tmp = service->property( QLatin1String("X-KDE-PluginInfo-EnabledByDefault") );
    d->enabledbydefault = tmp.isValid() ? tmp.toBool() : false;
}
#endif

KPluginInfo::KPluginInfo()
    : d(0) // isValid() == false
{
}

bool KPluginInfo::isValid() const
{
    return d.data() != 0;
}

KPluginInfo::KPluginInfo(const KPluginInfo &rhs)
    : d(rhs.d)
{
}

KPluginInfo &KPluginInfo::operator=(const KPluginInfo &rhs)
{
    d = rhs.d;
    return *this;
}

bool KPluginInfo::operator==(const KPluginInfo &rhs) const
{
    return d == rhs.d;
}

bool KPluginInfo::operator!=(const KPluginInfo &rhs) const
{
    return d != rhs.d;
}

bool KPluginInfo::operator<(const KPluginInfo &rhs) const
{
    if (category() < rhs.category()) {
        return true;
    }
    if (category() == rhs.category()) {
        return name() < rhs.name();
    }
    return false;
}

bool KPluginInfo::operator>(const KPluginInfo &rhs) const
{
    if (category() > rhs.category()) {
        return true;
    }
    if (category() == rhs.category()) {
        return name() > rhs.name();
    }
    return false;
}

KPluginInfo::~KPluginInfo()
{
}

QList<KPluginInfo> KPluginInfo::fromServices(const KService::List &services, const KConfigGroup &config)
{
    QList<KPluginInfo> infolist;
    for( KService::List::ConstIterator it = services.begin();
            it != services.end(); ++it )
    {
        KPluginInfo info(*it);
        info.setConfig(config);
        infolist += info;
    }
    return infolist;
}

QList<KPluginInfo> KPluginInfo::fromFiles(const QStringList &files, const KConfigGroup &config)
{
    QList<KPluginInfo> infolist;
    for( QStringList::ConstIterator it = files.begin(); it != files.end(); ++it )
    {
        KPluginInfo info(*it);
        info.setConfig(config);
        infolist += info;
    }
    return infolist;
}

QList<KPluginInfo> KPluginInfo::fromKPartsInstanceName(const QString &name, const KConfigGroup &config)
{
    QStringList files;
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, name + QLatin1String("/kpartplugins"), QStandardPaths::LocateDirectory);
    Q_FOREACH(const QString& dir, dirs) {
        QDirIterator it(dir, QStringList() << QLatin1String("*.desktop"));
        while (it.hasNext()) {
            files.append(it.next());
        }
    }
    return fromFiles(files, config);
}

bool KPluginInfo::isHidden() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->hidden;
}

void KPluginInfo::setPluginEnabled( bool enabled )
{
    KPLUGININFO_ISVALID_ASSERTION;
    //qDebug() << Q_FUNC_INFO;
    d->pluginenabled = enabled;
}

bool KPluginInfo::isPluginEnabled() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    //qDebug() << Q_FUNC_INFO;
    return d->pluginenabled;
}

bool KPluginInfo::isPluginEnabledByDefault() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    //qDebug() << Q_FUNC_INFO;
    return d->enabledbydefault;
}

QString KPluginInfo::name() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->name;
}

QString KPluginInfo::comment() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->comment;
}

QString KPluginInfo::icon() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->icon;
}

QString KPluginInfo::entryPath() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->entryPath;
}

QString KPluginInfo::author() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->author;
}

QString KPluginInfo::email() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->email;
}

QString KPluginInfo::category() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->category;
}

QString KPluginInfo::pluginName() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->pluginName;
}

QString KPluginInfo::version() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->version;
}

QString KPluginInfo::website() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->website;
}

QString KPluginInfo::license() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->license;
}

#if 0
KAboutLicense KPluginInfo::fullLicense() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return KAboutLicense::byKeyword(d->license);
}
#endif

QStringList KPluginInfo::dependencies() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->dependencies;
}

KService::Ptr KPluginInfo::service() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->service;
}

QList<KService::Ptr> KPluginInfo::kcmServices() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    if ( !d->kcmservicesCached )
    {
        d->kcmservices = KServiceTypeTrader::self()->query( QLatin1String("KCModule"), QLatin1Char('\'') + d->pluginName +
            QString::fromLatin1("' in [X-KDE-ParentComponents]") );
        //qDebug() << "found" << d->kcmservices.count() << "offers for" << d->pluginName;

        d->kcmservicesCached = true;
    }

    return d->kcmservices;
}

void KPluginInfo::setConfig(const KConfigGroup &config)
{
    KPLUGININFO_ISVALID_ASSERTION;
    d->config = config;
}

KConfigGroup KPluginInfo::config() const
{
    KPLUGININFO_ISVALID_ASSERTION;
    return d->config;
}

QVariant KPluginInfo::property( const QString & key ) const
{
    KPLUGININFO_ISVALID_ASSERTION;
    if( d->service )
        return d->service->property( key );
    else
        return QVariant();
}

void KPluginInfo::save(KConfigGroup config)
{
    KPLUGININFO_ISVALID_ASSERTION;
    //qDebug() << Q_FUNC_INFO;
    if (config.isValid()) {
        config.writeEntry(d->pluginName + QString::fromLatin1("Enabled"), isPluginEnabled());
    } else {
        if (!d->config.isValid()) {
            qWarning() << "no KConfigGroup, cannot save";
            return;
        }
        d->config.writeEntry(d->pluginName + QString::fromLatin1("Enabled"), isPluginEnabled());
    }
}

void KPluginInfo::load(const KConfigGroup &config)
{
    KPLUGININFO_ISVALID_ASSERTION;
    //qDebug() << Q_FUNC_INFO;
    if (config.isValid()) {
        setPluginEnabled(config.readEntry(d->pluginName + QString::fromLatin1("Enabled"), isPluginEnabledByDefault()));
    } else {
        if (!d->config.isValid()) {
            qWarning() << "no KConfigGroup, cannot load";
            return;
        }
        setPluginEnabled(d->config.readEntry(d->pluginName + QString::fromLatin1("Enabled"), isPluginEnabledByDefault()));
    }
}

void KPluginInfo::defaults()
{
    //qDebug() << Q_FUNC_INFO;
    setPluginEnabled( isPluginEnabledByDefault() );
}

uint qHash(const KPluginInfo &p)
{
    return qHash(reinterpret_cast<quint64>(p.d.data()));
}

#undef KPLUGININFO_ISVALID_ASSERTION

// vim: sw=4 sts=4 et
