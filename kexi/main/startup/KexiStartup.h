/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXI_STARTUPHANDLER_H
#define KEXI_STARTUPHANDLER_H

#include <qstring.h>
#include <qwidget.h>

#include <core/kexistartupdata.h>
#include <core/kexi.h>
#include <kexiutils/tristate.h>

class KexiProjectData;
class KexiProjectData;
class KCmdLineArgs;
class KexiStartupHandlerPrivate;
namespace KexiDB {
	class ConnectionData;
}

/*! Handles startup actions for Kexi application.
*/
class KEXIMAIN_EXPORT KexiStartupHandler 
	: public QObject, public KexiStartupData, public Kexi::ObjectStatus
{
	Q_OBJECT

	public:
		KexiStartupHandler();
		virtual ~KexiStartupHandler();

		virtual tristate init(int argc, char **argv);
		
	#if 0
		/*! Used for opening existing projects. 
		 Detects project file type by mime type and returns project data, if it can be detected,
		 otherwise - NULL. \a parent is passed as parent for potential error message boxes.
		 Also uses \a cdata connection data for server-based projects.
		 cdata.driverName is adjusted, if a file-based project has been detected.
		*/
		static KexiProjectData* detectProjectData( 
			KexiDB::ConnectionData& cdata, const QString &dbname, QWidget *parent);
	#endif

		/*! Options for detectDriverForFile() */
		enum DetectDriverForFileOptions { 
			DontConvert = 1, //!< skip asking for conversion (used e.g. when dropdb is called)
			ThisIsAProjectFile = 2, //!< a hint, forces detection of the file as a project file
			ThisIsAShortcutToAProjectFile = 4, //!< a hint, forces detection of the file 
			                                   //!< as a shortcut to a project file
			ThisIsAShortcutToAConnectionData = 8 //!< a hint, forces detection of the file 
			                                     //!< as a shortcut to a connection data
		};

		/*! Used for opening existing file-based projects. 
		 Detects project file type by mime type.
		 \return driver name suitable for it -- if it can be detected, otherwise - NULL. 
		 Can return "shortcut" string if the file looks like a shortcut to a project/connection file
		 or "connection" if the file looks like a connection data file.
		 \a parent is passed as a parent for potential error message boxes.
		 \a driverName is a preferred driver name. For 
		 cdata.driverName is adjusted, if a file-based project has been detected.
		*/
		static QString detectDriverForFile( const QString& driverName, 
			const QString &dbFileName, QWidget *parent = 0, int options = 0 );

		/*! Allows user to select a project with KexiProjectSelectorDialog.
			\return selected project's data
			Returns NULL and sets cancelled to true if the dialog was cancelled.
			Returns NULL and sets cancelled to false if there was an error.
		*/
		KexiProjectData* selectProject(KexiDB::ConnectionData *cdata, bool& cancelled, QWidget *parent = 0);

	protected slots:
		void slotSaveShortcutFileChanges();
		void slotShowConnectionDetails();

	protected:
		bool getAutoopenObjects(KCmdLineArgs *args, const QCString &action_name);

		KexiStartupHandlerPrivate *d;
};

namespace Kexi
{
	KEXIMAIN_EXPORT KexiStartupHandler& startupHandler();
}

#endif

