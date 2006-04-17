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

#include "action.h"
#include "manager.h"
#include "exception.h"

#include <qdom.h>
#include <kdebug.h>

using namespace KoMacro;

namespace KoMacro {

	/**
	* @internal d-pointer class to be more flexible on future extension of the
	* functionality without to much risk to break the binary compatibility.
	*/
	class Action::Private
	{
		public:

			/**
			* The name this @a Action has.
			*/
			QString name;

			/**
			* The comment the user is able to define for each action.
			*/
			QString comment;

			/**
			 * Defines if this action should be executed synchron
			 * (blocking) or asynchron (nonblocking).
			 */
			bool blocking;

			/**
			 * A map of @a Variable instances this @a Action
			 * provides accessible by there QString name.
			 */
			Variable::Map variables;

			/**
			* Cached QDomElement.
			*/
			QDomElement element;

			/**
			* Constructor.
			*/
			Private()
				: blocking(true)
			{
			}

	};

}

Action::Action(const QString& name)
	: KAction()
	, KShared()
	, d( new Private() ) // create the private d-pointer instance.
{
	kdDebug() << "Action::Action() name=" << name << endl;
	d->name = name;

/*
	// We need a QCString. XML is always utf8, right?
	QCString name = element.attribute("name").utf8();
	setName( name );

	setText( element.attribute("text") );
	setComment( element.attribute("comment") );
	setIcon( element.attribute("icon") );
	//setWhatsThis( element.attribute("whatsthis") );
	//setToolTip( element.attribute("tooltip") );

	d->element = element;
	//kdDebug() << QString("Action::Action() name=\"%1\"").arg(name) << endl;
*/
}

Action::~Action()
{
	//kdDebug() << QString("Action::~Action() name=\"%1\"").arg(name()) << endl;

	// destroy the private d-pointer instance.
	delete d;
}

const QString Action::toString() const
{
	return QString("Action:%1").arg(name());
}

const QString Action::name() const
{
	return d->name;
}

const QString Action::comment() const
{
	return d->comment;
}

void Action::setComment(const QString& comment)
{
	d->comment = comment;
}

bool Action::isBlocking() const
{
	return d->blocking;
}

void Action::setBlocking(bool blocking)
{
	d->blocking = blocking;
}

Variable::Ptr Action::variable(const QString& name) const
{
	return d->variables[name];
}

Variable::Map Action::variables() const
{
	return d->variables;
}

void Action::setVariable(const QString& name, Variable::Ptr variable)
{
	Q_ASSERT(! d->variables.contains(name));
	d->variables.replace(name, variable);
}

const QDomElement Action::domElement() const
{
	return d->element;
}

void Action::activate()
{
	kdDebug() << "Action::activate() name=" << name() << " text=" << text() << endl;

	QCString s = name().isNull() ? "" : name().latin1();
	KAction* action = Manager::self()->guiClient()->action(s);
	if(action) {
		action->activate();
	}
	else {
		kdWarning() << QString("Action::activate() No such action \"%1\"").arg(name()) << endl;
	}

	emit activated();
}

void Action::activate(Context::Ptr /*context*/)
{
	kdDebug() << "Action::activate(Context*) name=" << name() << " text=" << text() << endl;

	QCString s = name().isNull() ? "" : name().latin1();
	KAction* action = Manager::self()->guiClient()->action( s );
	if(! action) {
		throw Exception(
			QString("No such action \"%1\"").arg(name()),
			"KoMacro::Action::activate(Context*)"
		);
	}

	action->activate();

	emit activated();
}

void Action::setAction(const KAction* action)
{
	setName( action->name() );
	setText( action->text().remove("&") );
	setComment( "" );
	setIcon( action->icon() );
	setWhatsThis( action->whatsThis() );
	setToolTip( action->toolTip() );
	//d->element = QDomElement();
}

#include "action.moc"
