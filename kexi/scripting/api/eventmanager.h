/***************************************************************************
 * eventmanager.h
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

#ifndef KROSS_API_EVENTMANAGER_H
#define KROSS_API_EVENTMANAGER_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qvariant.h>
#include <qsignalmapper.h>
#include <qguardedptr.h>
#include <qobject.h>
#include <kdebug.h>

namespace Kross { namespace Api {

    // Forward declarations.
    class ScriptContainer;
    class QtObject;
    class EventSignal;
    class EventSlot;

    /**
     * The EventManager spends us the bridge between
     * Qt signals and slots and Kross. This class is
     * mainly a signalmapper that manages connections
     * between signals and slots.
     */
    class EventManager : public QObject
    {
            Q_OBJECT

            friend class EventSignal;
            friend class EventSlot;

        public:

            /**
             * Constructor.
             *
             * \param scriptcontainer The \a ScriptContainer instance
             *       used to create this EventManager and that
             *       holds the details to e.g.call functions
             *       within the script.
             * \param qtobj The optional \a QtObject that uses this
             *       EventManager instance to wrap there signals,
             *       slots and properties.
             */
            EventManager(ScriptContainer* scriptcontainer, QtObject* qtobj = 0);

            /**
             * Destructor.
             */
            ~EventManager();

            /**
             * Connect a QObject signal with a script function.
             *
             * \param sender The QObject that is the sender/emitter
             *       of the signal.
             * \param signal The SIGNAL(...) itself.
             * \param function The name of the scriptfunction to
             *       call if the signal got emitted.
             */
            bool connect(QObject *sender, const QCString& signal, const QString& functionname);

            /**
             * Disconnect a QObject signal from a script function.
             */
            bool disconnect(QObject *sender, const QCString& signal, const QString& functionname);

        private:
            /// The \a ScriptContainer connected with this EventManager.
            ScriptContainer* m_scriptcontainer;
            /// The optional \a QtObject instance this EventManager wraps the events for.
            QtObject* m_qtobj;
            /// List of \a EventSignal signals.
            QValueList<EventSignal*> m_signals;
            /// List of \a EventSlot slots.
            QValueList<EventSlot*> m_slots;
    };

}}

#endif

