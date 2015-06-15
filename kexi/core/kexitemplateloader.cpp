/* This file is part of the KDE project
   Copyright (C) 2007-2011 Jarosław Staniek <staniek@kde.org>

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

#include <KDbUtils>

#include <KStandardDirs>
#include <KConfig>
#include <KConfigGroup>
#include <KComponentData>

#include <QDir>
#include <QIcon>
#include <QStandardPaths>
#include <QApplication>

KexiTemplateCategoryInfo::KexiTemplateCategoryInfo()
  : enabled(true)
{
}

KexiTemplateCategoryInfo::~KexiTemplateCategoryInfo()
{
}

KexiTemplateInfo::KexiTemplateInfo()
  : enabled(true)
{
}

KexiTemplateInfo::~KexiTemplateInfo()
{
    qDeleteAll(autoopenObjects);
}

void KexiTemplateCategoryInfo::addTemplate(const KexiTemplateInfo& t)
{
    KexiTemplateInfo _t = t;
    _t.category = name;
    m_templates.append(_t);
}

//static
KexiTemplateInfoList KexiTemplateLoader::loadListInfo()
{
    KexiTemplateInfoList list;
//! @todo KEXI3 KAboutData::applicationData().componentName() OK?
    const QString subdir = KAboutData::applicationData().componentName() + "/templates";
    const QLocale locale;
    QString lang(QLocale::languageToString(locale.language()));
    QStringList dirs(QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, subdir));
    while (true) {
        foreach(const QString &dirname, dirs) {
            QDir dir(dirname + lang);
            if (!dir.exists())
                continue;
            if (!dir.isReadable()) {
                qWarning() << "\"" << dir.absolutePath() << "\" not readable!";
                continue;
            }
            const QStringList templateDirs(dir.entryList(QDir::Dirs, QDir::Name));
            const QString absDirPath(dir.absolutePath() + '/');
            foreach(const QString &templateDir, templateDirs) {
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
        qWarning() << "\"" << directory << "\" not readable!";
        return KexiTemplateInfo();
    }
    if (!QFileInfo(directory + "/info.txt").isReadable())
        return KexiTemplateInfo();
    KConfig infoTxt(directory + "/info.txt", KConfig::SimpleConfig);
    KConfigGroup cg = infoTxt.group(QString());

    KexiTemplateInfo info;
    info.name = cg.readEntry("Name");
    if (info.name.isEmpty()) {
        qWarning() << "\"" << (directory + "/info.txt") << "\" contains no \"name\" field";
        return KexiTemplateInfo();
    }
    QStringList templateFileNameFilters;
    templateFileNameFilters.append("*.kexi");
    const QStringList templateFiles(
        dir.entryList(templateFileNameFilters, QDir::Files | QDir::Readable, QDir::Name));
    if (templateFiles.isEmpty()) {
        qWarning() << "no readable .kexi template file found in \"" << directory << "\"";
        return KexiTemplateInfo();
    }
    info.filename = directory + "/" + templateFiles.first();
    info.description = cg.readEntry("Description");
    const QString iconFileName(cg.readEntry("Icon"));
    if (!iconFileName.isEmpty())
        info.icon = QIcon::fromTheme(directory + '/' + iconFileName);
    if (info.icon.isNull())
        info.icon = Kexi::defaultFileBasedDriverIcon();
    QStringList autoopenObjectsString = cg.readEntry("AutoOpenObjects", QStringList());
    foreach(const QString &autoopenObjectString, autoopenObjectsString) {
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
