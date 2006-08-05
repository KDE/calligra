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

#include "executeaction.h"

#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>
#include <core/kexipart.h>
#include <core/keximainwindow.h>

#include <klocale.h>

using namespace KexiMacro;

namespace KexiMacro {
	static const QString OBJECT = "object";
	static const QString NAME = "name";
}

ExecuteAction::ExecuteAction()
	: KexiAction("execute", i18n("Execute"))
{
	int conditions = ObjectVariable<ExecuteAction>::VisibleInNav | ObjectVariable<ExecuteAction>::Executable;
	KSharedPtr<KoMacro::Variable> objvar = new ObjectVariable<ExecuteAction>(this, conditions);
	setVariable(objvar);

	setVariable(KSharedPtr<KoMacro::Variable>( new ObjectNameVariable<ExecuteAction>(this, objvar->variant().toString()) ));
}

ExecuteAction::~ExecuteAction() 
{
}

bool ExecuteAction::notifyUpdated(KSharedPtr<KoMacro::MacroItem> macroitem, const QString& name)
{
	kdDebug()<<"ExecuteAction::notifyUpdated() name="<<name<<" macroitem.action="<<(macroitem->action() ? macroitem->action()->name() : "NOACTION")<<endl;
	KSharedPtr<KoMacro::Variable> variable = macroitem->variable(name, false);
	if(! variable) {
		kdWarning()<<"ExecuteAction::notifyUpdated() No such variable="<<name<<" in macroitem."<<endl;
		return false;
	}

	variable->clearChildren();
	if(name == OBJECT) {
		const QString objectvalue = macroitem->variant(OBJECT, true).toString(); // e.g. "macro" or "script"
		const QString objectname = macroitem->variant(NAME, true).toString(); // e.g. "macro1" or "macro2" if objectvalue above is "macro"
		macroitem->variable(NAME, true)->setChildren(
			KoMacro::Variable::List() << KSharedPtr<KoMacro::Variable>(new ObjectNameVariable<ExecuteAction>(this, objectvalue, objectname)) );
	}

	return true;
}

void ExecuteAction::activate(KSharedPtr<KoMacro::Context> context)
{
	if(! mainWin()->project()) {
		kdWarning() << "ExecuteAction::activate(KSharedPtr<KoMacro::Context>) Invalid project" << endl;
		return;
	}

	const QString mimetype = QString("kexi/%1").arg( context->variable("object")->variant().toString() );
	const QString name = context->variable("name")->variant().toString();

	KexiPart::Part* part = Kexi::partManager().partForMimeType(mimetype);
	if(! part) {
		throw KoMacro::Exception(i18n("No such mimetype \"%1\"").arg(mimetype));
	}

	KexiPart::Item* item = mainWin()->project()->item(part->info(), name);
	if(! item) {
		throw KoMacro::Exception(i18n("Failed to open part \"%1\" for mimetype \"%2\"").arg(name).arg(mimetype));
	}

	part->execute(item);
}

//#include "executeaction.moc"
