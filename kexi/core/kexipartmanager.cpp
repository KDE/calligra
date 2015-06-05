/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#include <QApplication>
#include <QDebug>

#include <kservicetype.h>
#include <kservice.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <KLocalizedString>

#include <KoServiceLocator.h>

#include "kexipart.h"
#include "kexiinternalpart.h"
#include "kexipartinfo.h"
#include "kexistaticpart.h"
#include "kexi_version.h"

#include <db/connection.h>
#include <db/cursor.h>
#include <db/pluginloader.h>
#include <KSharedConfig>

using namespace KexiPart;

typedef QHash<QString, KexiInternalPart*> KexiInternalPartDict;

class Manager::Private
{
public:
    explicit Private(Manager *manager_);
    ~Private();

    template <typename PartClass>
    PartClass* part(Info *i, QHash<QString, PartClass*> &partDict);

    Manager *manager;
    PartDict parts;
    KexiInternalPartDict internalParts;
    PartInfoList partlist;
    PartInfoDict partsByClass;
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

template <typename PartClass>
PartClass* Manager::Private::part(Info *i, QHash<QString, PartClass*> &partDict)
{
    manager->clearError();
    if (!i)
        return 0;
    if (!manager->lookup())
        return 0;

    if (i->isBroken()) {
        manager->setError(i->errorMessage());
        return 0;
    }

    PartClass *p = partDict.value(i->partClass());
    if (!p && !i->ptr().isNull()) {
        KexiPluginLoader loader(i->ptr(), "X-Kexi-Class");
        if (loader.majorVersion() != KEXI_PART_VERSION) {
            i->setBroken(true,
                xi18nc("@info", "Incompatible plugin <resource>%1</resource> version: "
                      "found version %2, expected version %3.",
                      i->objectName(),
                      loader.majorVersion(),
                      KEXI_PART_VERSION));
            manager->setError(i->errorMessage());
            return 0;
        }
        p = loader.createPlugin<PartClass>(manager);
        if (!p) {
            qWarning() << "failed";
            i->setBroken(true, xi18nc("@info", "Error while loading plugin <resource>%1</resource>",
                                     i->objectName()));
            manager->setError(i->errorMessage());
            return 0;
        }
        p->setInfo(i);
        p->setObjectName(QString("%1 plugin").arg(i->objectName()));
        partDict.insert(i->partClass(), p);
    }
    return p;
}

//---

Manager::Manager(QObject *parent)
    : QObject(parent), d(new Private(this))
{
}

Manager::~Manager()
{
    delete d;
}

static QString appIncorrectlyInstalledMessage()
{
    return xi18nc("@info", "<application>%1</application> could have been incorrectly installed or started. The application will be closed.",
                 KGlobal::mainComponent().aboutData()->programName());
}

bool Manager::lookup()
{
//! @todo Allow refreshing!!!! (will need calling removeClient() by Part objects)
    if (d->lookupDone)
        return d->lookupResult;
    d->lookupDone = true;
    d->lookupResult = false;
    d->partlist.clear();
    d->partsByClass.clear();
    d->parts.clear();

    if (!KServiceType::serviceType("Kexi/Handler")) {
        qWarning() << "No 'Kexi/Handler' service type installed! Aborting.";
        m_serverErrorMsg = xi18nc("@info", "No <resource>%1</resource> service type installed.",
                                 QLatin1String("Kexi/Handler"));
        setError(appIncorrectlyInstalledMessage());
        return false;
    }

    KConfigGroup cg(KSharedConfig::openConfig()->group("Parts"));
    if (qApp && !cg.hasKey("Order")) {
        m_serverErrorMsg = xi18nc("@info",
                                 "Missing or invalid default application configuration. No <resource>%1</resource> key.",
                                 QLatin1String("Parts/Order"));
        setError(appIncorrectlyInstalledMessage());
        return false;
    }
    const QStringList sl_order = cg.readEntry("Order").split(',');  //we'll set parts in defined order
    QVector<KService::Ptr> ordered(sl_order.count());

    //compute order
    const KService::List tlist = KoServiceLocator::instance()->entries("Kexi/Handler");
    foreach(KService::Ptr ptr, tlist) {
        // check type name (class is optional)
        QString partClass = ptr->property("X-Kexi-Class", QVariant::String).toString();
        //QString partName = ptr->property("X-Kexi-TypeName", QVariant::String).toString();
        //qDebug() << partName << partClass;
        if (partClass.isEmpty()) {
            qWarning() << "No class name (X-Kexi-Class) specified for Kexi Part" << ptr->desktopEntryName() << ptr->entryPath() << "-- skipping!";
            continue;
        }
        if (   (!Kexi::tempShowMacros() && partClass == "org.kexi-project.macro")
            || (!Kexi::tempShowScripts() && partClass == "org.kexi-project.script")
           )
        {
            continue;
        }
        // check version
        bool ok;
        const int ver = ptr->property("X-Kexi-PartVersion").toInt(&ok);
        if (!ok) {
            qWarning() << "No version (X-Kexi-PartVersion) specified for Kexi Part" << ptr->desktopEntryName() << "-- skipping!";
            continue;
        }
        if (ver != KEXI_PART_VERSION) {
            qWarning() << "kexi part" << partClass << "has version (X-Kexi-PartVersion)"
                       << ver << "but required version is" << KEXI_PART_VERSION << "-- skipping!";
            continue;
        }
        const int idx = sl_order.indexOf(partClass);
        if (idx != -1) {
            ordered[idx] = ptr;
        }
        else {
            ordered.append(ptr);
        }
    }
    //fill final list using computed order
    for (int i = 0; i < ordered.size(); i++) {
        KService::Ptr ptr = ordered[i];
        if (ptr) {
            Info *info = new Info(ptr);
            // to avoid duplicates
            if (!info->partClass().isEmpty()) {
                d->partsByClass.insert(info->partClass(), info);
                //qDebug() << "inserting info to" << info->partClass();
            }
            d->partlist.append(info);
        }
    }
    d->lookupResult = true;
    return true;
}

Part* Manager::part(Info *i)
{
    Part *p = d->part<Part>(i, d->parts);
    if (p) {
        emit partLoaded(p);
    }
    return p;
}

static QString realPartClass(const QString &className)
{
    if (className.contains('.')) {
        return className;
    }
    else {
        // not like "org.kexi-project.table" - construct
        return QString::fromLatin1("org.kexi-project.")
            + QString(className).remove("kexi/");
    }
}

Part* Manager::partForClass(const QString &className)
{
    Info* info = infoForClass(className);
    return part(info);
}

Info* Manager::infoForClass(const QString &className)
{
    if (!lookup())
        return 0;
    const QString realClass = realPartClass(className);
    Info *i = realClass.isEmpty() ? 0 : d->partsByClass.value(realClass);
    if (i)
        return i;
    setError(xi18nc("@info", "No plugin for class <resource>%1</resource>", realClass));
    return 0;
}

void Manager::insertStaticPart(StaticPart* part)
{
    if (!part)
        return;
    if (!lookup())
        return;
    d->partlist.append(part->info());
    if (!part->info()->partClass().isEmpty())
        d->partsByClass.insert(part->info()->partClass(), part->info());
    d->parts.insert(part->info()->partClass(), part);
}

KexiInternalPart* Manager::internalPartForClass(const QString& className)
{
    Info* info = infoForClass(className);
    return d->part<KexiInternalPart>(info, d->internalParts);
}

PartInfoList* Manager::infoList()
{
    if (!lookup()) {
        return 0;
    }
    return &d->partlist;
}

