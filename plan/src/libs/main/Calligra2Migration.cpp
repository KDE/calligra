/* This file is part of the KDE project
 *   Copyright (C) 2016 Dag Andersen <danders@get2net.dk>
 * 
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 * 
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 * 
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#include "Calligra2Migration.h"

#include "KoResourcePaths.h"

#include <kdelibs4configmigrator.h>
#include <kdelibs4migration.h>
#include <QStandardPaths>
#include <QDir>
#include <QPluginLoader>
#include <QLoggingCategory>
#include <QDebug>

Q_DECLARE_LOGGING_CATEGORY(CALLIGRA2MIGRATION)
Q_LOGGING_CATEGORY(CALLIGRA2MIGRATION, "calligra.lib.migration")

Calligra2Migration::Calligra2Migration(const QString &appName, const QString &oldAppName)
    : m_newAppName(appName)
    , m_oldAppName(oldAppName)
{
    qCDebug(CALLIGRA2MIGRATION)<<appName<<oldAppName;
}

void Calligra2Migration::setConfigFiles(const QStringList &configFiles)
{
    m_configFiles = configFiles;
}

void Calligra2Migration::setUiFiles(const QStringList &uiFiles)
{
    m_uiFiles = uiFiles;
}

void Calligra2Migration::migrate()
{
    const QString newdatapath = KoResourcePaths::saveLocation("data", m_newAppName, false);
    QDir newdatadir(newdatapath);
    if (newdatadir.exists()) {
        // assume we have migrated
        qCDebug(CALLIGRA2MIGRATION)<<"migration has been done";
        return;
    }

    // do common calligra in case not yet migrated
    Kdelibs4ConfigMigrator m("calligra");
    m.setConfigFiles(QStringList() << QStringLiteral("calligrarc"));
    m.setUiFiles(QStringList() << QStringLiteral("calligra_shell.rc") << QStringLiteral("osx.stylesheet"));
    m.migrate();

    bool didSomething = false;
    Kdelibs4ConfigMigrator cm(m_oldAppName.isEmpty() ? m_newAppName : m_oldAppName);
    cm.setConfigFiles(m_configFiles);
    cm.setUiFiles(m_uiFiles);
    if (cm.migrate() && !m_oldAppName.isEmpty()) {
        // rename config files to new names
        qCDebug(CALLIGRA2MIGRATION)<<"rename config files to new names"<<m_configFiles;
        for (const QString &oldname : m_configFiles) {
            QString newname = oldname;
            newname.replace(m_oldAppName, m_newAppName);
            if (oldname == newname) {
                continue;
            }
            QString oldfile = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, oldname);
            if (!oldfile.isEmpty()) {
                qCDebug(CALLIGRA2MIGRATION)<<"config rename:"<<oldfile;
                QFile f(oldfile);
                QFileInfo fi(f);
                f.rename(fi.absolutePath() + '/' + newname);
                didSomething = true;
                qCDebug(CALLIGRA2MIGRATION)<<"config renamed:"<<f.fileName();
            }
        }
        // subdirectory must be renamed
        // eg: .local/share/kxmlgui5/ + m_oldAppName
        // rename to: .local/share/kxmlgui5/ + m_newAppName
        const QString loc = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/kxmlgui5/");
        const QString oldui = loc + m_oldAppName;
        const QString newui = loc + m_newAppName;
        qCDebug(CALLIGRA2MIGRATION)<<"rename ui dir:"<<oldui;
        QDir newdir(newui);
        if (!newdir.exists()) {
            newdir.rename(oldui, newui);
            didSomething = true;
            qCDebug(CALLIGRA2MIGRATION)<<"renamed ui dir:"<<newui;
        }
        qCDebug(CALLIGRA2MIGRATION)<<"rename ui files to new names"<<m_uiFiles;
        for (const QString &oldname : m_uiFiles) {
            QString newname = oldname;
            newname.replace(m_oldAppName, m_newAppName);
            if (oldname == newname) {
                continue;
            }
            const QString oldfile = newui + QLatin1Char('/') + oldname;
            QFile f(oldfile);
            if (f.exists()) {
                const QString newfile = newui + QLatin1Char('/') + newname;
                f.rename(newfile);
                didSomething = true;
                QFileInfo fi(f);
                qCDebug(CALLIGRA2MIGRATION)<<"ui renamed:"<<oldfile<<"->"<<fi.filePath();
            }
        }
    }

    // migrate data dir, must be done after ui files
    Kdelibs4Migration md;
    if (md.kdeHomeFound()) {
        const QString oldpath = md.saveLocation("data", m_oldAppName.isEmpty() ? m_newAppName : m_oldAppName);
        if (!oldpath.isEmpty()) {
            qCDebug(CALLIGRA2MIGRATION)<<"old data:"<<oldpath;
            newdatadir.rename(oldpath, newdatapath);
            qCDebug(CALLIGRA2MIGRATION)<<"renamed data:"<<newdatapath;
        }
    } else {
        qCWarning(CALLIGRA2MIGRATION)<<"kde home not found";
    }

    // Copied from Kdelibs4ConfigMigrator:
    // Trigger KSharedConfig::openConfig()->reparseConfiguration() via the framework integration plugin
    if (didSomething) {
        qCDebug(CALLIGRA2MIGRATION)<<"reparse configuration";
        QPluginLoader lib(QStringLiteral("kf5/FrameworkIntegrationPlugin"));
        QObject *rootObj = lib.instance();
        if (rootObj) {
            QMetaObject::invokeMethod(rootObj, "reparseConfiguration");
        }
    }
}
