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
			QValueList<MacroItem::Ptr> itemlist;

			/**
			* The name the @a Macro has.
			*/
			QString name;

	};

}

Macro::Macro(const QString& name)
	: QObject()
	, KShared()
	, XMLHandler(this)
	, d( new Private() ) // create the private d-pointer instance.
{
	d->name = name;
}

Macro::~Macro()
{
	// destroy the private d-pointer instance.
	delete d;
}

const QString Macro::name() const
{
	return d->name;
}

void Macro::setName(const QString& name)
{
	d->name = name;
}

const QString Macro::toString() const
{
	return QString("Macro:%1").arg(name());
}

QValueList<MacroItem::Ptr>& Macro::items() const
{
	return d->itemlist;
}

void Macro::addItem(MacroItem::Ptr item)
{
	d->itemlist.append(item);
}

void Macro::clearItems()
{
	d->itemlist.clear();
}

void Macro::connectSignal(const QObject* sender, const char* signal)
{
	MetaProxy* metaproxy = new MetaProxy();

	metaproxy->connectSignal(sender, signal);

	connect( metaproxy, SIGNAL(slotCalled(QValueList< KSharedPtr<Variable> >)),
			 this,SLOT(activate(QValueList< KSharedPtr<Variable> >)) );

	//TODO d->proxies.append( metaproxy );
}

void Macro::execute(QObject* sender)
{
	kdDebug() << "Macro::execute(Context::Ptr)" << endl;

	Context::Ptr c = Context::Ptr( new Context(this) );
	if(sender) {
		// set the sender-variable if we got a sender QObject.
		c->setVariable("[sender]", Variable::Ptr( new Variable(sender) ));
	}
	//connect(context, SIGNAL(activated()), this, SIGNAL(activated()));
	c->activate( c );
}

#include "macro.moc"
