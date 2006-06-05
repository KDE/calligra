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
			KSharedPtr<MetaObject> metaobject;
	};

}

Function::Function(const QString& name)
	: Action(name)
	, d( new Private() ) // create the private d-pointer instance.
{
	kdDebug() << "Function::Function() name=" << name << endl;
/*
	// each function points to a receiver and a slot.
	d->receiver = element.attribute("receiver");
	QString slot = element.attribute("slot");
	d->slot = QObject::normalizeSignalSlot( slot.isNull() ? "" : slot.latin1() );

	// Iterate through the list of child-nodes. The child-nodes provides us e.g.
	// optional arguments.	
	int argcounter = 1;
	QDomNode node = element.firstChild();
	while(! node.isNull()) {
		QDomElement e = node.toElement(); // try to convert the node to an element.
		if(! e.isNull()) { // be sure we have a valid element.
			if(e.tagName() == "argument") { // an argument
				// Append a new variable to our list of arguments.
				Variable* v = new Variable(e);
				v->setName( QString::number(argcounter) );
				this->setVariable(v);
				argcounter++;
			}
			else if(e.tagName() == "return") { // the returnvalue
				Variable* v = new Variable(e);
				v->setName("0");
				this->setVariable(v);
				
			}
			//else throw Exception(QString("Invalid tagname '%1'").arg(e.tagName()), "KoMacro::Function::Function");

		}
		node = node.nextSibling();
	}
*/
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

KSharedPtr<MetaObject> Function::receiverObject()
{
	if(! d->metaobject) {
		QStringList list = QStringList::split("/", d->receiver);
		QStringList::ConstIterator it( list.constBegin() ), end( list.constEnd() ) ;
		QObject* object = Manager::self()->object( *it );
		for(++it; object && it != end; ++it) {
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

void Function::setReceiverObject(KSharedPtr<MetaObject> metaobject)
{
	d->metaobject = metaobject;
}

void Function::activate(KSharedPtr<Context> context)
{
	kdDebug() << "Function::activate(KSharedPtr<Context>)" << endl;
	try {
		// First we build a list of variables passed to the invoke as arguments.
		Variable::List variables;
		Variable::Map varmap = this->variables();
		variables.append( varmap["0"] );

		for(int i = 1; true; i++) {
			KSharedPtr<Variable> var = varmap[ QString::number(i) ];
			if(! var.data())
				break;

			// Look for variables.
			if( var->type() == MetaParameter::TypeVariant ) {
				QString s = var->toString();
				if(s.startsWith("$") && context.data()) {
					// If the content is a variable we try to read the
					// variable from the context.
					KSharedPtr<Variable> v = context->variable(s);
					/*if(!v) {
						 throw Exception(QString("No such variable: %1").arg(s), "Function::activate");
					}*/
					variables.append(v);
					continue; // variable successfully handled.
				}
			}

			variables.append(var);
		}

		// Now invoke the method.
		KSharedPtr<MetaObject> metaobject = receiverObject();
		//kdDebug() << "Function::activate(KSharedPtr<Context>) metaobject: " << metaobject.data() << endl;
		KSharedPtr<Variable> returnvalue = metaobject->invokeMethod(
			metaobject->indexOfSlot( d->slot.isNull() ? "" : d->slot.latin1() ), // the index of the slot which should be invoked.
			variables // the optional list of passed arguments
		);

		// Look if the returnvalue is a variable.
		KSharedPtr<Variable> rv = this->variable("0");
		const QString rvvalue = rv ? rv->toString() : QString::null;
		if(rvvalue.startsWith("$") && context.data()) {
			// the returnvalue is a variable... so, just remember it in the context.
			context->setVariable(rvvalue,returnvalue);
		}
		else {
			// the first item in the list of variables is always the returnvalue.
			returnvalue->setName("0");
			this->setVariable(returnvalue);
		}

		kdDebug() << "Function::activate(KSharedPtr<Context>) return=" << rvvalue << " value=" << returnvalue->toString() << endl;
		emit activated(); // job done.
	}
	catch(Exception& e) {
		kdWarning() << QString("Function::activate() EXCEPTION: %1").arg(e.errorMessage()) << endl;
		e.addTraceMessage("KoMacro::Function::activate()");
		throw Exception(e); // re-throw exception
	}
}

#include "function.moc"
