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

		virtual bool init();
		
		/*! Detects filename by mime type and returns project data, if it can be detected,
			otherwise - NULL. \a parent is passed as parent for potential error message boxes. */
		static KexiProjectData* detectProjectData( const QString &fname, QWidget *parent = 0 );

		/*! Allows user to select a project with KexiProjectSelectorDialog.
			\return selected project's data or NULL if dialog was cancelled.
		*/
		KexiProjectData* selectProject(KexiDB::ConnectionData *cdata, QWidget *parent = 0);

	protected:
		void getAutoopenObjects(KCmdLineArgs *args, const QCString &action_name);

		KexiStartupHandlerPrivate *d;
};

namespace Kexi
{
	KEXIMAIN_EXPORT KexiStartupHandler& startupHandler();
}

#endif

