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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXI_STARTUPHANDLER_H
#define KEXI_STARTUPHANDLER_H

#include <qstring.h>
#include <qwidget.h>

#include <core/kexistartupdata.h>
#include <core/kexi.h>

class KexiProjectData;
class KexiProjectData;
class KCmdLineArgs;
class KexiStartupHandlerPrivate;
namespace KexiDB {
	class ConnectionData;
}

class KEXIMAIN_EXPORT KexiStartupHandler 
	: public QObject, public KexiStartupData, public Kexi::ObjectStatus
{
public:
	
	KexiStartupHandler();
	virtual ~KexiStartupHandler();

	virtual bool init(int argc, char **argv);
	
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

	/*! Used for opening existing file-based projects. 
	 Detects project file type by mime type and returns driver name suitable for it
	 -- if it can be detected, otherwise - NULL. 
	 \a parent is passed as a parent for potential error message boxes.
	 \a driverName is a preferred driver name. For 
	 cdata.driverName is adjusted, if a file-based project has been detected.
	*/
	static QString detectDriverForFile( const QString& driverName, 
		const QString &dbFileName, QWidget *parent = 0 );

	/*! Allows user to select a project with KexiProjectSelectorDialog.
		\return selected project's data or NULL if dialog was cancelled.
	*/
	KexiProjectData* selectProject(KexiDB::ConnectionData *cdata, QWidget *parent = 0);

protected:
	bool getAutoopenObjects(KCmdLineArgs *args, const QCString &action_name);

	KexiStartupHandlerPrivate *d;
};

namespace Kexi
{
	KEXIMAIN_EXPORT KexiStartupHandler& startupHandler();
}

#endif

