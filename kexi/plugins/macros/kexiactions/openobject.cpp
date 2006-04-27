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

#include "../lib/variable.h"
#include "../lib/context.h"

#include <kexi_export.h>
#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>
#include <core/kexipart.h>
#include <core/keximainwindow.h>

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
			KexiMainWindow* const mainwin;
			Private(KexiMainWindow* const mainwin)
				: mainwin(mainwin)
			{
			}
	};

	class NameVariable : public KoMacro::GenericVariable<NameVariable>
	{
		public:
			explicit NameVariable(KexiMainWindow* const mainwin, KoMacro::Action::Ptr action, const QString& objectname = QString::null)
				: KoMacro::GenericVariable<NameVariable>("name", i18n("Name"), action)
			{
				if(! mainwin->project()) {
					kdWarning() << "KexiMacro::NameVariable() No project loaded." << endl;
					return;
				}

				QString name;
				KexiPart::PartInfoList* parts = Kexi::partManager().partInfoList();
				for(KexiPart::PartInfoListIterator it(*parts); it.current(); ++it) {
					KexiPart::Info* info = it.current();
					if(! info->isVisibleInNavigator() || objectname != info->objectName()) {
						continue;
					}

					KexiPart::ItemDict* items = mainwin->project()->items(info);
					if(! items) {
						break;
					}

					KexiPart::ItemDictIterator item_it(*items);
					for(; item_it.current(); ++item_it) {
						//const QString name = (*item_it.current())->name();
						const QString n = item_it.current()->name();
						children().append( KoMacro::Variable::Ptr(new KoMacro::Variable(n)) );
						if(name.isNull()) {
							name = n;
						}
						kdDebug() << "KexiMacro::NameVariable() name=" << n << endl;
					}
				
					/*
					bool openingCancelled;
					if(! mainwin->openObject(item, Kexi::DataViewMode, openingCancelled) && !openingCancelled)
						KMessageBox::error(m_win, i18n("Specified document could not be opened."));
					*/

					break; // job is done.
				}

				if(name.isNull()) {
					children().append( KoMacro::Variable::Ptr(new KoMacro::Variable("")) );
				}

				setVariant(name);
			}

			virtual ~NameVariable() {}

	};

}

OpenObject::OpenObject(KexiMainWindow* const mainwin)
	: KoMacro::GenericAction<OpenObject>("openobject", i18n("Open Object"))
	, d( new Private(mainwin) )
{
	ObjectVariable* objvar = new ObjectVariable(this);
	setVariable(KoMacro::Variable::Ptr( objvar ));

	setVariable(KoMacro::Variable::Ptr( new NameVariable(d->mainwin, this, objvar->variant().toString()) ));
}

OpenObject::~OpenObject() 
{
	delete d;
}

KoMacro::Variable::List OpenObject::notifyUpdated(KoMacro::Variable::Ptr variable)
{
	kdDebug()<<"OpenObject::notifyUpdated() name="<<variable->name()<<" value="<< variable->variant().toString() <<endl;

	KoMacro::Variable::List list;
	if(variable->name() == "object") {
		const QString objectname = variable->variant().toString(); // e.g. "table" or "query"
		//list.append( KoMacro::Variable::Ptr(new ObjectVariable(this, objectname)) );
		list.append( KoMacro::Variable::Ptr(new NameVariable(d->mainwin, this, objectname)) );
	}

	return list;
}

void OpenObject::activate(KoMacro::Context::Ptr context)
{
	Q_UNUSED(context);
	kdDebug() << "OpenObject::activate(Context::Ptr)" << endl;
	//TODO
}
