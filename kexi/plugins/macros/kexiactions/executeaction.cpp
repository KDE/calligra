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

#include <kexi_export.h>
#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>
#include <core/kexipart.h>
#include <core/keximainwindow.h>

#include <klocale.h>

using namespace KexiMacro;

ExecuteAction::ExecuteAction()
	: KexiAction("executeaction", i18n("Execute"))
{
/*
	ObjectVariable* objvar = new ObjectVariable(this);
	setVariable(KoMacro::Variable::Ptr( objvar ));

	setVariable(KoMacro::Variable::Ptr( new ViewVariable(d->mainwin, this, objvar->variant().toString()) ));
	setVariable(KoMacro::Variable::Ptr( new NameVariable(d->mainwin, this, objvar->variant().toString()) ));
*/
}

ExecuteAction::~ExecuteAction() 
{
}

KoMacro::Variable::List ExecuteAction::notifyUpdated(const QString& variablename, KoMacro::Variable::Map variablemap)
{
	Q_UNUSED(variablename);
	Q_UNUSED(variablemap);
	//kdDebug()<<"OpenObject::ExecuteAction() name="<<variable->name()<<" value="<< variable->variant().toString() <<endl;
	KoMacro::Variable::List list;
	return list;
}

void ExecuteAction::activate(KoMacro::Context::Ptr context)
{
	//TODO
}

//#include "executeaction.moc"
