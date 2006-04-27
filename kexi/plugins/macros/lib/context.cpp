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
			* The iterator pointing to the @a Action instance that should
			* be executed next.
			*/
			QValueList<MacroItem::Ptr>::Iterator itemit;

			/**
			* The currently selected @a MacroItem or NULL if there
			* is now @a MacroItem selected yet.
			*/
			MacroItem::Ptr macroitem;

			/**
			* Map of all @a Variable instance that are defined within this
			* context.
			*/
			QMap<QString, Variable::Ptr> variables;

	};

}
//Construktor with initialization of our d-pointer
Context::Context(Macro::Ptr macro)
	: QObject()
	, d( new Private() ) // create the private d-pointer instance.
{
	// remember the macro
	d->macro = macro;
	//set d-pointer children to macro children
	d->items = macro->items();
	//set d-pointer iterator on first child
	d->itemit = d->items.begin();
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
	if (! d->variables.contains(name)) {
		throw Exception(QString("Variable name='%1' does not exist.").arg(name), QString("Komacro::Context::variable"));
	}
	return d->variables[name];
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

//return the currently selected MacroItem
MacroItem::Ptr Context::macroItem() const
{
	return d->macroitem;
}

//activate an action if there is one 
void Context::activate()
{
	kdDebug() << "Context::activate()" << endl;
	//Q_ASSIGN(d->macro);

	//check if the iterator reached the end.
	if(d->itemit == d->items.end()) {
		//if that's the case, just don't execute. Note, that this makes
		//Context a one-execution only container.
		d->macroitem = MacroItem::Ptr(0);
		return;
	}

	//fetch the MacroItem we like to handle.
	d->macroitem = MacroItem::Ptr(*d->itemit);

	//increment iterator to point to the next MacroItem we like to handle
	//after the current one is done.
	++d->itemit;

	//check if we really got a valid MacroItem
	if(! d->macroitem.data()) {
		kdWarning() << "Context::activate() MacroItem is NULL" << endl;
		return;
	}

	//fetch the Action, the MacroItem points to.
	Action* action = d->macroitem->action();
	
	//skip the MacroItem if it doesn't point to a valid Action
	if(! action) {
		kdDebug() << "Context::activate() Skipping empty MacroItem" << endl;
		activate();
		return;
	}

	if(action->isBlocking()) { // execution should be done synchron/blocking
		//activate the action
		action->activate(this);
		//recursive call ourself to handle the next action.
		activate();
	}
	else { // execution should be done asynchron/nonblocking
		//connect action activated signal to our activate slot.
		connect(action, SIGNAL(activated()), this, SLOT(activate()));
		//QTimer::singleShot(1000, action, SLOT(activate())); //testcase
		//activate the action
		action->activate(this);
	}

	// The run is done. So, let's remove the currently selected item
	// to outline, that we don't do anything yet.
	d->macroitem = MacroItem::Ptr(0);
}

void Context::activate(Context::Ptr context)
{
	// Merge the passed context into this context
	Variable::Map variables = context->variables();
	//copy variables
	Variable::Map::ConstIterator it, end( variables.constEnd() );
	for( it = variables.constBegin(); it != end; ++it)
		setVariable(it.key(), it.data());
	
	//activate copied context.
	activate();
}

#include "context.moc"
