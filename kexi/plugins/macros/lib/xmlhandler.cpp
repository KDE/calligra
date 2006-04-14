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

#include <qdom.h>

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
			Macro::Ptr macro;

			/**
			* Constructor.
			*
			* @param macro The @a Macro instance this
			* @a XMLHandler manages.
			*/
			Private(Macro::Ptr macro)
				: macro(macro)
			{
			}

			/**
			* Parse an @a Action out of a given node
			* @return Returns an @a Action, if the node
			* contains a valid userdefined
			* @a Action, else returns NULL.
			*/
			Action* insertAction(const QDomElement& element)
			{
				Q_UNUSED(element);
				/*
				QMap<QString, Action*> actionTypes = Manager::self().actionTypes();
				Action* action = actionTypes[node.getAttribute("actiontype"));
				// - subclasses of Action and let them parse
				// - read properties of a node and fill them automaticly
				// - factory in Action

				// diff between predefined Action and in a Macro defined Actions
				// - Action vs. ActionImpl
				// - copy from Action
				ActionImpl* ai = action->create();

				action->activate(); // bzw. ai->activate()
				return ai or action;
				*/
				return 0;
			}
	};

}

XMLHandler::XMLHandler(Macro::Ptr macro)
	: d( new Private(macro) )
{
}

XMLHandler::~XMLHandler()
{
	delete d;
}

bool XMLHandler::fromXML(const QDomElement& element)
{
	Q_UNUSED(element);
	/*
	Macro* macro = new Macro();
	while (node.nextNode != null) {
		macro->addChild(insertAction(node.toElement()));
		node = node.nextSibling();
	}
	*/
	return true;
}

QDomElement XMLHandler::toXML()
{
	// The QDomDocument provides us the functionality to create new QDomElement instances.
	QDomDocument document;
	// Create the Macro-QDomElement. This element will be returned.
	QDomElement macroelem = document.createElement("macro");
	// The children (aka actions) a Macro provides.
	QValueList<Action::Ptr> children = d->macro->children();
	// Create an iterator...
	QValueList<Action::Ptr>::Iterator childit = children.begin();
	// ...and iterate over the list of children the Macro provides.
	for(;childit != children.end(); childit++) {
		Action::Ptr action = *childit;
		// The name the action has is used as tagname.
		const QString tagname = action->name();
		// Create a new QDomElement...
		QDomElement actionelem = document.createElement(tagname);

		//TODO add action-specific attributes/child-nodes/etc.

		// ...and append the new QDomElement as child to our Macro-QDomElement.
		macroelem.appendChild(actionelem);
	}
	return macroelem;
}

