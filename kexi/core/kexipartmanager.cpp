/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#include "kexipartmanager.h"
#include "kexipart.h"
#include "kexiinternalpart.h"
#include "kexipartinfo.h"
//! @todo KEXI3 #include "kexistaticpart.h"
#include "kexi_version.h"
#include "KexiJsonTrader.h"

#include <KDbConnection>
#include <KDbCursor>

#include <KLocalizedString>
#include <KPluginFactory>
#include <KConfigGroup>
#include <KSharedConfig>

#include <QApplication>
#include <QDebug>
#include <QGlobalStatic>


using namespace KexiPart;

typedef QHash<QString, KexiInternalPart*> KexiInternalPartDict;

Q_GLOBAL_STATIC_WITH_ARGS(KexiJsonTrader, KexiPartTrader_instance, ("kexi"))

class Manager::Private
{
public:
    explicit Private(Manager *manager_);
    ~Private();

    Manager *manager;
    PartDict parts;
    KexiInternalPartDict internalParts;
    PartInfoList partlist;
    PartInfoDict partsByPluginId;
    bool lookupDone;
    bool lookupResult;
};

Manager::Private::Private(Manager *manager_)
    : manager(manager_)
    , lookupDone(false)
    , lookupResult(false)
{
}

Manager::Private::~Private()
{
    qDeleteAll(partlist);
    partlist.clear();
}

//---

Manager::Manager(QObject *parent)
    : QObject(parent), KDbResultable(), d(new Private(this))
{
}

Manager::~Manager()
{
    delete d;
}

template <typename PartClass>
PartClass* Manager::part(Info *info, QHash<QString, PartClass*> *partDict)
{
    if (!info) {
        return 0;
    }
    clearResult();
    if (!lookup()) {
        return 0;
    }
    if (!info->isValid()) {
        m_result = KDbResult(info->errorMessage());
        return 0;
    }
    PartClass *p = partDict->value(info->pluginId());
    if (p) {
        return p;
    }

    // actual loading
    KPluginFactory *factory = qobject_cast<KPluginFactory*>(info->instantiate());
    if (!factory) {
        m_result = KDbResult(ERR_OBJECT_NOT_FOUND,
                             xi18nc("@info", "Could not load Kexi plugin file \"%1\".",
                                    info->fileName()));
        info->setErrorMessage(m_result.message());
        qWarning() << m_result.message();
        return 0;
    }
    p = factory->create<PartClass>(this);
    if (!p) {
        m_result = KDbResult(ERR_CANNOT_LOAD_OBJECT,
                             i18n("Could not open Kexi plugin \"%1\".").arg(info->fileName()));
        qWarning() << m_result.message();
        return 0;
    }
    p->setInfo(info);
    p->setObjectName(QString("%1 plugin").arg(info->id()));
    partDict->insert(info->pluginId(), p);
    return p;
}

//! @return a string list @a list with removed whitespace from the beginning and end of each string.
//! Empty strings are also removed.
static QStringList cleanupStringList(const QStringList &list)
{
    QStringList result;
    foreach(const QString &item, list) {
        QString cleanedItem = item.trimmed();
        if (!cleanedItem.isEmpty()) {
            result.append(cleanedItem);
        }
    }
    return result;
}

