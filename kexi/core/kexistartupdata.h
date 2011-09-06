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

#ifndef KEXI_STARTUPDATA_H
#define KEXI_STARTUPDATA_H

#include <qstring.h>
#include <kexi_export.h>

class KexiProjectData;

//! Startup data used for storing results of startup operations in Kexi.
//! @see KexiStartupHandler
class KEXICORE_EXPORT KexiStartupData
{
public:
    enum Action {
        DoNothing,
        CreateBlankProject,
        CreateFromTemplate,
        OpenProject,
        ImportProject,
        ShowWelcomeScreen,
        Exit
    };

    /*! Data required to perform import action.
     It is set by KexiStartupHandler::detectActionForFile()
     if a need for project/data importing has been detected. */
    class KEXICORE_EXPORT Import
    {
    public:
        Import();
        operator bool() const;
        QString fileName;
        QString mimeType;
    };

    KexiStartupData();
    virtual ~KexiStartupData();

    virtual bool init() {
        return true;
    }

    Action action() const;

    //! \return project data of a project that should be opened (for action()==OpenProject)
    KexiProjectData *projectData() const;

    //! \return import action's data needed to perform import (for action()==ImportProject)
    KexiStartupData::Import importActionData() const;

    /*! \return true is the Design Mode is forced for this project.
      Used on startup (by --design-mode comman line switch). */
    bool forcedDesignMode() const;

    /*! \return true is the User Mode is forced for this project.
      Used on startup (by --user-mode comman line switch).
      By default this is false. */
    bool forcedUserMode() const;

    /*! \return true if the Project Navigator should be visible even if User Mode is on. */
    bool isProjectNavigatorVisible() const;

    /*! \return true if the main menu (usually displayed as the tabbed toolbar) should be visible. */
    bool isMainMenuVisible() const;
    
protected:
    KexiProjectData *m_projectData;
    Action m_action;
    KexiStartupData::Import m_importActionData;
    bool m_forcedUserMode;
    bool m_forcedDesignMode;
    bool m_isProjectNavigatorVisible;
    bool m_isMainMenuVisible;
    bool m_createDB;
    bool m_dropDB;
    bool m_alsoOpenDB;
};

#endif
