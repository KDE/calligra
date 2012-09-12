/* This file is part of the KDE project
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXI_H
#define KEXI_H

#include <QPointer>

#include <kexi_version.h>
#include "kexiprojectdata.h"
#include "kexipartmanager.h"
#include "kexidbconnectionset.h"
#include "kexiprojectset.h"
#include <db/drivermanager.h>
#include <db/driver.h>

#include <klocale.h>
#include <kmessagebox.h>

class QLabel;
class KexiAboutData;
class KexiRecentProjects;

namespace Kexi
{
KEXICORE_EXPORT void initCmdLineArgs(int argc, char *argv[], const KexiAboutData& aboutData);

/*! Modes of view for the dialogs. Used mostly for parts and KexiWindow. */
enum ViewMode {
    AllViewModes = 0, //!< Usable primarily in KexiPart::initInstanceActions()
    NoViewMode = 0, //!< In KexiView::afterSwitchFrom() and KexiView::beforeSwitchTo()
    //!< means that parent dialog of the view has not yet view defined.
    DataViewMode = 1,
    DesignViewMode = 2,
    TextViewMode = 4 //!< Also known as SQL View Mode
};
Q_DECLARE_FLAGS(ViewModes, ViewMode)

/*! @return i18n'ed name of view mode @a mode. If @a withAmpersand is true,
 ampersands used for accelerators are included, e.g. "&Data View".*/
KEXICORE_EXPORT QString nameForViewMode(ViewMode mode, bool withAmpersand = false);

/*! @return icon name of view mode @a mode. */
KEXICORE_EXPORT QString iconNameForViewMode(ViewMode mode);

//! A set of known connections
KEXICORE_EXPORT KexiDBConnectionSet& connset();

//! A set available of project information
KEXICORE_EXPORT KexiRecentProjects* recentProjects();

//! shared driver manager
KEXICORE_EXPORT KexiDB::DriverManager& driverManager();

//! shared part manager
KEXICORE_EXPORT KexiPart::Manager& partManager();

//! can be called to delete global objects like driverManager and partManager
//! (and thus, all loaded factories/plugins)
//! before KLibrary::~KLibrary() do this for us
KEXICORE_EXPORT void deleteGlobalObjects();

//some temporary flags

//! false by default, flag loaded on main window startup
KEXICORE_EXPORT bool& tempShowMacros();

//! false by default, flag loaded on main window startup
KEXICORE_EXPORT bool& tempShowScripts();

/*! Helper class for storing object status. */
class KEXICORE_EXPORT ObjectStatus
{
public:
    ObjectStatus();

    ObjectStatus(const QString& message, const QString& description);

    ObjectStatus(KexiDB::Object* dbObject, const QString& message, const QString& description);

    ~ObjectStatus();

    const ObjectStatus& status() const;

    bool error() const;

    void setStatus(const QString& message, const QString& description);

    //! Note: for safety, \a dbObject needs to be derived from QObject,
    //! otherwise it won't be assigned
    void setStatus(KexiDB::Object* dbObject,
                   const QString& message = QString(), const QString& description = QString());

    void setStatus(KexiDB::ResultInfo* result,
                   const QString& message = QString(), const QString& description = QString());

    void setStatus(KexiDB::Object* dbObject, KexiDB::ResultInfo* result,
                   const QString& message = QString(), const QString& description = QString());

    void clearStatus();

    QString singleStatusString() const;

    void append(const ObjectStatus& otherStatus);

    KexiDB::Object *dbObject() const {
        return dynamic_cast<KexiDB::Object*>((QObject*)dbObj);
    }

    //! Helper returning pseudo handler that just updates this ObjectStatus object
    //! by receiving a message
    operator KexiDB::MessageHandler*();

    QString message, description;
protected:
    QPointer<QObject> dbObj; //! This is in fact KexiDB::Object
    KexiDB::MessageHandler* msgHandler;
};

KEXICORE_EXPORT QString msgYouCanImproveData();

}//namespace Kexi

//! Icon name for database servers
#define KEXI_ICON_DATABASE_SERVER "network-server-database"

Q_DECLARE_OPERATORS_FOR_FLAGS(Kexi::ViewModes)

//! Displays information that feature "feature_name" is not availabe in the current application version
KEXICORE_EXPORT void KEXI_UNFINISHED(
    const QString& feature_name, const QString& extra_text = QString());

//! Like KEXI_UNFINISHED but returns new label instance with expected text
KEXICORE_EXPORT QLabel *KEXI_UNFINISHED_LABEL(
    const QString& feature_name, const QString& extra_text = QString());

//! Like above - for use inside KexiActionProxy subclass - reuses feature name from shared action's text
#define KEXI_UNFINISHED_SHARED_ACTION(action_name) \
    KEXI_UNFINISHED(sharedAction(action_name) ? sharedAction(action_name)->text() : QString())

#endif
