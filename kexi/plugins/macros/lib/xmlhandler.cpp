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

	class XMLHandler::Private
	{
		public:

			/**
			* The \a Macro instance this \a XMLHandler
			* manages.
			*/
			Macro::Ptr macro;

			/**
			* Constructor.
			*
			* @param macro The \a Macro instance this
			* \a XMLHandler manages.
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
			Action* insertAction(QDomElement element)
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

void XMLHandler::toMacro(QDomElement element)
{
	Q_UNUSED(element);
	/*
	Macro* macro = new Macro();
	while (node.nextNode != null) {
		macro->addChild(insertAction(node.toElement()));
		node = node.nextSibling();
	}
	*/
}

QDomElement XMLHandler::toXML()
{
	QDomDocument document;
	QDomElement macroelem = document.createElement("macro");
	QValueList<Action::Ptr> children = d->macro->children();
	QValueList<Action::Ptr>::Iterator childit = children.begin();
	for(;childit != children.end(); childit++) {
		QDomElement actionelem = document.createElement("action");
		macroelem.appendChild(actionelem);
	}
	return macroelem;
}

