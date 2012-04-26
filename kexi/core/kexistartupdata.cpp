/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

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

#include "kexistartupdata.h"
#include "kexi.h"

#include <kexidb/driver.h>
#include <kexidb/drivermanager.h>

#include <QFileInfo>

KexiStartupData::KexiStartupData()
        : m_projectData(0)
        , m_action(KexiStartupData::DoNothing)
        , m_forcedUserMode(false)
        , m_forcedDesignMode(false)
        , m_isProjectNavigatorVisible(false)
// , m_createDB(false)
// , m_dropDB(false)
// , m_alsoOpenDB(false)
{
}

KexiStartupData::~KexiStartupData()
{
    delete m_projectData;
}

KexiProjectData *KexiStartupData::projectData() const
{
    return m_projectData;
}

KexiStartupData::Action KexiStartupData::action() const
{
    return m_action;
}

bool KexiStartupData::forcedUserMode() const
{
    return m_forcedUserMode;
}

bool KexiStartupData::forcedDesignMode() const
{
    return m_forcedDesignMode;
}

bool KexiStartupData::isProjectNavigatorVisible() const
{
    if (m_forcedUserMode && !m_forcedDesignMode)
        return m_isProjectNavigatorVisible;
    return true;
}

bool KexiStartupData::isMainMenuVisible() const
{
    return m_isMainMenuVisible;
}

KexiStartupData::Import KexiStartupData::importActionData() const
{
    return m_importActionData;
}

KexiStartupData::Import::Import()
{
}

KexiStartupData::Import::operator bool() const
{
    return !fileName.isEmpty() && !mimeType.isEmpty();
}
