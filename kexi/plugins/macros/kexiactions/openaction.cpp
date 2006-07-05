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

#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>
#include <core/kexipart.h>
#include <core/keximainwindow.h>

#include <klocale.h>

using namespace KexiMacro;

namespace KexiMacro {

	static const QString DATAVIEW = "data";
	static const QString DESIGNVIEW = "design";
	static const QString TEXTVIEW = "text";
	
	static const QString OBJECT = "object";
	static const QString NAME = "name";
	static const QString VIEW = "view";

	/**
	* The ViewVariable class provide a list of viewmodes supported
	* by a KexiPart::Part as @a KoMacro::Variable .
	*/
	template<class ACTIONIMPL>
	class ViewVariable : public KexiVariable<ACTIONIMPL>
	{
		public:
			ViewVariable(ACTIONIMPL* actionimpl, const QString& objectname = QString::null, const QString& viewname = QString::null)
				: KexiVariable<ACTIONIMPL>(actionimpl, VIEW, i18n("View"))
			{
				QStringList namelist;
				KexiPart::Part* part = Kexi::partManager().partForMimeType( QString("kexi/%1").arg(objectname) );
				if(part) {
					int viewmodes = part->supportedViewModes();
					if(viewmodes & Kexi::DataViewMode)
						namelist << DATAVIEW;
					if(viewmodes & Kexi::DesignViewMode)
						namelist << DESIGNVIEW;
					if(viewmodes & Kexi::TextViewMode)
						namelist << TEXTVIEW;
					for(QStringList::Iterator it = namelist.begin(); it != namelist.end(); ++it)
						this->children().append( KSharedPtr<KoMacro::Variable>(new KoMacro::Variable(*it)) );
				}
				const QString n =
					namelist.contains(viewname)
						? viewname
						: namelist.count() > 0 ? namelist[0] : "";

				this->setVariant(n);
			}
	};

}

OpenAction::OpenAction()
	: KexiAction("open", i18n("Open"))
{
	const int conditions = ObjectVariable<OpenAction>::VisibleInNav;
	
	KSharedPtr<KoMacro::Variable> objvar = new ObjectVariable<OpenAction>(this, conditions);
	setVariable(objvar);

	setVariable(KSharedPtr<KoMacro::Variable>( new ObjectNameVariable<OpenAction>(this, objvar->variant().toString()) ));
	setVariable(KSharedPtr<KoMacro::Variable>( new ViewVariable<OpenAction>(this, objvar->variant().toString()) ));
}

OpenAction::~OpenAction()
{
}

bool OpenAction::notifyUpdated(KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name)
{
	kdDebug()<<"OpenAction::notifyUpdated() name="<<name<<" macroitem.action="<<(macroitem->action() ? macroitem->action()->name() : "NOACTION")<<endl;

	KSharedPtr<KoMacro::Variable> variable = macroitem->variable(name, true);
	if(! variable) {
		kdWarning()<<"OpenAction::notifyUpdated() No such variable="<<name<<" in macroitem."<<endl;
		return false;
	}

	variable->children().clear();
	if(name == OBJECT) {
		const QString objectvalue = macroitem->variant(OBJECT, true).toString(); // e.g. "table" or "query"
		const QString objectname = macroitem->variant(NAME, true).toString();
		const QString viewname = macroitem->variant(VIEW, true).toString();

		kdDebug()<<"OpenAction::notifyUpdated() objectvalue="<<objectvalue<<" objectname="<<objectname<<" viewname="<<viewname<<endl;

		variable->children().append(
			KSharedPtr<KoMacro::Variable>(new ObjectNameVariable<OpenAction>(this, objectvalue, objectname))
		);
		variable->children().append(
			KSharedPtr<KoMacro::Variable>(new ViewVariable<OpenAction>(this, objectvalue, viewname))
		);
	}

	return true;
}

void OpenAction::activate(KSharedPtr<KoMacro::Context> context)
{
	if(! mainWin()->project()) {
		throw KoMacro::Exception(i18n("No project loaded."));
	}

	const QString objectname = context->variable(OBJECT)->variant().toString();
	const QString name = context->variable(NAME)->variant().toString();
	KexiPart::Item* item = mainWin()->project()->itemForMimeType( QString("kexi/%1").arg(objectname).latin1(), name );
	if(! item) {
		throw KoMacro::Exception(i18n("No such object \"%1.%2\".").arg(objectname).arg(name));
	}

	// Determinate the viewmode.
	const QString view = context->variable(VIEW)->variant().toString();
	int viewmode;
	if(view == DATAVIEW)
		viewmode = Kexi::DataViewMode;
	else if(view == DESIGNVIEW)
		viewmode = Kexi::DesignViewMode;
	else if(view == TEXTVIEW)
		viewmode = Kexi::TextViewMode;
	else {
		throw KoMacro::Exception(i18n("No such viewmode \"%1\" in object \"%2.%3\".").arg(view).arg(objectname).arg(name));
	}

	// Try to open the object now.
	bool openingCancelled;
	if(! mainWin()->openObject(item, viewmode, openingCancelled)) {
		if(! openingCancelled) {
			throw KoMacro::Exception(i18n("Failed to open object \"%1.%2\".").arg(objectname).arg(name));
		}
	}
}

//#include "openaction.moc"
