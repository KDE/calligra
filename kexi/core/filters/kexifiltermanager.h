/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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
#ifndef _KEXIFILTERMANAGER_
#define _KEXIFILTERMANAGER_

#include <qobject.h>
#include <qstringlist.h>
#include <kurl.h>

class KexiProject;

class KexiFilterManager:public QObject {

	Q_OBJECT

public:
	KexiFilterManager(KexiProject *project);
	virtual ~KexiFilterManager();
	enum StdDataTypes{Data=1,Queries=2,Forms=4,Scripts=8,AllEntries=0xFFFFFFFF};
	bool import(unsigned long importType);
	KexiProject *project();
private:
	bool doImport(const QString& lib, const KURL &url, unsigned long allowedTypes);
	KexiProject *m_project;
	QStringList m_typeNames; //This has to be modified if the StdDataTypes enum is changed
};

#endif
