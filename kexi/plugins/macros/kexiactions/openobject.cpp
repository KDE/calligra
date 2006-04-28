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

#include "../lib/macroitem.h"
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

	/**
	* The ViewVariable class provide a list of viewmodes supported
	* by a KexiPart::Part as @a KoMacro::Variable .
	*/
	class ViewVariable : public KoMacro::GenericVariable<ViewVariable>
	{
		public:
			explicit ViewVariable(KexiMainWindow* const mainwin, KoMacro::Action::Ptr action, const QString& objectname = QString::null, const QString& viewname = QString::null)
				: KoMacro::GenericVariable<ViewVariable>("view", i18n("View"), action)
			{
				Q_UNUSED(mainwin);
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
						children().append( KoMacro::Variable::Ptr(new KoMacro::Variable(*it)) );
				}
				QString n = viewname;
				if(n.isNull() || ! namelist.contains(n))
					n = namelist.count() > 0 ? namelist[0] : "";
				setVariant(n);
			}
	};

	/**
	* The ViewVariable class provide a list of KexiPart::PartItem's
	* supported by a KexiPart::Part as @a KoMacro::Variable .
	*/
	class NameVariable : public KoMacro::GenericVariable<NameVariable>
	{
		public:
			explicit NameVariable(KexiMainWindow* const mainwin, KoMacro::Action::Ptr action, const QString& objectname = QString::null, const QString& name = QString::null)
				: KoMacro::GenericVariable<NameVariable>("name", i18n("Name"), action)
			{
				if(! mainwin->project()) {
					kdWarning() << "KexiMacro::NameVariable() No project loaded." << endl;
					return;
				}

				QStringList namelist;
				KexiPart::Info* info = Kexi::partManager().infoForMimeType( QString("kexi/%1").arg(objectname) );
				if(info) {
					if(info->isVisibleInNavigator()) {
						KexiPart::ItemDict* items = mainwin->project()->items(info);
						if(items) {
							for(KexiPart::ItemDictIterator item_it = *items; item_it.current(); ++item_it) {
								const QString n = item_it.current()->name();
								namelist << n;
								children().append( KoMacro::Variable::Ptr(new KoMacro::Variable(n)) );
								kdDebug() << "KexiMacro::NameVariable() infoname=" << info->objectName() << " name=" << n << endl;
							}
						}
					}
				}

				if(namelist.count() <= 0) {
					namelist << "";
					children().append( KoMacro::Variable::Ptr(new KoMacro::Variable("")) );
				}

				QString n = (name.isNull() || ! namelist.contains(name)) ? namelist[0] : name;
				kdDebug() << "KexiMacro::NameVariable() name=" << n << " childcount=" << children().count() << endl;
				setVariant(n);
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

	setVariable(KoMacro::Variable::Ptr( new ViewVariable(d->mainwin, this, objvar->variant().toString()) ));

	setVariable(KoMacro::Variable::Ptr( new NameVariable(d->mainwin, this, objvar->variant().toString()) ));
}

OpenObject::~OpenObject() 
{
	delete d;
}

KoMacro::Variable::List OpenObject::notifyUpdated(const QString& variablename, KoMacro::Variable::Map variablemap)
{
	//kdDebug()<<"OpenObject::notifyUpdated() name="<<variable->name()<<" value="<< variable->variant().toString() <<endl;

	KoMacro::Variable::List list;
	if(variablename == "object") {
		const QString objectname = variablemap["object"]->variant().toString(); // e.g. "table" or "query"

		const QString name = variablemap.contains("name") ? variablemap["name"]->variant().toString() : QString::null;
		list.append( KoMacro::Variable::Ptr(new NameVariable(d->mainwin, this, objectname, name)) );

		const QString viewname = variablemap.contains("view") ? variablemap["view"]->variant().toString() : QString::null;
		list.append( KoMacro::Variable::Ptr(new ViewVariable(d->mainwin, this, objectname, viewname)) );
	}

	return list;
}

void OpenObject::activate(KoMacro::Context::Ptr context)
{
	if(! d->mainwin->project()) {
		kdWarning() << "OpenObject::activate(KoMacro::Context::Ptr) Invalid project" << endl;
		return;
	}

	const QString objectname = context->variable("object")->variant().toString();
	const QString name = context->variable("name")->variant().toString();
	KexiPart::Item* item = d->mainwin->project()->itemForMimeType( QString("kexi/%1").arg(objectname).latin1(), name );
	if(! item) {
		kdWarning() << "OpenObject::activate(KoMacro::Context::Ptr) Invalid item objectname=" << objectname << " name=" << name << endl;
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
		kdWarning() << "OpenObject::activate(KoMacro::Context::Ptr) Invalid viewmode=" << view << endl;
		return;
	}

	// Try to open the object now.
	bool openingCancelled;
	if(! d->mainwin->openObject(item, viewmode, openingCancelled)) {
		if(! openingCancelled) {
			KMessageBox::error(d->mainwin, i18n("Object of type \"%1\" with name \"%2\" could not be opened.").arg(objectname).arg(name));
		}
	}
}
