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

#ifndef KEXIMACRO_OBJECTNAMEVARIABLE_H
#define KEXIMACRO_OBJECTNAMEVARIABLE_H

#include "../lib/variable.h"

#include "kexivariable.h"

#include <kexi_export.h>
#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>

#include <klocale.h>

namespace KexiMacro {

	/**
	* The ViewVariable class provide a list of KexiPart::PartItem's
	* supported by a KexiPart::Part as @a KoMacro::Variable .
	*/
	template<class ACTIONIMPL>
	class ObjectNameVariable : public KexiVariable<ACTIONIMPL>
	{
		public:
			ObjectNameVariable(ACTIONIMPL* actionimpl, const QString& objectname = QString::null, const QString& name = QString::null)
				: KexiVariable<ACTIONIMPL>(actionimpl, "name", i18n("Name"))
			{
				if(! actionimpl->mainWin()->project())
					return;

				QStringList namelist;
				KexiPart::Info* info = Kexi::partManager().infoForMimeType( QString("kexi/%1").arg(objectname) );
				if(info) {
					KexiPart::ItemDict* items = actionimpl->mainWin()->project()->items(info);
					if(items)
						for(KexiPart::ItemDictIterator item_it = *items; item_it.current(); ++item_it)
							namelist << item_it.current()->name();
				}

				if(namelist.count() <= 0)
					namelist << "";

				for(QStringList::Iterator it = namelist.begin(); it != namelist.end(); ++it)
					this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable(*it)) );

				this->setVariant( (name.isNull() || ! namelist.contains(name)) ? namelist[0] : name );
			}

			virtual ~ObjectNameVariable() {}

	};

}

#endif
