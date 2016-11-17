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
// logging category for this class, default: log stuff >= warning
Q_LOGGING_CATEGORY(CALLIGRA2MIGRATION, "calligra.lib.migration", QtWarningMsg)

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
                QFile f(oldfile);
                f.rename(newname);
                didSomething = true;
                qCDebug(CALLIGRA2MIGRATION)<<"config renamed:"<<oldfile<<"->"<<newname;
            }
        }
        qCDebug(CALLIGRA2MIGRATION)<<"rename ui files to new names"<<m_uiFiles;
        for (const QString &oldname : m_uiFiles) {
            QString newname = oldname;
            newname.replace(m_oldAppName, m_newAppName);
            if (oldname == newname) {
                continue;
            }
            QString oldfile = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/kxmlgui5/") + m_newAppName + QLatin1Char('/') + oldname;
            if (!oldfile.isEmpty()) {
                QFile f(oldfile);
                f.rename(newname);
                didSomething = true;
                qCDebug(CALLIGRA2MIGRATION)<<"ui renamed:"<<oldfile<<"->"<<newname;
            }
        }
    }
    if (!m_oldAppName.isEmpty()) {
        // rename data dirs
        Kdelibs4Migration m;
        if (m.kdeHomeFound()) {
            const QString oldpath = m.saveLocation("data", m_oldAppName);
            if (!oldpath.isEmpty()) {
                qCDebug(CALLIGRA2MIGRATION)<<"old data:"<<oldpath;
                const QString newpath = KoResourcePaths::saveLocation("data", m_newAppName, false);
                QDir newdir(newpath);
                if (!newdir.exists()) {
                    newdir.rename(oldpath, newpath); // copy instead?
                    qCDebug(CALLIGRA2MIGRATION)<<"renamed data:"<<newpath;
                }
            }
        } else {qCDebug(CALLIGRA2MIGRATION)<<"kde home not found";}
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
