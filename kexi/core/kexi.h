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

#include "kexiprojectdata.h"
#include "kexipartmanager.h"
#include "kexidbconnectionset.h"
#include "kexiprojectset.h"
#include "kexivalidator.h"
#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>

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

	//some utils
	
	//! \return valid filename based on \a s
	KEXICORE_EXPORT QString string2FileName(const QString &s);
	
	/*! always returns valid identifier based on \a s.
	 Non alphanumeric chars (or spaces) are replaced with '_'.
	 If a number char is at the beginning, '_' is added at start.
	 Empty strings are not changed.
	*/
	KEXICORE_EXPORT QString string2Identifier(const QString &s);
	
	//! Validates input for identifier name.
	class KEXICORE_EXPORT IdentifierValidator : public KexiValidator
	{
		public:
			IdentifierValidator(QObject * parent = 0, const char * name = 0);
			virtual ~IdentifierValidator();
			virtual State validate( QString & input, int & pos) const;

		protected:
			virtual Result internalCheck(const QString &valueName, const QVariant& v, 
				QString &message, QString &details);
	};

	/*! Validates input: 
	 accepts if the name is not reserved for internal kexi objects. */
	class KEXICORE_EXPORT KexiDBObjectNameValidator : public KexiValidator
	{
		public:
			/*! \a drv is a KexiDB driver on which isSystemObjectName() will be 
			 called inside check(). If \a drv is 0, KexiDB::Driver::isKexiDBSystemObjectName()
			 static function is called instead. */
			KexiDBObjectNameValidator(KexiDB::Driver *drv, QObject * parent = 0, const char * name = 0);
			virtual ~KexiDBObjectNameValidator();

		protected:
			virtual Result internalCheck(const QString &valueName, const QVariant& v, 
				QString &message, QString &details);
			QGuardedPtr<KexiDB::Driver> m_drv;
	};

	/*! Sets "wait" cursor with 1 second delay. */
	KEXICORE_EXPORT void setWaitCursor();
	/*! Remove "wait" cursor previously set with \a setWaitCursor(), 
	 even if it's not yet visible. */
	KEXICORE_EXPORT void removeWaitCursor();

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

	/*! Generates an icons set created by bitblt-ing a small "star" icon 
	 over icon pointed by \a origIconName. Resulted icon is added to application 
	 pixmap cache and thus is available using SmallIcon( \a newIconName ), 
	 and similar functions (from kiconloader.h). 
	 
	 Note: SmallIconSet() seems to not work for this.
	 
	 It's used for automatic creating icons for actions of creating new objects 
	 for particular Kexi parts. */
	KEXICORE_EXPORT void generateIconSetWithStar(
		const QString& origIconName, const QString& newIconName);

}//namespace Kexi

//! sometimes we leave a space in the form of empty QFrame and want to insert here
//! a widget that must be instantiated by hand.
//! This macro inserts a widget \a what into a frame \a where.
#define GLUE_WIDGET(what, where) \
	{ QVBoxLayout *lyr = new QVBoxLayout(where); \
	  lyr->addWidget(what); }

#endif

