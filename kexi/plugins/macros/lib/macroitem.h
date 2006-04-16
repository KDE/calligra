/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2006 by Sascha Kupper (kusato@kfnv.de)
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

#ifndef KOMACRO_MACROITEM_H
#define KOMACRO_MACROITEM_H

#include <qobject.h>

//#include <kaction.h>
#include <ksharedptr.h>

// Forward declarations.
class QDomElement;

#include "action.h"
#include "context.h"

namespace KoMacro {

	// Forward-declarations.
	//class Action;

	class KOMACRO_EXPORT MacroItem : public QObject
	{
			Q_OBJECT
		public:

			MacroItem(QObject* parent);
			~MacroItem();

			QString comment() const;
			void setComment(const QString& comment);

			KSharedPtr<Action> action() const;
			void setAction(KSharedPtr<Action> action);

			KSharedPtr<Variable> variable(const QString& name) const;
			QMap<QString, KSharedPtr<Variable> > variables() const;
			bool setVariable(const QString& name, KSharedPtr<Variable> variable);

		public slots:
			void activate() {}

		signals:
			void updated(QStringList ids);

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
