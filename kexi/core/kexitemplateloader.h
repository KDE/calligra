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

#ifndef KEXI_TEMPLLOADER_H
#define KEXI_TEMPLLOADER_H

#include <QList>
#include <KIcon>
#include "kexiprojectdata.h"

//! A structure providing information about single kexi database template
class KEXICORE_EXPORT KexiTemplateInfo
{
public:
    KexiTemplateInfo();
    ~KexiTemplateInfo();

    QString name;
    QString caption;
    QString description;
    QString category;
    QString filename;
    bool enabled;
    KIcon icon;
    KexiProjectData::AutoOpenObjects autoopenObjects;
};

typedef QList<KexiTemplateInfo> KexiTemplateInfoList;

//! A structure providing information about single category for kexi database template
class KEXICORE_EXPORT KexiTemplateCategoryInfo
{
public:
    KexiTemplateCategoryInfo();
    ~KexiTemplateCategoryInfo();

    QString name;
    QString caption;
    bool enabled;
    KexiTemplateInfoList templates() const { return m_templates; }
    void addTemplate(const KexiTemplateInfo& t);
private:
    KexiTemplateInfoList m_templates;
};

typedef QList<KexiTemplateCategoryInfo> KexiTemplateCategoryInfoList;

//! Handles retrieving information about templates
class KEXICORE_EXPORT KexiTemplateLoader
{
public:
    static KexiTemplateInfoList loadListInfo();
    static KexiTemplateInfo loadInfo(const QString& directory);
};

#endif
