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

#include "objectvariable.h"

#include <kexi_export.h>
#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>

#include <klocale.h>
#include <kdebug.h>

using namespace KexiMacro;

ObjectVariable::ObjectVariable(KoMacro::Action::Ptr action, const QString& objectname)
	: KoMacro::GenericVariable<ObjectVariable>("object", i18n("Object"), action)
{
	KexiPart::PartInfoList* parts = Kexi::partManager().partInfoList();
	for(KexiPart::PartInfoListIterator it(*parts); it.current(); ++it) {
		KexiPart::Info* info = it.current();
		if(info->isVisibleInNavigator()) {
			const QString name = info->objectName(); //info->groupName();
			children().append( KoMacro::Variable::Ptr(new KoMacro::Variable(name)) );
		}
	}

	if(! objectname.isNull())
		setVariant( objectname );
	else if(children().count() > 0)
		setVariant( children()[0]->variant() );
	else
		setVariant( QString::null );
}

ObjectVariable::~ObjectVariable()
{
}

