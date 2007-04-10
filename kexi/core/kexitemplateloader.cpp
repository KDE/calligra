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
//	const QString subdir = QString(kapp->instanceName()) + "/templates";
#ifdef __GNUC
#warning KexiTemplateLoader::loadListInfo() -- OK? KGlobal::mainComponent().componentName()
#endif
	const QString subdir = KGlobal::mainComponent().componentName() + "/templates";
	QString lang( KGlobal::locale()->language() );
	QStringList dirs( KGlobal::dirs()->findDirs("data", subdir) );
	while (true) {
		foreach( QString dirname, dirs) {
			QDir dir(dirname+lang);
			if (!dir.exists())
				continue;
			if (!dir.isReadable()) {
				kWarning() << "KexiTemplateLoader::loadListInfo() \"" << dir.absolutePath() << "\" not readable!" << endl;
				continue;
			}
			const QStringList templateDirs( dir.entryList(QDir::Dirs, QDir::Name) );
			const QString absDirPath( dir.absolutePath() + '/' );
			foreach(QString templateDir, templateDirs) {
				if (templateDir=="." || templateDir=="..")
					continue;
				KexiTemplateInfo info = KexiTemplateLoader::loadInfo( absDirPath + templateDir );
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
	KConfig infoTxt(directory+"/info.txt", KConfig::OnlyLocal);
	KConfigGroup cg = infoTxt.group(QString());

	KexiTemplateInfo info;
	info.name = cg.readEntry("Name");
	if (info.name.isEmpty()) {
		kWarning() << "KexiTemplateLoader::loadInfo() \"" << (directory+"/info.txt") << "\" contains no \"name\" field" << endl;
		return KexiTemplateInfo();
	}
	QStringList templateFileNameFilters;
	templateFileNameFilters.append("*.kexi");
	const QStringList templateFiles( 
		dir.entryList(templateFileNameFilters, QDir::Files|QDir::Readable, QDir::Name) );
	if (templateFiles.isEmpty()) {
		kWarning() << "KexiTemplateLoader::loadInfo() no readable .kexi template file found in \"" << directory << "\"" << endl;
		return KexiTemplateInfo();
	}
	info.filename = directory+"/"+templateFiles.first();
	info.description = cg.readEntry("Description");
	const QString iconFileName( cg.readEntry("Icon") );
	if (!iconFileName.isEmpty())
		info.icon = QPixmap(directory+'/'+iconFileName);
	if (info.icon.isNull())
		info.icon = DesktopIcon("kexiproject_sqlite"); //default
	QStringList autoopenObjectsString = cg.readEntry("AutoOpenObjects", QStringList());
	foreach( QString autoopenObjectString, autoopenObjectsString) {
		KexiProjectData::ObjectInfo autoopenObject;
		QStringList autoopenObjectNameSplitted( autoopenObjectString.split(':') );
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
