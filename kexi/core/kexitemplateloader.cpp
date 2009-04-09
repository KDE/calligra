/* This file is part of the KDE project
   Copyright (C) 2007 Jarosław Staniek <staniek@kde.org>

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

#include "kexitemplateloader.h"
#include "kexi.h"
#include <kexidb/utils.h>

#include <kstandarddirs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>

#include <qdir.h>

KexiTemplateInfo::KexiTemplateInfo()
{
}

KexiTemplateInfo::~KexiTemplateInfo()
{
    qDeleteAll(autoopenObjects);
}

//static
KexiTemplateInfo::List KexiTemplateLoader::loadListInfo()
{
    KexiTemplateInfo::List list;
// const QString subdir = QString(kapp->instanceName()) + "/templates";
#ifdef __GNUC
#ifdef __GNUC__
#warning KexiTemplateLoader::loadListInfo() -- OK? KGlobal::mainComponent().componentName()
#else
#pragma WARNING( KexiTemplateLoader::loadListInfo() -- OK? KGlobal::mainComponent().componentName() )
#endif
#endif
    const QString subdir = KGlobal::mainComponent().componentName() + "/templates";
    QString lang(KGlobal::locale()->language());
    QStringList dirs(KGlobal::dirs()->findDirs("data", subdir));
    while (true) {
        foreach(QString dirname, dirs) {
            QDir dir(dirname + lang);
            if (!dir.exists())
                continue;
            if (!dir.isReadable()) {
                kWarning() << "\"" << dir.absolutePath() << "\" not readable!";
                continue;
            }
            const QStringList templateDirs(dir.entryList(QDir::Dirs, QDir::Name));
            const QString absDirPath(dir.absolutePath() + '/');
            foreach(QString templateDir, templateDirs) {
                if (templateDir == "." || templateDir == "..")
                    continue;
                KexiTemplateInfo info = KexiTemplateLoader::loadInfo(absDirPath + templateDir);
                if (!info.name.isEmpty())
                    list.append(info);
            }
        }
        if (lang != "en" && list.isEmpty()) //not found for current locale, try "en"
            lang = "en";
        else
            break;
    }
    return list;
}

//static
KexiTemplateInfo KexiTemplateLoader::loadInfo(const QString& directory)
{
    QDir dir(directory);
    if (!dir.isReadable()) {
        kWarning() << "\"" << directory << "\" not readable!";
        return KexiTemplateInfo();
    }
    if (!QFileInfo(directory + "/info.txt").isReadable())
        return KexiTemplateInfo();
    KConfig infoTxt(directory + "/info.txt", KConfig::SimpleConfig);
    KConfigGroup cg = infoTxt.group(QString());

    KexiTemplateInfo info;
    info.name = cg.readEntry("Name");
    if (info.name.isEmpty()) {
        kWarning() << "\"" << (directory + "/info.txt") << "\" contains no \"name\" field";
        return KexiTemplateInfo();
    }
    QStringList templateFileNameFilters;
    templateFileNameFilters.append("*.kexi");
    const QStringList templateFiles(
        dir.entryList(templateFileNameFilters, QDir::Files | QDir::Readable, QDir::Name));
    if (templateFiles.isEmpty()) {
        kWarning() << "no readable .kexi template file found in \"" << directory << "\"";
        return KexiTemplateInfo();
    }
    info.filename = directory + "/" + templateFiles.first();
    info.description = cg.readEntry("Description");
    const QString iconFileName(cg.readEntry("Icon"));
    if (!iconFileName.isEmpty())
        info.icon = QPixmap(directory + '/' + iconFileName);
    if (info.icon.isNull())
        info.icon = DesktopIcon(KexiDB::defaultFileBasedDriverIcon());
    QStringList autoopenObjectsString = cg.readEntry("AutoOpenObjects", QStringList());
    foreach(QString autoopenObjectString, autoopenObjectsString) {
        KexiProjectData::ObjectInfo* autoopenObject = new KexiProjectData::ObjectInfo();
        QStringList autoopenObjectNameSplitted(autoopenObjectString.split(':'));
        if (autoopenObjectNameSplitted.count() > 1) {
            autoopenObject->insert("type", autoopenObjectNameSplitted[0]);
            autoopenObject->insert("name", autoopenObjectNameSplitted[1]);
        } else {
            autoopenObject->insert("type", "table");
            autoopenObject->insert("name", autoopenObjectNameSplitted[0]);
        }
        autoopenObject->insert("action", "open");
        info.autoopenObjects.append(autoopenObject);
    }
    return info;
}
