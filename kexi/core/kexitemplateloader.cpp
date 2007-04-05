/* This file is part of the KDE project
   Copyright (C) 2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexitemplateloader.h"

#include <kstandarddirs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>

#include <qdir.h>

//static
KexiTemplateInfo::List KexiTemplateLoader::loadListInfo()
{
	KexiTemplateInfo::List list;
	const QString subdir = QString(kapp->instanceName()) + "/templates";
	QString lang( KGlobal::locale()->language() );
	QStringList dirs( kapp->dirs()->findDirs("data", subdir) );
	while (true) {
		foreach( QStringList::ConstIterator, it, dirs) {
			QDir dir((*it)+lang);
			if (!dir.exists())
				continue;
			if (!dir.isReadable()) {
				kWarning() << "KexiTemplateLoader::loadListInfo() \"" << dir.absPath() << "\" not readable!" << endl;
				continue;
			}
			const QStringList templateDirs( dir.entryList(QDir::Dirs, QDir::Name) );
			const QString absDirPath( dir.absPath() + '/' );
			foreach(QStringList::ConstIterator, templateIt, templateDirs) {
				if ((*templateIt)=="." || (*templateIt==".."))
					continue;
				KexiTemplateInfo info = KexiTemplateLoader::loadInfo( absDirPath + *templateIt );
				if (!info.name.isEmpty())
					list.append( info );	
			}
		}
		if (lang != "en" && list.isEmpty()) //not found for current locale, try "en"
			lang = "en";
		else
			break;
	}
	return list;
}

//static
KexiTemplateInfo KexiTemplateLoader::loadInfo(const QString& directory)
{
	QDir dir(directory);
	if (!dir.isReadable()) {
		kWarning() << "KexiTemplateLoader::loadInfo() \"" 
			<< directory << "\" not readable!" << endl;
		return KexiTemplateInfo();
	}
	if (!QFileInfo(directory+"/info.txt").isReadable())
		return KexiTemplateInfo();
	KConfig infoTxt(directory+"/info.txt", true/*readonly*/, false/*local*/);
	KexiTemplateInfo info;
	info.name = infoTxt.readEntry("Name");
	if (info.name.isEmpty()) {
		kWarning() << "KexiTemplateLoader::loadInfo() \"" << (directory+"/info.txt") << "\" contains no \"name\" field" << endl;
		return KexiTemplateInfo();
	}
	const QStringList templateFiles( dir.entryList("*.kexi", QDir::Files|QDir::Readable, QDir::Name) );
	if (templateFiles.isEmpty()) {
		kWarning() << "KexiTemplateLoader::loadInfo() no readable .kexi template file found in \"" << directory << "\"" << endl;
		return KexiTemplateInfo();
	}
	info.filename = directory+"/"+templateFiles.first();
	info.description = infoTxt.readEntry("Description");
	const QString iconFileName( infoTxt.readEntry("Icon") );
	if (!iconFileName.isEmpty())
		info.icon = QPixmap(directory+'/'+iconFileName);
	if (info.icon.isNull())
		info.icon = DesktopIcon("kexiproject_sqlite"); //default
	QStringList autoopenObjectsString = infoTxt.readListEntry("AutoOpenObjects");
	foreach( QStringList::ConstIterator, it, autoopenObjectsString) {
		KexiProjectData::ObjectInfo autoopenObject;
		QStringList autoopenObjectNameSplitted( QStringList::split(':', *it) );
		if (autoopenObjectNameSplitted.count()>1) {
			autoopenObject["type"] = autoopenObjectNameSplitted[0];
			autoopenObject["name"] = autoopenObjectNameSplitted[1];
		}
		else {
			autoopenObject["type"] = "table";
			autoopenObject["name"] = autoopenObjectNameSplitted[0];
		}
		autoopenObject["action"] = "open";
		info.autoopenObjects.append( autoopenObject );
	}
	return info;
}
