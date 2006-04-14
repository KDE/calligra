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

#include "function.h"
#include "manager.h"
#include "variable.h"
#include "exception.h"
#include "metaparameter.h"

#include <qvariant.h>
#include <qdom.h>
#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class Function::Private
	{
		public:

			/**
			* The name of the receiver object. Those name will be used
			* to look for published objects ( see @a Manager::publishObject() )
			* at runtime to determinate the matching QObject.
			*/
			QString receiver;

			/**
			* The name of the slot at the receiver object that is our
			* function.
			*/
			QString slot;

			/**
			* Cached @a MetaObject the receiver points to. The metaobject
			* is NULL if the receiver QString should be used to determinate
			* the matching QObject. If we determinated the QObject already
			* with the receiver QString and the receiver didn't changed
			* (aka got dirty), then the metaobject holds a valid pointer
			* to the defined QObject instance.
			*/
			MetaObject::Ptr metaobject;
	};

}

Function::Function(Manager* const manager, const QDomElement& element)
	: Action(manager, element)
	, d( new Private() ) // create the private d-pointer instance.
{
	//kdDebug() << "Function::Function()" << endl;

	// each function points to a receiver and a slot.
	d->receiver = element.attribute("receiver");
	QString slot = element.attribute("slot");
	d->slot = QObject::normalizeSignalSlot( slot.isNull() ? "" : slot.latin1() );

	// Iterate through the list of child-nodes. The child-nodes provides us e.g.
	// optional arguments.	
	Variable::List variables;
	Variable::Ptr returnvalue;
	QDomNode node = element.firstChild();
	while(! node.isNull()) {
		QDomElement e = node.toElement(); // try to convert the node to an element.
		if(! e.isNull()) { // be sure we have a valid element.

			if(e.tagName() == "argument") { // an argument
				// Append a new variable to our list of arguments.
				variables.append( new Variable(e) ); // fill the list of arguments.
			}
			else if(e.tagName() == "return") { // the returnvalue
				returnvalue = new Variable(e);
			}
			//else throw Exception(QString("Invalid tagname '%1'").arg(e.tagName()), "KoMacro::Function::Function");

		}
		node = node.nextSibling();
	}
	// the returnvalue is always the first item in the variables-list!
	variables.prepend(returnvalue);
	// set the variables
	this->setVariables(variables);
}

Function::~Function()
{
	//kdDebug() << "Function::~Function()" << endl;

	// destroy the private d-pointer instance.
	delete d;
}

const QString Function::toString() const
{
	return QString("Function:%1.%2").arg(d->receiver).arg(d->slot);
}

const QString Function::receiver() const
{
	return d->receiver;
}

const QString Function::slot() const
{
	return d->slot;
}

MetaObject::Ptr Function::receiverObject()
{
	if(! d->metaobject) {
		QStringList list = QStringList::split("/", d->receiver);
		QStringList::Iterator it = list.begin();
		QObject* object = manager()->object( *it );
		for(++it; object && it != list.end(); ++it) {
			if((*it).isNull()) {
				object = 0;
				break;
			}
			object = object->child( (*it).latin1() );
		}
		if(! object) {
			throw Exception(
				QString("Invalid receiver \"%1\"").arg(d->receiver),
				"KoMacro::Function::activate()"
			);
		}

		d->metaobject = new MetaObject(object);
	}
	return d->metaobject;
}

void Function::setReceiverObject(MetaObject::Ptr metaobject)
{
	d->metaobject = metaobject;
}

void Function::activate()
{
	Context::Ptr context = new Context(this);
	activate(context);
}

void Function::activate(Context::Ptr context)
{
	//  kdDebug() << "Function::activate(Context::Ptr) try" << endl;
	try {
		// First we build a list of variables passed to the invoke as arguments.
		Variable::List variables;
		Variable::List v = this->variables(); // cache the list for the iteration
		for(Variable::List::Iterator it = v.begin(); it != v.end(); ++it) {
			if(*it) {
				// Look for variables.
				if( (*it)->type() == MetaParameter::TypeVariant ) {
					QString s = (*it)->toString();
					if(s.startsWith("$")) {
						// If the content is a variable we try to read the
						// variable from the context.
						Variable::Ptr v = context->variable(s);
						/*if(!v) {
							 throw Exception(QString("No such variable: %1").arg(s), "Function::activate");
						}*/
						variables.append(v);
						continue; // variable successfully handled.
					}
				}
			}

			// append the variable.
			variables.append( *it );
		}

		// Now invoke the method.
		MetaObject::Ptr metaobject = receiverObject();
		//kdDebug() << "Function::activate(Context::Ptr) metaobject: " << metaobject.data() << endl;
		Variable::Ptr returnvalue = metaobject->invokeMethod(
			metaobject->indexOfSlot( d->slot.isNull() ? "" : d->slot.latin1() ), // the index of the slot which should be invoked.
			variables // the optional list of passed arguments
		);

		// Look if the returnvalue is a variable.
		Variable::Ptr rv = this->variables().first();
		const QString rvname = rv ? rv->toString() : QString::null;
		if(rvname.startsWith("$")) {
			// the returnvalue is a variable... so, just remember it in the context.
			context->setVariable(rvname, returnvalue);
		}
		else {
			// the first item in the list of variables is always the returnvalue.
			this->variables().first() = returnvalue;
		}

		kdDebug() << "Function::activate(Context::Ptr) return=" << rvname << " value=" << returnvalue->toString() << endl;
		emit activated(); // job done.
	}
	catch(Exception& e) {
		kdWarning() << QString("Function::activate() EXCEPTION: %1").arg(e.errorMessage()) << endl;
		e.addTraceMessage("KoMacro::Function::activate()");
		throw Exception(e); // re-throw exception
	}
}

#include "function.moc"
