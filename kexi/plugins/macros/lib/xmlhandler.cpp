/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
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

#include "xmlhandler.h"
#include "macro.h"
#include "macroitem.h"
#include "action.h"

#include <qdom.h>
#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class XMLHandler::Private
	{
		public:

			/**
			* The @a Macro instance this @a XMLHandler
			* manages.
			*/
			Macro* const macro;

			/**
			* Constructor.
			*
			* @param macro The @a Macro instance this
			* @a XMLHandler manages.
			*/
			Private(Macro* const macro)
				: macro(macro)
			{
			}
	};

}

XMLHandler::XMLHandler(Macro* const macro)
	: d( new Private(macro) )
{
}

XMLHandler::~XMLHandler()
{
	delete d;
}

bool XMLHandler::parseXML(const QDomElement& element)
{
	// We expect a <macro> element. Do we really need to be such strict or
	// would it be more wise to trust the application in that case?
	if(element.tagName() != "macro") {
		kdDebug() << QString("XMLHandler::parseXML() Invalid tagname \"%1\"").arg(element.tagName()) << endl;
		return false;
	}

	// Remove old items.
	d->macro->clearItems();

	// Do we need to load the macro's name?
	// d->macro->setName(element.attribute("name"));

	// Iterate through the child nodes the passed QDomElement has and
	// build the MacroItem elements.
	QDomNode node = element.firstChild();
	while(! node.isNull()) {
		// The tagname should be "item"
		if(node.nodeName() == "item") {
			// The node is an element.
			QDomElement e = node.toElement();

			// Create a new MacroItem
			MacroItem* item = new MacroItem();

			// Add the new item to our Macro.
			d->macro->addItem( MacroItem::Ptr(item) );

			// Each MacroItem may point to an Action instance. We
			// try to determinate this action now and if it's defined
			// and available, we set it.
			Action::Ptr action = Manager::self()->action(e.attribute("action"));
			if(action.data()) {
				item->setAction(action);
			}

			// Set the comment
			item->setComment(e.attribute("comment"));
		}
		// Fetch the next item.
		node = node.nextSibling();
	}
	// Job was done successfully.
	return true;
}

QDomElement XMLHandler::toXML()
{
	// The QDomDocument provides us the functionality to create new QDomElement instances.
	QDomDocument document;

	// Create the Macro-QDomElement. This element will be returned.
	QDomElement macroelem = document.createElement("macro");

	// Do we need to store the macro's name? Normaly the application
	// could/should take care of it cause we don't know how the app
	// may store the XML and cause we don't like to introduce
	// redundancy at this point.
	//macroelem.setAttribute("name",d->macro->name());

	// The list of MacroItem-children a Macro provides.
	QValueList<MacroItem::Ptr> items = d->macro->items();

	// Create an iterator...
	QValueList<MacroItem::Ptr>::ConstIterator it(items.constBegin()), end(items.constEnd());
	// ...and iterate over the list of children the Macro provides.
	for(;it != end; it++) {
		// We are iterating over MacroItem instances.
		MacroItem* item = *it;

		// Flag to determinate if we really need to remember this item what
		// is only the case if comment or action is defined.
		bool append = false;

		// Each MacroItem will have an own node.
		QDomElement itemelem = document.createElement("item");

		// Each MacroItem could point to an Action provided by the Manager.
		const KoMacro::Action* action = item->action().data();
		if(action) {
			append = true;

			// Remember the name of the action.
			itemelem.setAttribute("action", action->name());

			// Each MacroItem could have a list of variables. We
			// iterate through that list and build a element
			// for each single variable.
			Variable::Map vmap = item->variables();
			Variable::Map::ConstIterator vit(vmap.constBegin()), vend(vmap.constEnd());
			for(; vit != vend; ++vit) {
				// Create an own element for the variable. The tagname will be
				// the name of the variable.
				QDomElement varelement = document.createElement(vit.key());
				// Remember the value as textnode.
				varelement.appendChild(document.createTextNode(vit.data()->toString()));
				// Add the new variable-element to our MacroItem.
				itemelem.appendChild(varelement);
			}
		}

		// Each MacroItem could have an optional comment.
		const QString comment = item->comment();
		if(! comment.isEmpty()) {
			append = true;
			itemelem.setAttribute("comment", item->comment());
		}

		// Check if we really need to remember the item.
		if(append) {
			macroelem.appendChild(itemelem);
		}
	}

	// Job done. Return the macro's element.
	return macroelem;
}
