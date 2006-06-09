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
#include "objectvariable.h"

#include "../lib/macroitem.h"
#include "../lib/context.h"

#include "objectnamevariable.h"

#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>
#include <core/kexipart.h>
#include <core/keximainwindow.h>

#include <klocale.h>

using namespace KexiMacro;

ExecuteAction::ExecuteAction()
	: KexiAction("execute", i18n("Execute"))
{
	int conditions = ObjectVariable<ExecuteAction>::VisibleInNav | ObjectVariable<ExecuteAction>::Executable;
	KoMacro::Variable* objvar = new ObjectVariable<ExecuteAction>(this, conditions);
	setVariable(KSharedPtr<KoMacro::Variable>( objvar ));

	setVariable(KSharedPtr<KoMacro::Variable>( new ObjectNameVariable<ExecuteAction>(this, objvar->variant().toString()) ));
}

ExecuteAction::~ExecuteAction() 
{
}

#if 0
KoMacro::Variable::List ExecuteAction::notifyUpdated(const QString& variablename, KoMacro::Variable::Map variablemap)
{
	Q_UNUSED(variablename);
	Q_UNUSED(variablemap);
	//kdDebug()<<"OpenObject::ExecuteAction() name="<<variable->name()<<" value="<< variable->variant().toString() <<endl;
	KoMacro::Variable::List list;

	if(variablename == "object") {
		const QString objectname = variablemap["object"]->variant().toString(); // e.g. "table" or "query"

		const QString name = variablemap.contains("name") ? variablemap["name"]->variant().toString() : QString::null;
		list.append( KSharedPtr<KoMacro::Variable>(new ObjectNameVariable<ExecuteAction>(this, objectname, name)) );
	}

	return list;
}
#endif

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
