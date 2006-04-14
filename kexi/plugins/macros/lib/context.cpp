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
			Action::Ptr action;

			/**
			* List of @a Action instances that are children of the
			* macro.
			*/
			QValueList<Action::Ptr> children;

			/**
			* The iterator pointing to the @a Action instance that should
			* be executed next.
			*/
			QValueList<Action::Ptr>::Iterator childit;

			/**
			* Map of all @a Variable instance that are defined within this
			* context.
			*/
			QMap<QString, Variable::Ptr> variables;

	};

}
//Construktor with initialization of our d-pointer
Context::Context(Action::Ptr action)
	: QObject()
	, d( new Private() ) // create the private d-pointer instance.
{
	d->action = action;

	//creat a Macro
	Macro* macro = dynamic_cast<Macro*>( action.data() );
	if(macro) {
		//set d-pointer children to macro children
	d->children = macro->children();
	//set d-pointer iterator on first child
		d->childit = d->children.begin();
	}
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

//activate an action if there is one 
void Context::activate()
{
	kdDebug() << "Context::activate()" << endl;

	if(! d->action) {
		kdDebug() << "Context::activate() no Action" << endl;
		return;
	}

	//cast macro from internal d-pointer
	Macro* macro = dynamic_cast<Macro*>( d->action.data() );
	//it really is a macro
	if(macro) {
	//walk trough children
		if(d->childit != d->children.end()) {
			//fetch an action
			Action* action = (*d->childit).data();
			//increment iterator
			++d->childit;
			if(! action) {
				return; //abort if there stays no action.
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
		}
	}
	//no macro just activate our d-pointer action
	else {
		d->action->activate();
	}
}

void Context::activate(Context::Ptr context)
{
	// Merge the passed context into this context
	Variable::Map variables = context->variables();
	//copy variables
	for(Variable::Map::Iterator it = variables.begin(); it != variables.end(); ++it)
		setVariable(it.key(), it.data());
	
	//activate copied context.
	activate();
}

#include "context.moc"
