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

#ifndef KEXI_TEMPLLOADER_H
#define KEXI_TEMPLLOADER_H

#include <qpixmap.h>
#include "kexiprojectdata.h"

//! A structure providing information about single kexi database template file
struct KEXICORE_EXPORT KexiTemplateInfo
{
	typedef Q3ValueList<KexiTemplateInfo> List;

	QString filename, name, description;
	QPixmap icon;
	Q3ValueList<KexiProjectData::ObjectInfo> autoopenObjects;
};

//! Handles retrieving information about templates
class KEXICORE_EXPORT KexiTemplateLoader
{
	public:
		static KexiTemplateInfo::List loadListInfo();
		static KexiTemplateInfo loadInfo(const QString& directory);
};

#endif
