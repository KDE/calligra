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
			KSharedPtr<Macro> macro;

			/**
			* List of @a Action instances that are children of the
			* macro.
			*/
			QValueList<KSharedPtr<MacroItem > > items;

			/**
			* The currently selected @a MacroItem or NULL if there
			* is now @a MacroItem selected yet.
			*/
			KSharedPtr<MacroItem> macroitem;

			/**
			* Map of all @a Variable instance that are defined within
			* this context.
			*/
			QMap<QString, KSharedPtr<Variable > > variables;

			/**
			* The @a Exception instance thrown at the last @a activate()
			* call or NULL if there was no exception thrown yet.
			*/
			Exception* exception;

			/// Constructor.
			explicit Private(KSharedPtr<Macro> m)
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
//Constructor with initialization of our Private.object (d-pointer)
Context::Context(KSharedPtr<Macro> macro)
	: QObject()
	, d( new Private(macro) ) // create the private d-pointer instance.
{
}

//Destructor.
Context::~Context()
{
	delete d;
}

//return if we have (d-pointer) variables
bool Context::hasVariable(const QString& name) const
{
	//Use QMap?s contains to check if a variable with name exists
	return d->variables.contains(name);
}

//return variable with name or throw an exception if none is found in variables
KSharedPtr<Variable> Context::variable(const QString& name) const
{
	//Use QMap?s contains to check if a variable with name exists in context	
	if (d->variables.contains(name)) {
		//return it
		return d->variables[name];
	}
	//if there is a macroitem try to get variable from it 
	if(d->macroitem.data()) {
		KSharedPtr<Variable> v = d->macroitem->variable(name, true);
		if(v.data()) {
			return v;
		}
	}
	//none found throw exception
	throw Exception(QString("Variable name='%1' does not exist.").arg(name));
}

//return a map of our (d-pointer) variables
Variable::Map Context::variables() const
{
	return d->variables;
}

//set a variable
void Context::setVariable(const QString& name, KSharedPtr<Variable> variable)
{
	//debuging infos
	kdDebug() << QString("KoMacro::Context::setVariable name='%1' variable='%2'").arg(name).arg(variable->toString()) << endl;
	//Use QMap?s replace to set/replace the variable named name
	d->variables.replace(name, variable);
}

//return the associated Macro
KSharedPtr<Macro> Context::macro() const
{
	return d->macro;
}

//return the currently selected MacroItem
KSharedPtr<MacroItem> Context::macroItem() const
{
	return d->macroitem;
}

//return if this context had an exception
bool Context::hadException() const
{
	return d->exception != 0;
}

//return the (d-pointer) exception
Exception* Context::exception() const
{
	return d->exception;
}

//try to activate all action?s in this context
void Context::activate(QValueList<KSharedPtr<MacroItem > >::ConstIterator it)
{
	//debuging infos
	kdDebug() << "Context::activate()" << endl;
	//Q_ASSIGN(d->macro);

	//set end to constEnd
	QValueList<KSharedPtr<MacroItem > >::ConstIterator end(d->items.constEnd());
	//loop through actions
	for(;it != end; ++it) {
		// fetch the MacroItem we are currently pointing to.
		d->macroitem = KSharedPtr<MacroItem>(*it);
		//skip empty macroitems
		if(! d->macroitem.data()) {
			kdDebug() << "Context::activate() Skipping empty MacroItem" << endl;
			continue;
		}

		// fetch the Action, the MacroItem points to.
		KSharedPtr<Action> action = d->macroitem->action();
		//skip macroitems without an action
		if(! action.data()) {
			kdDebug() << "Context::activate() Skipping MacroItem with no action" << endl;
			continue;
		}

		try {
			// activate the action
			action->activate(this);
		}
		//catch exceptions
		catch(Exception& e) {
			//create a new exception from caugth one and set internal exception 
			d->exception = new Exception(e);
			//add new tracemessages
			//the macro name
			d->exception->addTraceMessage( QString("macro=%1").arg(d->macro->name()) );
			//the action name
			d->exception->addTraceMessage( QString("action=%1").arg(action->name()) );
			//and all variables wich belong to the action/macro
			QStringList variables = action->variableNames();
			for(QStringList::Iterator vit = variables.begin(); vit != variables.end(); ++vit) {
				KSharedPtr<Variable> v = d->macroitem->variable(*vit, true);
				d->exception->addTraceMessage( QString("%1=%2").arg(*vit).arg(v->toString()) );
			}
			return; // abort execution
		}
	}

	// The run is done. So, let's remove the currently selected item to
	// outline, that we did the job and there stays no dangling item.
	d->macroitem = KSharedPtr<MacroItem>(0);
}

//try to activated an context
void Context::activate(KSharedPtr<Context> context)
{
	//setup context
	delete d->exception; d->exception = 0;
	
	if(context->hadException()) {
		// if the context in which this context should run in already had an exception,
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

//try to continue activation of a context
void Context::activateNext()
{	
	//setup/clear context,
	//allows us to continue activation even when an exception happend before
	delete d->exception; d->exception = 0;

	if(! d->macroitem) { // if no MacroItem is defined, we don't need to try to continue execution
		return;
	}

	//find the macroitem from which to continue
	QValueList<KSharedPtr<MacroItem > >::ConstIterator it = d->items.find(d->macroitem);
	if (it != d->items.constEnd()) {
		activate(++it); // try to continue the execution.
	}
}

#include "context.moc"
