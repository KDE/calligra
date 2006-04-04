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

#ifndef KOMACRO_MANAGER_H
#define KOMACRO_MANAGER_H

#include <qmap.h>
#include <qguardedptr.h>
#include <ksharedptr.h>
#include <kxmlguiclient.h>

#include "komacro_export.h"

class QObject;
class QDomElement;

namespace KoMacro {

	// Forward declarations.
	class Action;
	class Macro;

	/**
	* The Manager class acts as window-wide manager for macros.
	*
	* Example how KoMacro could be used.
	* @code
	* // We have a class that inheritates from QObject and
	* // implements some public signals and slots that will
	* // be accessible by Macros once a class-instance
	* // got published.
	* class PublishedObject : public QObject {};
	*
	* // Somewhere we have our KMainWindow.
	* KMainWindow* mainwindow = new KMainWindow();
	*
	* // Create a new KoMacro::Manager instance to access the
	* // Macro-framework.
	* KoMacro::Manager* manager = new KoMacro::Manager( mainwindow );
	*
	* // Now we like to publish a QObject
	* PublishedObject* publishedobject = new PublishedObject();
	* manager->publishObject(publishedobject);
	*
	* // ... here we are able to use manager->createAction() to
	* // create Action instances on the fly and work with them.
	*
	* // Finally free the publishedobject instance we created. We
	* // need to free it manualy cause PublishedObject doesn't
	* // got a QObject parent as argument.
	* delete publishedobject;
	*
	* // Finally free the manager-instance. It's always needed
	* // to free the instance by yourself!
	* delete manager;
	* @endcode
	*/
	class KOMACRO_EXPORT Manager
	{
		public:

			/**
			* Constructor.
			*
			* @param xmlguiclient The KXMLGUIClient instance this
			* @a Manager is associated with.
			*/
			explicit Manager(KXMLGUIClient* const xmlguiclient);

			/**
			* Destructor.
			*/
			virtual ~Manager();

			/**
			* @return the KXMLGUIClient instance this @a Manager is
			* associated with.
			*/
			KXMLGUIClient* const guiClient() const;

			/**
			* \return true if we carry a \a Macro with the
			* defined \p macroname .
			*/
			bool hasMacro(const QString& macroname);

			/**
			* \return the \a Macro defined with \p macroname
			* or NULL if we don't have such a \a Macro.
			*/
			KSharedPtr<Macro> getMacro(const QString& macroname);

			/**
			* Add a new \a Macro to the list of known macros. If
			* there exists already a \a Macro instance with the
			* defined \p macroname then the already existing one
			* will be replace.
			*
			* \param macroname The name the \a Macro will be
			*        accessible as.
			* \param macro The \a Macro instance.
			*/
			void addMacro(const QString& macroname, KSharedPtr<Macro> macro);

			/**
			* Remove the \a Macro defined with \p macroname . If
			* we don't know about a \a Macro with that \p macroname
			* nothing happens.
			*/
			void removeMacro(const QString& macroname);

			/**
			* Factory function to create a new \a Macro instances.
			* The returned new \a Macro instance will not be added
			* to the list of known macros. Use \a addMacro if you
			* like to attach the returned new \a Macro to this
			* \a Manager instance.
			*/
			KSharedPtr<Macro> createMacro(const QString& macroname);



			/**
			* Factory method to create @a Action instances from the
			* defined @p element .
			*
			* @param element The serialized QDomElement that should
			* be used to create the @a Action instance.
			* @return A new @a Action instance or NULL if the
			* defined @p element is not valid.
			*
			* @deprecated Move to common XMLReader/XMLWriter classes.
			*/
			KSharedPtr<Action> createAction(const QDomElement& element);



			/**
			* Publish the passed QObject @p object. Those object will
			* provide it's slots as callable functions.
			*/
			bool publishObject(const QString& name, QObject* object);

			/**
			* @return the publish QObject defined with name @p name
			* or NULL if there exists no such object.
			*/
			QGuardedPtr<QObject> object(const QString& name) const;

			/**
			* @return a map of the published QObject instances.
			*/
			QMap<QString, QGuardedPtr<QObject> > objects() const;

		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
	};

}

#endif
