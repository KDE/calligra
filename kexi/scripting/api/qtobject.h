/***************************************************************************
 * qtobject.h
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef KROSS_API_QTOBJECT_H
#define KROSS_API_QTOBJECT_H

#include <qstring.h>
#include <qobject.h>

#include "class.h"

namespace Kross { namespace Api {

    // Forward declarations.
    class Object;
    class Variant;
    class ScriptContainer;
    //class EventManager;
    class ScriptContrainer;

    /**
     * Class to wrap \a QObject or inherited instances.
     *
     * This class publishs all SIGNAL's, SLOT's and Q_PROPERTY's
     * the QObject has.
     */
    class QtObject : public Kross::Api::Class<QtObject>
    {
        public:

            /**
             * Constructor.
             *
             * \param scriptcontrainer The \a ScriptContrainer
             *       instance this QObject should be
             *       wrapped for.
             * \param object The \a QObject instance this
             *        class wraps.
             * \param name The name this QtObject has.
             */
            QtObject(ScriptContainer* scriptcontainer, QObject* object, const QString& name = "qtobject");

            /**
             * Destructor.
             */
            virtual ~QtObject();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

            /**
             * Return the \a QObject instance this class wraps.
             *
             * \return The wrapped QObject.
             */
            QObject* getObject();

        private:
            ScriptContainer* m_scriptcontainer;
            QObject* m_object;
            //EventManager* m_eventmanager;

            // QProperty's
            Kross::Api::Object::Ptr propertyNames(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr hasProperty(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr getProperty(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setProperty(Kross::Api::List::Ptr);

            // Slots
            Kross::Api::Object::Ptr slotNames(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr hasSlot(Kross::Api::List::Ptr);

            // Signals
            Kross::Api::Object::Ptr signalNames(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr hasSignal(Kross::Api::List::Ptr);

            // Connect a QObject signal with a scripting function
            Kross::Api::Object::Ptr connectSignal(Kross::Api::List::Ptr);

            // Disconnect a QObject signal from a scripting function
            Kross::Api::Object::Ptr disconnectSignal(Kross::Api::List::Ptr);

            // Emit a QObject signal per scripting
            Kross::Api::Object::Ptr emitSignal(Kross::Api::List::Ptr);

            // Call a slot in a QObject per scripting
            Kross::Api::Object::Ptr callSlot(Kross::Api::List::Ptr);
    };

}}

#endif

