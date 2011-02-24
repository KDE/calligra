/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2009 Jarosław Staniek <staniek@kde.org>

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

//#include <KLibLoader>
#include <KServiceTypeTrader>
#include <KDebug>
#include <KConfig>
#include <KConfigGroup>
#include <KLocale>

#include "kexipartmanager.h"
#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexistaticpart.h"
#include "kexi_version.h"

#include <kexidb/connection.h>
#include <kexidb/cursor.h>

using namespace KexiPart;

Manager::Manager(QObject *parent)
        : QObject(parent)
{
    m_lookupDone = false;
    m_lookupResult = false;
//    m_nextTempProjectPartID = -1;
}

Manager::~Manager()
{
    qDeleteAll(m_partlist);
    m_partlist.clear();
}

bool Manager::lookup()
{
//js: TODO: allow refreshing!!!! (will need calling removeClient() by Part objects)
    if (m_lookupDone)
        return m_lookupResult;
    m_lookupDone = true;
    m_lookupResult = false;
    m_partlist.clear();
    m_partsByClass.clear();
    m_parts.clear();

    if (!KServiceType::serviceType("Kexi/Handler")) {
        kWarning() << "No 'Kexi/Handler' service type installed! Aborting.";
        setError(i18n("No \"%1\" service type installed. Check your Kexi installation. Aborting.",
                      QString("Kexi/Handler")));
        return false;
    }
    KService::List tlist = KServiceTypeTrader::self()->query("Kexi/Handler",
                           "[X-Kexi-PartVersion] == " + QString::number(KEXI_PART_VERSION));

    KConfigGroup cg(KGlobal::config()->group("Parts"));
    const QStringList sl_order = cg.readEntry("Order").split(",");  //we'll set parts in defined order
    QVector<KService::Ptr> ordered(sl_order.count());

    //compute order
    foreach(KService::Ptr ptr, tlist) {
        QString partClass = ptr->property("X-Kexi-Class", QVariant::String).toString();
        QString partName = ptr->property("X-Kexi-TypeName", QVariant::String).toString();
        kDebug() << partName << partClass;
        if (   partClass.isEmpty()
            || (!Kexi::tempShowMacros() && partClass == "org.kexi-project.macro")
            || (!Kexi::tempShowScripts() && partClass == "org.kexi-project.script")
           )
        {
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
//            info->setProjectPartID(m_nextTempProjectPartID--); // temp. part id are -1, -2, and so on,
            // to avoid duplicates
            if (!info->partClass().isEmpty()) {
                m_partsByClass.insert(info->partClass(), info);
                kDebug() << "inserting info to" << info->partClass();
            }
            m_partlist.append(info);
        }
    }
    m_lookupResult = true;
    return true;
}

Part* Manager::part(Info *i)
{
    clearError();
    if (!i)
        return 0;
    if (!lookup())
        return 0;

    if (i->isBroken()) {
        setError(i->errorMessage());
        return 0;
    }

    Part *p = m_parts.value(i->partClass());
    if (!p) {
//2.0        int error = 0;
/*2.0        p = KService::createInstance<Part>(i->ptr(), this, QStringList(), &error); */
        KPluginLoader loader(i->ptr()->library());
        const uint foundMajor = (loader.pluginVersion() >> 16) & 0xff;
//        const uint foundMinor = (loader.pluginVersion() >> 8) & 0xff;
        if (foundMajor != KEXI_PART_VERSION) {
            i->setBroken(true, 
                i18n("Incompatible plugin \"%1\" version: found version %2, expected version %3.",
                    i->objectName(),
                    QString::number(foundMajor),
                    QString::number(KEXI_PART_VERSION)));
            setError(i->errorMessage());
            return 0;
        }
        KPluginFactory *factory = loader.factory();
        if (factory)
            p = factory->create<Part>(this);

        if (!p) {
            kDebug() << "failed";
            i->setBroken(true, i18n("Error while loading plugin \"%1\"", i->objectName()));
            setError(i->errorMessage());
            return 0;
        }
/*
        if (p->registeredPartID() > 0) {
            i->setProjectPartID(p->registeredPartID());
        }*/
        p->setInfo(i);
        p->setObjectName(QString("%1 plugin").arg(i->objectName()));
        m_parts.insert(i->partClass(), p);
        emit partLoaded(p);
    }
    return p;
}

static QString realPartClass(const QString &className)
{
    if (className.contains(".")) {
        return className;
    }
    else {
        // not like "org.kexi-project.table" - construct
        return QString::fromLatin1("org.kexi-project.")
            + QString(className).replace("kexi/", QString());
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
    Info *i = realClass.isEmpty() ? 0 : m_partsByClass.value(realClass);
    if (i)
        return i;
    setError(i18n("No plugin for class \"%1\"", realClass));
    return 0;
}

void Manager::insertStaticPart(StaticPart* part)
{
    if (!part)
        return;
    if (!lookup())
        return;
//    part->info()->setProjectPartID(m_nextTempProjectPartID--); // temp. part id are -1, -2, and so on,
    m_partlist.append(part->info());
    if (!part->info()->partClass().isEmpty())
        m_partsByClass.insert(part->info()->partClass(), part->info());
    m_parts.insert(part->info()->partClass(), part);
}

#include "kexipartmanager.moc"
