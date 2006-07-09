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

#include "macro.h"
#include "macroitem.h"
#include "manager.h"
#include "context.h"
#include "variable.h"

#include "metaproxy.h"

#include <qdom.h>
#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class Macro::Private
	{
		public:

			/**
			* A list of @a MacroItem instances.
			*/
			QValueList<KSharedPtr<MacroItem > > itemlist;

			/**
			* The name the @a Macro has.
			*/
			QString name;

	};

}

//constructor, initalize internal (d-pointer) name
Macro::Macro(const QString& name)
	: QObject()
	, KShared()
	, XMLHandler(this)
	, d( new Private() ) // create the private d-pointer instance.
{
	d->name = name;
}

//destructor
Macro::~Macro()
{
	// destroy the private d-pointer instance.
	delete d;
}

//get internal (d-pointer) name
const QString Macro::name() const
{
	return d->name;
}

//set internal (d-pointer) name
void Macro::setName(const QString& name)
{
	d->name = name;
}

//get an "extended" name
const QString Macro::toString() const
{
	return QString("Macro:%1").arg(name());
}

//get (d-pointer) itemlist
QValueList<KSharedPtr<MacroItem > >& Macro::items() const
{
	return d->itemlist;
}

//add a macroitem to internal (d-pointer) itemlist
void Macro::addItem(KSharedPtr<MacroItem> item)
{
	d->itemlist.append(item);
}
//clear internal (d-pointer) itemlist
void Macro::clearItems()
{
	d->itemlist.clear();
}

//no longer needed
// void Macro::connectSignal(const QObject* sender, const char* signal)
// {
// 	MetaProxy* metaproxy = new MetaProxy();
// 
// 	metaproxy->connectSignal(sender, signal);
// 
// 	connect( metaproxy, SIGNAL(slotCalled(QValueList< KSharedPtr<Variable> >)),
// 			 this,SLOT(activate(QValueList< KSharedPtr<Variable> >)) );
// 
// 	//TODO d->proxies.append( metaproxy );
// }

//run our macro
KSharedPtr<Context> Macro::execute(QObject* sender)
{
	kdDebug() << "Macro::execute(KSharedPtr<Context>)" << endl;

	//create context in which macro can/should run
	KSharedPtr<Context> c = KSharedPtr<Context>( new Context(this) );
	if(sender) {
		// set the sender-variable if we got a sender QObject.
		c->setVariable("[sender]", KSharedPtr<Variable>( new Variable(sender) ));
	}
	//connect(context, SIGNAL(activated()), this, SIGNAL(activated()));
	
	//call activate in the context of the macro
	c->activate( c );

	return c;
}

#include "macro.moc"
