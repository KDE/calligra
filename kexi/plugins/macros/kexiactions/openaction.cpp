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

#include "openaction.h"
#include "objectvariable.h"
#include "objectnamevariable.h"

#include "../lib/macroitem.h"
#include "../lib/context.h"

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
	* The ViewVariable class provide a list of viewmodes supported
	* by a KexiPart::Part as @a KoMacro::Variable .
	*/
	template<class ACTIONIMPL>
	class ViewVariable : public KexiVariable<ACTIONIMPL>
	{
		public:
			ViewVariable(ACTIONIMPL* actionimpl, const QString& objectname = QString::null, const QString& viewname = QString::null)
				: KexiVariable<ACTIONIMPL>(actionimpl, "view", i18n("View"))
			{
				QStringList namelist;
				KexiPart::Part* part = Kexi::partManager().partForMimeType( QString("kexi/%1").arg(objectname) );
				if(part) {
					int viewmodes = part->supportedViewModes();
					if(viewmodes & Kexi::DataViewMode)
						namelist << "data";
					if(viewmodes & Kexi::DesignViewMode)
						namelist << "design";
					if(viewmodes & Kexi::TextViewMode)
						namelist << "text";
					for(QStringList::Iterator it = namelist.begin(); it != namelist.end(); ++it)
						this->children().append( KoMacro::Variable::Ptr(new KoMacro::Variable(*it)) );
				}
				QString n = viewname;
				if(n.isNull() || ! namelist.contains(n))
					n = namelist.count() > 0 ? namelist[0] : "";
				this->setVariant(n);
			}
	};

}

OpenAction::OpenAction()
	: KexiAction("open", i18n("Open"))
{
	int conditions = ObjectVariable<OpenAction>::VisibleInNav;
	KoMacro::Variable* objvar = new ObjectVariable<OpenAction>(this, conditions);
	setVariable(KoMacro::Variable::Ptr( objvar ));

	setVariable(KoMacro::Variable::Ptr( new ObjectNameVariable<OpenAction>(this, objvar->variant().toString()) ));
	setVariable(KoMacro::Variable::Ptr( new ViewVariable<OpenAction>(this, objvar->variant().toString()) ));

	/*TODO
	KoMacro::Variable::Ptr activatevar = KoMacro::Variable::Ptr( new KexiVariable<OpenAction>(this, "activate", i18n("Activate")) );
	activatevar->setVariant( QVariant(true,0) );
	setVariable(activatevar);
	*/
}

OpenAction::~OpenAction()
{
}

KoMacro::Variable::List OpenAction::notifyUpdated(const QString& variablename, KoMacro::Variable::Map variablemap)
{
	//kdDebug()<<"OpenAction::notifyUpdated() name="<<variable->name()<<" value="<< variable->variant().toString() <<endl;

	KoMacro::Variable::List list;
	if(variablename == "object") {
		const QString objectname = variablemap["object"]->variant().toString(); // e.g. "table" or "query"

		const QString name = variablemap.contains("name") ? variablemap["name"]->variant().toString() : QString::null;
		list.append( KoMacro::Variable::Ptr(new ObjectNameVariable<OpenAction>(this, objectname, name)) );

		const QString viewname = variablemap.contains("view") ? variablemap["view"]->variant().toString() : QString::null;
		list.append( KoMacro::Variable::Ptr(new ViewVariable<OpenAction>(this, objectname, viewname)) );
	}

	return list;
}

void OpenAction::activate(KoMacro::Context::Ptr context)
{
	if(! mainWin()->project()) {
		kdWarning() << "OpenAction::activate(KoMacro::Context::Ptr) Invalid project" << endl;
		return;
	}

	const QString objectname = context->variable("object")->variant().toString();
	const QString name = context->variable("name")->variant().toString();
	KexiPart::Item* item = mainWin()->project()->itemForMimeType( QString("kexi/%1").arg(objectname).latin1(), name );
	if(! item) {
		kdWarning() << "OpenAction::activate(KoMacro::Context::Ptr) Invalid item objectname=" << objectname << " name=" << name << endl;
		return;
	}

	// Determinate the viewmode.
	const QString view = context->variable("view")->variant().toString();
	int viewmode;
	if(view == "data")
		viewmode = Kexi::DataViewMode;
	else if(view == "design")
		viewmode = Kexi::DesignViewMode;
	else if(view == "text")
		viewmode = Kexi::TextViewMode;
	else {
		kdWarning() << "OpenAction::activate(KoMacro::Context::Ptr) Invalid viewmode=" << view << endl;
		return;
	}

	// Try to open the object now.
	bool openingCancelled;
	if(! mainWin()->openObject(item, viewmode, openingCancelled)) {
		if(! openingCancelled) {
			KMessageBox::error(mainWin(), i18n("Object of type \"%1\" with name \"%2\" could not be opened.").arg(objectname).arg(name));
		}
	}
}

#include "openaction.moc"