bool Manager::lookup()
{
//! @todo Allow refreshing!!!! (will need calling removeClient() by Part objects)
    if (d->lookupDone) {
        return d->lookupResult;
    }
    d->lookupDone = true;
    d->lookupResult = false;
    d->partlist.clear();
    d->partsByPluginId.clear();
    d->parts.clear();

    // load visual order of plugins
    KConfigGroup cg(KSharedConfig::openConfig()->group("Parts"));
    const QStringList orderedPluginIds = cleanupStringList(
        cg.readEntry("Order", "org.kexi-project.table,"
                              "org.kexi-project.query,"
                              "org.kexi-project.form,"
                              "org.kexi-project.report,"
                              "org.kexi-project.macro,"
                              "org.kexi-project.script").split(','));
    QVector<Info*> orderedInfos(orderedPluginIds.count());
    QStringList serviceTypes;
    serviceTypes << "Kexi/Viewer" << "Kexi/Designer" << "Kexi/Editor"
                 << "Kexi/ModalDialog";
    const QList<QPluginLoader*> offers = KexiPartTrader_instance->query(serviceTypes);
    foreach(QPluginLoader *loader, offers) {
        Info *info = new Info(*loader);
        if (info->id().isEmpty()) {
            qWarning() << "No plugin ID (X-KDE-PluginInfo-Name) specified for Kexi Part"
                       << info->fileName() << "-- skipping!";
            continue;
        }
        // check version
        if (info->majorVersion() != KEXI_PART_VERSION) {
            qWarning() << "Kexi plugin" << info->id() << "has version (X-KDE-PluginInfo-Version)"
                       << info->majorVersion() << "but required version is" << KEXI_PART_VERSION
                       << "-- skipping!";
            continue;
        }
        // skip experimental types
        if (   (!Kexi::tempShowMacros() && info->id() == "org.kexi-project.macro")
            || (!Kexi::tempShowScripts() && info->id() == "org.kexi-project.script")
           )
        {
            continue;
        }
        // skip duplicates
        if (d->partsByPluginId.contains(info->id())) {
            qWarning() << "More than one Kexi plugin with ID"
                       << info->id() << info->fileName() << "-- skipping this one";
            continue;
        }
        // find correct place for plugins visible in Navigator
        if (info->isVisibleInNavigator()) {
            const int index = orderedPluginIds.indexOf(info->id());
            if (index != -1) {
                orderedInfos[index] = info;
            }
            else {
                orderedInfos.append(info);
            }
            // append later when we know order
        }
        else {
            // append now
            d->partlist.append(info);
        }
        d->partsByPluginId.insert(info->pluginId(), info);
    }

    // fill the final list using computed order
    for (int i = 0; i < orderedInfos.size(); i++) {
        Info *info = orderedInfos[i];
        if (!info) {
            continue;
        }
        //qDebug() << "adding Kexi part info" << info->pluginId();
        d->partlist.insert(i, info);
    }
    // now the d->partlist is: [ordered plugins visible in Navigator] [other plugins in unspecified order]
    d->lookupResult = true;
    return true;
}

Part* Manager::part(Info *info)
{
    Part *p = part<Part>(info, &d->parts);
    if (p) {
        emit partLoaded(p);
    }
    return p;
}

static QString realPluginId(const QString &pluginId)
{
    if (pluginId.contains('.')) {
        return pluginId;
    }
    else {
        // not like "org.kexi-project.table" - construct
        return QString::fromLatin1("org.kexi-project.")
            + QString(pluginId).remove("kexi/");
    }
}

Part* Manager::partForPluginId(const QString &pluginId)
{
    Info* info = infoForPluginId(pluginId);
    return part(info);
}

Info* Manager::infoForPluginId(const QString &pluginId)
{
    if (!lookup())
        return 0;
    const QString realId = realPluginId(pluginId);
    Info *i = realId.isEmpty() ? 0 : d->partsByPluginId.value(realId);
    if (i)
        return i;
    m_result = KDbResult(xi18nc("@info", "No plugin for ID <resource>%1</resource>", realId));
    return 0;
}

/*! @todo KEXI3
void Manager::insertStaticPart(StaticPart* part)
{
    if (!part)
        return;
    if (!lookup())
        return;
    d->partlist.append(part->info());
    if (!part->info()->pluginId().isEmpty())
        d->partsByPluginId.insert(part->info()->pluginId(), part->info());
    d->parts.insert(part->info()->pluginId(), part);
}
*/

KexiInternalPart* Manager::internalPartForPluginId(const QString& pluginId)
{
    Info* info = infoForPluginId(pluginId);
    return info ? part<KexiInternalPart>(info, &d->internalParts) : 0;
}

PartInfoList* Manager::infoList()
{
    if (!lookup()) {
        return 0;
    }
    return &d->partlist;
}
