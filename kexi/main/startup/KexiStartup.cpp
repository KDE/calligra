/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include "KexiStartup.h"
#include "kexiprojectdata.h"
#include "kexi.h"

#include <kexidb/driver.h>
#include <kexidb/drivermanager.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kmessagebox.h>

#include <qfileinfo.h>

using namespace Kexi;

KexiProjectData* Kexi::detectProjectData( const QString &fname, QWidget *parent)
{
	KexiProjectData *projectData = 0;
	QFileInfo finfo(fname);
	if (fname.isEmpty() || !finfo.isReadable()) {
		KMessageBox::sorry(parent, i18n("<qt>The file \"%1\" does not exist.").arg(fname));
		return 0;
	}
	if (!finfo.isWritable()) {
		//TODO: if file is ro: change project mode
	}
	KMimeType::Ptr ptr = KMimeType::findByFileContent(fname);
	QString mimename = ptr.data()->name();
	kdDebug() << "Kexi::detectProjectData(): found mime is: " << ptr.data()->name() << endl;
	if (mimename=="application/x-kexiproject-shortcut") {
		return 0;//TODO: get information for xml shortcut file
	}
	// "application/x-kexiproject-sqlite", etc
	QString drivername = Kexi::driverManager().lookupByMime(mimename);
	kdDebug() << "Kexi::detectProjectData(): driver name: " << drivername << endl;
	if(drivername.isEmpty()) {
		KMessageBox::detailedSorry(parent, i18n( "The file \"%1\" is not recognized as being supported by Kexi.").arg(fname),
		i18n("<qt>Database driver for this file type not found. <p>Detected MIME type: %1").arg(mimename));
		return 0;
	}
	KexiDB::ConnectionData cdata;
	cdata.driverName = drivername;
	cdata.setFileName( fname );
	projectData = new KexiProjectData(cdata,fname,fname);
	kdDebug() << "Kexi::detectProjectData(): file is a living database of engine " << drivername << endl;
	return projectData;
}

