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

#ifndef KOMACRO_METAPROXY_H
#define KOMACRO_METAPROXY_H

#include <qobject.h>
#include <ksharedptr.h>

#include "komacro_export.h"

struct QUObject;

namespace KoMacro {

	// forward declarations.
	class Variable;
	//class MetaMethod;

	/**
	* The interface for @a MetaProxy classes. Those interface is
	* needed only to be able to use the Q_OBJECT macro and provide
	* that way signals and slots at compiletime.
	*/
	class KOMACRO_EXPORT MetaProxyInterface : public QObject
	{
			Q_OBJECT
		public:

			/**
			* Constructor.
			*/
			explicit MetaProxyInterface() {}

			/**
			* Destructor.
			*/
			virtual ~MetaProxyInterface() {}

		signals:
			void slotCalled( /*QObject* receiver,*/ QValueList< KSharedPtr<Variable> >);
			//void signalEmitted(QObject* sender, Variable::List);
	};

	/**
	* The MetaProxy class is used as proxy between Qt signals and
	* slots.
	*/
	class KOMACRO_EXPORT MetaProxy : public MetaProxyInterface
	{
			//Q_OBJECT // don't use the moc in that class cause we will handle it dynamicly!.
		public:

			/**
			* Constructor.
			*/
			explicit MetaProxy();

			/**
			* Destructor.
			*/
			~MetaProxy();

			void setSlots(QValueList<const char*> slotlist);
			//void setSignals(QValueList<const char*> signallist);

			//void connectMethod(KSharedPtr<MetaMethod>);
			//void connectSignal(QObject* sender, const char* signal);
			//void disconnectSignal(QObject* sender, const char* signal);
			//void connectSlot(QObject* receiver, const char* slot);
			//void disconnectSlot(QObject* receiver, const char* slot);

			bool connectSignal(const QObject* sender, const char * signal);
			//bool connectSlot(const QObject* receiver, const char * slot);
			//bool disconnect(const char * signal = 0, const QObject * receiver = 0, const char * member = 0) {}
			//bool disconnect(const QObject * receiver, const char * member = 0) {}


			/**
			* Overloaded method the Qt moc-generator normaly creates 
			* during pre-processing if the Q_OBJECT macro is
			* defined. We implement this method by hand cause we
			* don't use the Qt moc to generate it staticly rather
			* then dynamicly on the fly.
			*
			* @param index The index of the signal or slot that
			* should be invoked.
			* @param ou Optional arguments passed to the signal or
			* slot.
			* @return true if invoke was handled else false.
			*/
			bool qt_invoke(int index, QUObject* ou);

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
