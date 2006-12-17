/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiactioncategories.h"

#include <kstaticdeleter.h>
#include <qmap.h>
#include <qasciidict.h>

namespace Kexi {

//! @internal
class ActionInternal
{
	public:
		ActionInternal(int _categories) : categories(_categories), supportedObjectTypes(0) {}
		~ActionInternal() { delete supportedObjectTypes; }
		int categories;
		QMap<int, bool> *supportedObjectTypes;
};

static KStaticDeleter<ActionCategories> Kexi_actionCategoriesDeleter;
ActionCategories* Kexi_actionCategories = 0;

//! @internal
class ActionCategories::Private
{
	public:
		Private()
		{
			actions.setAutoDelete(true);
		}

	QAsciiDict<ActionInternal> actions;
};

KEXICORE_EXPORT ActionCategories *actionCategories()
{
	if (!Kexi_actionCategories)
		Kexi_actionCategoriesDeleter.setObject( Kexi_actionCategories, new ActionCategories() );
	return Kexi_actionCategories;
}

}

using namespace Kexi;

//----------------------------------

ActionCategories::ActionCategories()
 : d( new Private() )
{
}

ActionCategories::~ActionCategories()
{
	delete d;
}

void ActionCategories::addAction(const char* name, int categories,
	KexiPart::ObjectTypes supportedObjectType1, KexiPart::ObjectTypes supportedObjectType2,
	KexiPart::ObjectTypes supportedObjectType3, KexiPart::ObjectTypes supportedObjectType4, 
	KexiPart::ObjectTypes supportedObjectType5, KexiPart::ObjectTypes supportedObjectType6,
	KexiPart::ObjectTypes supportedObjectType7, KexiPart::ObjectTypes supportedObjectType8)
{
	ActionInternal * a = d->actions.find( name );
	if (a) {
		a->categories |= categories;
	}
	else {
		a = new ActionInternal(categories);
		d->actions.insert(name, a);
	}
	if (supportedObjectType1) {
		if (!a->supportedObjectTypes)
			a->supportedObjectTypes = new QMap<int, bool>();
		a->supportedObjectTypes->insert(supportedObjectType1, true);
		if (supportedObjectType2) {
			a->supportedObjectTypes->insert(supportedObjectType2, true);
			if (supportedObjectType3) {
				a->supportedObjectTypes->insert(supportedObjectType3, true);
				if (supportedObjectType4) {
					a->supportedObjectTypes->insert(supportedObjectType4, true);
					if (supportedObjectType5) {
						a->supportedObjectTypes->insert(supportedObjectType5, true);
						if (supportedObjectType6) {
							a->supportedObjectTypes->insert(supportedObjectType6, true);
							if (supportedObjectType7) {
								a->supportedObjectTypes->insert(supportedObjectType7, true);
								if (supportedObjectType8) {
									a->supportedObjectTypes->insert(supportedObjectType8, true);
								}
							}
						}
					}
				}
			}
		}
	}
}

int ActionCategories::actionCategories(const char* name) const
{
	const ActionInternal * a = d->actions.find( name );
	return a ? a->categories : 0;
}

bool ActionCategories::actionSupportsObjectType(const char* name, KexiPart::ObjectTypes objectType) const
{
	const ActionInternal * a = d->actions.find( name );
	return (a && a->supportedObjectTypes) ? a->supportedObjectTypes->contains(objectType) : false;
}
