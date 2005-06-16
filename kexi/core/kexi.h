/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KEXI_H__
#define __KEXI_H__

#include <qguardedptr.h>

#include <kexi_version.h>
#include "kexiprojectdata.h"
#include "kexipartmanager.h"
#include "kexidbconnectionset.h"
#include "kexiprojectset.h"
#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>

#include <klocale.h>
#include <kmessagebox.h>

namespace Kexi
{
	/*! Modes of view for the dialogs. Used mostly for parts and KexiDialogBase. */
	enum ViewMode { 
		AllViewModes = 0, //!< Usable primarily in KexiPart::initInstanceActions()
		NoViewMode = 0, //!< In KexiViewBase::afterSwitchFrom() and KexiViewBase::beforeSwitchTo() 
		                //!< means that parent dialog of the view has not yet view defined.
		DataViewMode = 1,
		DesignViewMode = 2,
		TextViewMode = 4 //!< Also known as SQL View Mode
	};
	//! i18n'ed name of view mode \a m
	KEXICORE_EXPORT QString nameForViewMode(int m);

	//! A set of known connections
	KEXICORE_EXPORT KexiDBConnectionSet& connset();
	
	//! A set avaiulable of project infos
	KEXICORE_EXPORT KexiProjectSet& recentProjects();
	
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
	KEXICORE_EXPORT bool& tempShowForms(); 

	//! false by default, flag loaded on main window startup
	KEXICORE_EXPORT bool& tempShowReports(); 

	//! false by default, flag loaded on main window startup
	KEXICORE_EXPORT bool& tempShowScripts(); 

	/*! Helper class for storing object status. */
	class KEXICORE_EXPORT ObjectStatus
	{
		public:
			ObjectStatus();
			ObjectStatus(const QString& message, const QString& description);
			ObjectStatus(KexiDB::Object* dbObject, const QString& message, const QString& description);
			const ObjectStatus& status() const;
			bool error() const;
			void setStatus(const QString& message, const QString& description);
			void setStatus(KexiDB::Object* dbObject, const QString& message, const QString& description = QString::null);
			void clearStatus();
			QString singleStatusString() const;
			void append( const ObjectStatus& otherStatus );

			KexiDB::Object *dbObject() const { return dbObj; }

			QString message, description;
		protected:
//todo: make it guarded!
			KexiDB::Object *dbObj; 
	};

	QString msgYouCanImproveData() {
		return i18n("You can correct data in this row or use \"Cancel row changes\" function.");
	}

}//namespace Kexi

//! displays information that feature "feature_name" is not availabe in the current application version
inline void KEXI_UNFINISHED(QString feature_name, QString extra_text = QString::null) 
{
	QString msg;
	if (feature_name.isEmpty())
		msg = i18n("This function is not available for version %1 of %2 application.")
			.arg(KEXI_VERSION_STRING)
			.arg(KEXI_APP_NAME); 
	else 
		msg = i18n("\"%1\" function is not available for version %2 of %3 application.")
			.arg(feature_name.replace("&",""))
			.arg(KEXI_VERSION_STRING)
			.arg(KEXI_APP_NAME); 

	if (!extra_text.isEmpty())
		extra_text.prepend("\n");

	KMessageBox::sorry(0, msg + extra_text);
}

//! like above - for use inside KexiActionProxy subclass - reuses feature name from shared action's text
#define KEXI_UNFINISHED_SHARED_ACTION(action_name) \
	KEXI_UNFINISHED(sharedAction(action_name) ? sharedAction(action_name)->text() : QString::null)

#endif
