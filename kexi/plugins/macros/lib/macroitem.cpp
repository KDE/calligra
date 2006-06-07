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
			KSharedPtr<Action> action;
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

QVariant MacroItem::variant(const QString& name, bool checkaction) const
{
	KSharedPtr<Variable> v = variable(name, checkaction);
	return v.data() ? v->variant() : QVariant();
}

KSharedPtr<Variable> MacroItem::variable(const QString& name, bool checkaction) const
{
	if(d->variables.contains(name))
		return d->variables[name];
	if(checkaction && d->action.data())
		return d->action->variable(name);
	return KSharedPtr<Variable>(0);
}

Variable::Map MacroItem::variables() const
{
	return d->variables;
}

bool MacroItem::setVariant(const QString& name, const QVariant& variant)
{
	// The QVariant v holds our new variant value while the variant itself
	// contains the original value.
	QVariant v = variant;

	// Let's look if there is an action defined for the variable. If that's
	// the case, we try to use that action to preserve the type of the variant.
	KSharedPtr<Variable> actionvariable = d->action ? d->action->variable(name) : KSharedPtr<Variable>(0);
	if(actionvariable.data()) {
		const QVariant var = actionvariable->variant();

		// We can't use QVariant::canCast() cause it returns "false positives"
		// for e.g. integers. So, let's check explicit if casting is possible.
		bool ok = true;
		switch( var.type() ) {
			case QVariant::Bool: {
				const QString s = variant.toString();
				ok = (s == "true" || s == "false" || s == "0" || s == "1" || s == "-1");
				v = QVariant( variant.toBool(), 0 );
				// kdDebug() << "parse an bool " << v << endl;
			} break;
			case QVariant::Int: {
				v = variant.toInt(&ok);
				Q_ASSERT(!ok || v.toString() == variant.toString());
			} break;
			case QVariant::UInt: 
				v = variant.toUInt(&ok); 
				Q_ASSERT(!ok || v.toString() == variant.toString());
				break;
			case QVariant::LongLong: 
				v = variant.toLongLong(&ok); 
				Q_ASSERT(!ok || v.toString() == variant.toString());
				break;
			case QVariant::ULongLong: 
				v = variant.toULongLong(&ok); 
				Q_ASSERT(!ok || v.toString() == variant.toString());
				break;
			case QVariant::Double: 
				v = variant.toDouble(&ok);
				Q_ASSERT(!ok || v.toString() == variant.toString());
				break;
			case QVariant::String: 
				v = variant.toString();
				break;
			default: {
				ok = v.cast( var.type() );
				kdWarning()<<"MacroItem::setVariable() Unhandled type="<<var.type()<<" value="<<v<<endl;
			} break;
		}
		// If cast fails, the variant will be automatically handled as QString.
		if(! ok)
			v = variant;
	}

	KSharedPtr<Variable> variable = d->variables[name];
	if(! variable.data()) {
		// if there exists no such variable yet, create one.
		kdDebug() << "MacroItem::setVariable() Creating new variable name=" << name << endl;

		variable = KSharedPtr<Variable>( new Variable() );
		variable->setName(name);
		d->variables.replace(name, variable);
	}

	const QVariant oldvar = variable->variant();
	//if(oldvar == v) return false; // check if an update is really needed
	variable->setVariant(v);

	if(! d->action->notifyUpdated(this, name)) {
		kdWarning() << "MacroItem::setVariable() Notify failed for variable name=" << name << endl;
		variable->setVariant(oldvar);
		return false;
	}

	return true;
}

bool MacroItem::setVariable(const QString& name, const QVariant& variant)
{
	return setVariant(name, variant);
}

KSharedPtr<Variable> MacroItem::addVariable(const QString& name, const QVariant& value)
{
	Q_ASSERT(! d->variables.contains(name) );
	KSharedPtr<Variable> variable = KSharedPtr<Variable>( new Variable() );
	variable->setName(name);
	d->variables.replace(name, variable);
	this->setVariant(name, value);
	return variable;
}

#if 0
QStringList MacroItem::setVariable(const QString& name, KSharedPtr<Variable> variable)
{
	// First try to find the matching in the action defined variable.
	Variable* v = d->action ? d->action->variable(name).data() : 0;
	if(! v) {
		/*
		// The name isn't known, try to fallback to the name the variable defines.
		v = d->action->variable( variable->name() );
		if(! v.data()) {
		*/
		kdWarning() << QString("MacroItem::setVariable() No such variable \"%1\"").arg(name) << endl;
		return QStringList();
	}

	// Check if the variable is valid.
	if(! v->validVariable(variable)) {
		kdWarning() << QString("MacroItem::setVariable() update for variable \"%1\" failed.").arg(name) 	<< endl;
		return QStringList();
	}
	kdDebug() << "MacroItem::setVariable() name=" << name << " variable=" << variable->variant().toString() << endl;

//TODO
	// remember the new variable.
	d->variables.replace(name, variable);

	// notify the action that we updated a variable.
	return d->action->notifyUpdated(name, this);

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
		KSharedPtr<Variable> v = d->variables[ (*it)->name() ];
		if(v.data() && (*it)->type() == v->type() && ! v->variant().isNull()) {
			(*it)->setVariant( v->variant().toString() );
		}
		*/
	}
	return sl;
}
#endif

#include "macroitem.moc"
