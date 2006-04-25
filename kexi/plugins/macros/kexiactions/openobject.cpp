/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
 * copyright (C) 2006 by Sascha Kupper (kusato@kfnv.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "openobject.h"

#include "objectvariable.h"

#include <klocale.h>

using namespace KexiMacro;

namespace KexiMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class OpenObject::Private
	{
		public:
	};

}

OpenObject::OpenObject()
	: KoMacro::GenericAction<OpenObject>("openobject", i18n("Open Object"))
	, d( new Private() )
{
	addVariable( new ObjectVariable(this) );

	//QStringList objectlist;
	//objectlist << "Tables" << "Queries";
	//addVariable("object", i18n("Object"), QVariant(objectlist));
	/* example how to fetch the list of objects.
	KexiPart::PartInfoList* parts = Kexi::partManager().partInfoList();
	QStringList objectnames, objecttexts;
	for(KexiPart::PartInfoListIterator it(*parts); it.current(); ++it) {
		KexiPart::Info* info = it.current();
		if(info->isVisibleInNavigator()) {
			objectnames << info->objectName();
			objecttexts << info->groupName();
		}
	}
	*/

	QStringList viewlist;
	viewlist << "Data View" << "Design View" << "Text View";
	addVariable("view", i18n("View"), QVariant(viewlist));

	addVariable("name", i18n("Name"), QVariant(QString("")));
}

OpenObject::~OpenObject() 
{	
	delete d;
}

KoMacro::Variable::List OpenObject::notifyUpdated(KoMacro::Variable::Ptr variable)
{
	KoMacro::Variable::List l;
/*TODO
	if(var->name() == "object"){
		l.append(new Variable("view",i18n(""),QString("designview")));
		
		QString s = var->variant();
		if(s == "table"){
			l.append(new Variable("name",i18n(""),new SQL(select * from system_tables)));
		}
		else if(s == "form"){
			l.append(new Variable("name",i18n(""),new SQLformQuery());
		}
		else if(s == "query"){
			l.append(new Variable("name",i18n(""),new SQLqueryQuery());
		}
	}
*/
	return l;
}
