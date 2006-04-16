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
#include "xmlhandler.h"

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
			* A map of @a MacroItem instances accessible
			* by there unique name.
			*/
			QMap<QString, MacroItem*> itemmap;

			/**
			* A list of @a MacroItem instances.
			*/
			QValueList<MacroItem*> itemlist;

			/**
			*
			*/
			XMLHandler* xmlhandler;

			Private()
				: xmlhandler(0)
			{
			}

			~Private()
			{
				delete xmlhandler;
			}

	};

}

Macro::Macro(const QString& name)
	: Action(name)
	, d( new Private() ) // create the private d-pointer instance.
{
	//kdDebug() << "Macro::Macro(Manager*, QDomElement&)" << endl;

/*
	// Iterate through the child nodes the passed DOM node has and build
	// recursivly a Macro tree.
	QDomNode node = element.firstChild();
	while(! node.isNull()) {
		QDomElement e = node.toElement(); // try to convert the node to an element.
		if(! e.isNull()) { // be sure we have a valid element.

			Action::Ptr action = manager->createAction(e);
			if(action) {
				addChild(action);
			}
			else {
				kdWarning() << QString("Macro::Macro() Failed to create action for tag \"%1\"").arg(e.tagName()) << endl;
			}

		}
		node = node.nextSibling();
	}
*/
}

Macro::~Macro()
{
	//kdDebug() << "Macro::~Macro()" << endl;
	// destroy the private d-pointer instance.
	delete d;
}

const QString Macro::toString() const
{
	return QString("Macro:%1").arg(name());
}

MacroItem* Macro::item(const QString& name) const
{
	return d->itemmap[name];
}

QValueList<MacroItem*> Macro::items() const
{
	return d->itemlist;
}

MacroItem* Macro::addItem(const QString& name)
{
	MacroItem* item = new MacroItem(this);
	d->itemmap.replace(name, item);
	d->itemlist.append(item);
	return item;
}

/*
void Macro::addChild(Action::Ptr action)
{
	if(! d->actionmap.contains( action->name() ))
		d->actionlist.append( action );
	d->actionmap.replace( action->name(), action );
}

bool Macro::hasChildren() const
{
	return (! d->actionlist.empty());
}

Action::Ptr Macro::child(const QString& name) const
{
	return d->actionmap[name];
}

QValueList<Action::Ptr> Macro::children() const
{
	return d->actionlist;
}
*/

void Macro::connectSignal(const QObject* sender, const char* signal)
{
	MetaProxy* metaproxy = new MetaProxy();

	metaproxy->connectSignal(sender, signal);

	connect( metaproxy, SIGNAL(slotCalled(QValueList< KSharedPtr<Variable> >)),
			 this,SLOT(activate(QValueList< KSharedPtr<Variable> >)) );

	//TODO d->proxies.append( metaproxy );
}

XMLHandler* Macro::xmlHandler()
{
	if(! d->xmlhandler) {
		d->xmlhandler = new XMLHandler(this);
	}
	return d->xmlhandler;
}

void Macro::activate()
{
	kdDebug() << "Macro::activate()" << endl;

	Context::Ptr context = new Context(this);
	//connect(context, SIGNAL(activated()), this, SIGNAL(activated()));
	context->activate();
}

void Macro::activate(Context::Ptr context)
{
	Context* c = new Context(this);
	//connect(context, SIGNAL(activated()), this, SIGNAL(activated()));
	c->activate( context );
}

#include "macro.moc"
