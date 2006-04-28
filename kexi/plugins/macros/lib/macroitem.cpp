/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
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

#include "macroitem.h"

#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class MacroItem::Private
	{
		public:
			Action::Ptr action;
			QString comment;
			Variable::Map variables;
	};

}

MacroItem::MacroItem()
	: QObject()
	, KShared()
	, d( new Private() )
{
}

MacroItem::~MacroItem()
{
	delete d;
}

QString MacroItem::comment() const
{
	return d->comment;
}

void MacroItem::setComment(const QString& comment)
{
	d->comment = comment;
}

KSharedPtr<Action> MacroItem::action() const
{
	return d->action;
}

void MacroItem::setAction(KSharedPtr<Action> action)
{
	d->action = action;
}

Variable::Ptr MacroItem::variable(const QString& name, bool checkaction) const
{
	if(d->variables.contains(name))
		return d->variables[name];
	if(checkaction && d->action.data())
		return d->action->variable(name);
	return Variable::Ptr(0);
}

Variable::Map MacroItem::variables() const
{
	return d->variables;
}

QStringList MacroItem::setVariable(const QString& name, Variable::Ptr variable)
{
	// First try to find the matching in the action defined variable.
	Variable* v = d->action->variable(name).data();
	if(! v) {
		/*
		// The name isn't known, try to fallback to the name the variable defines.
		v = d->action->variable( variable->name() );
		if(! v.data()) {
		*/
		kdDebug() << QString("MacroItem::setVariable() No such variable \"%1\"").arg(name) << endl;
		return QStringList();
	}

	// Check if the variable is valid.
	if(! v->validVariable(variable)) {
		kdDebug() << QString("MacroItem::setVariable() update for variable \"%1\" failed.").arg(name) 	<< endl;
		return QStringList();
	}

	kdDebug() << "MacroItem::setVariable() name=" << name << " variable=" << variable->variant().toString() << endl;

	// remember the new variable.
	d->variables.replace(name, variable);

	// Notify the variable, that we updated it.
	//v->updated(this);

	// set depending variables by asking the own action.
	QStringList sl;
	Variable::List list = d->action->notifyUpdated(name, d->variables);
	Variable::List::Iterator it(list.begin()), end(list.end());
	for (; it != end; ++it) {
		const QString n = (*it)->name();;
		sl << n;

		//if( ! d->variables.contains(n) ) {
		kdDebug()<<"    name=" << n << " value=" << (*it)->variant().toString() << endl;
		//setVariable(n, *it);
		d->variables.replace(n, *it);
		//}

		/*
		Variable::Ptr v = d->variables[ (*it)->name() ];
		if(v.data() && (*it)->type() == v->type() && ! v->variant().isNull()) {
			(*it)->setVariant( v->variant().toString() );
		}
		*/
	}

	return sl;
}

#include "macroitem.moc"
