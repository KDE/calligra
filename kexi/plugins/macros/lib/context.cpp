/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2005 by Tobi Krebs (tobi.krebs@gmail.com)
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

#include "context.h"
#include "action.h"
#include "macro.h"
#include "macroitem.h"
#include "exception.h"

//#include <qtimer.h>
#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class Context::Private
	{
		public:

			/**
			* The @a Macro instance that owns this @a Context .
			*/
			Macro::Ptr macro;

			/**
			* List of @a Action instances that are children of the
			* macro.
			*/
			QValueList<MacroItem::Ptr> items;

			/**
			* The currently selected @a MacroItem or NULL if there
			* is now @a MacroItem selected yet.
			*/
			MacroItem::Ptr macroitem;

			/**
			* Map of all @a Variable instance that are defined within
			* this context.
			*/
			QMap<QString, Variable::Ptr> variables;

			/**
			* The @a Exception instance thrown at the last @a activate()
			* call or NULL if there was no exception thrown yet.
			*/
			Exception* exception;

			/// Constructor.
			explicit Private(Macro::Ptr m)
				: macro(m) // remember the macro
				, items(m->items()) // set d-pointer children to macro children
				, exception(0) // no exception yet.
			{
			}

			/// Destructor.
			~Private()
			{
				delete exception;
			}
	};

}
//Construktor with initialization of our d-pointer
Context::Context(Macro::Ptr macro)
	: QObject()
	, d( new Private(macro) ) // create the private d-pointer instance.
{
}

Context::~Context()
{
	delete d;
}

//return if we have variables
bool Context::hasVariable(const QString& name) const
{
	return d->variables.contains(name);
}

//return variable with name or throw an exception if none is found
Variable::Ptr Context::variable(const QString& name) const
{
	if (d->variables.contains(name)) {
		return d->variables[name];
	}
	if(d->macroitem.data()) {
		Variable::Ptr v = d->macroitem->variable(name, true);
		if(v.data()) {
			return v;
		}
	}
	throw Exception(QString("Variable name='%1' does not exist.").arg(name), QString("Komacro::Context::variable"));
}

//return a map of our variables
Variable::Map Context::variables() const
{
	return d->variables;
}

//set a variable
void Context::setVariable(const QString& name, Variable::Ptr variable)
{
	kdDebug() << QString("KoMacro::Context::setVariable name='%1' variable='%2'").arg(name).arg(variable->toString()) << endl;
	d->variables.replace(name, variable);
}

//return the associated Macro
Macro::Ptr Context::macro() const
{
	return d->macro;
}

// //return the currently selected MacroItem
MacroItem::Ptr Context::macroItem() const
{
	return d->macroitem;
}

bool Context::hadException() const
{
	return d->exception != 0;
}

Exception* Context::exception() const
{
	return d->exception;
}

//activate an action if there is one
void Context::activate(QValueList<MacroItem::Ptr>::ConstIterator it)
{
	kdDebug() << "Context::activate()" << endl;
	//Q_ASSIGN(d->macro);

	QValueList<MacroItem::Ptr>::ConstIterator end(d->items.constEnd());
	for(;it != end; ++it) {
		// fetch the MacroItem we are currently pointing to.
		d->macroitem = MacroItem::Ptr(*it);
		if(! d->macroitem.data()) {
			kdDebug() << "Context::activate() Skipping empty MacroItem" << endl;
			continue;
		}

		// fetch the Action, the MacroItem points to.
		Action::Ptr action = d->macroitem->action();
		if(! action.data()) {
			kdDebug() << "Context::activate() Skipping MacroItem with no action" << endl;
			continue;
		}

		try {
			// activate the action
			action->activate(this);
		}
		catch(Exception& e) {
			d->exception = new Exception(e);
			d->exception->addTraceMessage( QString("macro=%1").arg(d->macro->name()) );
			d->exception->addTraceMessage( QString("action=%1").arg(action->name()) );
			QStringList variables = action->variableNames();
			for(QStringList::Iterator vit = variables.begin(); vit != variables.end(); ++vit) {
				Variable::Ptr v = d->macroitem->variable(*vit, true);
				d->exception->addTraceMessage( QString("%1=%2").arg(*vit).arg(v->toString()) );
			}
			return; // abort execution
		}
	}

	// The run is done. So, let's remove the currently selected item to
	// outline, that we did the job and there stays no dangling item.
	d->macroitem = MacroItem::Ptr(0);
}

void Context::activate(Context::Ptr context)
{
	delete d->exception; d->exception = 0;
	if(context->hadException()) {
		// if the context this context should run in had an exception,
		// we adopt this exception and abort the execution.
		d->exception = new Exception( *context->exception() );
		return;
	}

	// Merge the passed context into this context
	Variable::Map variables = context->variables();
	//copy variables
	Variable::Map::ConstIterator it, end( variables.constEnd() );
	for( it = variables.constBegin(); it != end; ++it)
		setVariable(it.key(), it.data());
	
	//activate copied context.
	activate(d->items.constBegin());
}

void Context::activateNext()
{	
	delete d->exception; d->exception = 0;

	if(! d->macroitem) { // if no MacroItem is defined, we don't need to try to continue execution
		return;
	}

	QValueList<MacroItem::Ptr>::ConstIterator it = d->items.find(d->macroitem);
	if (it != d->items.constEnd()) {
		activate(++it); // try to continue the execution.
	}
}

#include "context.moc"
