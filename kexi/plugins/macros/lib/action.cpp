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

#include <qstringlist.h>
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
			* The i18n-caption text this @a Action has.
			*/
			QString text;

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
			Variable::Map varmap;

			/**
			* List of variablenames. This list provides a
			* sorted order for the @a Variable instances
			* defined in the map above.
			*/
			QStringList varnames;

			/**
			* Constructor.
			*/
			Private()
				: blocking(true)
			{
			}

	};

}

Action::Action(const QString& name, const QString& text)
	: QObject()
	, KShared()
	, d( new Private() ) // create the private d-pointer instance.
{
	kdDebug() << "Action::Action() name=" << name << endl;
	d->name = name;
	setText(text);
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

void Action::setName(const QString& name)
{
	d->name = name;
}

const QString Action::text() const
{
	return d->text;
}

void Action::setText(const QString& text)
{
	d->text = text;
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

bool Action::hasVariable(const QString& name) const
{
	return d->varmap.contains(name);
}

Variable::Ptr Action::variable(const QString& name) const
{
	return d->varmap.contains(name) ? d->varmap[name] : Variable::Ptr(0);
}

Variable::Map Action::variables() const
{
	return d->varmap;
}

QStringList Action::variableNames() const
{
	return d->varnames;
}

void Action::setVariable(Variable::Ptr variable)
{
	const QString name = variable->name();
	if(! d->varmap.contains(name)) {
		d->varnames.append(name);
	}
	d->varmap.replace(name, variable);
}

void Action::setVariable(const QString& name, const QString& text, const QVariant& variant)
{
	Variable* variable = new Variable(variant);
	variable->setName(name);
	variable->setText(text);
	setVariable( Variable::Ptr(variable) );
}

void Action::removeVariable(const QString& name)
{
	if(d->varmap.contains(name)) {
		d->varmap.remove(name);
		d->varnames.remove(name);
	}
}

/*
void Action::activate(Context::Ptr context)
{
	Q_UNUSED(context);
	kdDebug() << "Action::activate(Context::Ptr) name=" << name() << " text=" << text() << endl;

	QCString s = name().isNull() ? "" : name().latin1();
	KAction* action = Manager::self()->guiClient()->action( s );
	if(action) {
		action->activate();
	}
	else {
		kdWarning() << QString("Action::activate(Context::Ptr) No such action \"%1\"").arg(name()) << endl;
	}

	emit activated();
}
*/

#include "action.moc"
