/***************************************************************************
 * eventslot.h
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

#ifndef KROSS_API_EVENTSLOT_H
#define KROSS_API_EVENTSLOT_H

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
    class Object;
    class List;
    class QtObject;
    class EventManager;

    /**
     * Each Qt signal and slot connection between a QObject
     * instance and a functionname is represented with
     * a EventSlot and handled by the \a EventManager.
     */
    class EventSlot : protected QObject
    {
            Q_OBJECT
            friend class EventManager;

        public:

            /**
             * Constructor.
             *
             * \param eventmanager The \a EventManager instance
             *       used to create this EventSlot.
             */
            explicit EventSlot(EventManager* eventmanager = 0);

            /**
             * Destructor.
             */
            virtual ~EventSlot() {}

virtual EventSlot* create(EventManager* eventmanager) {
    return new EventSlot(eventmanager);
}
            /**
             * Return the slot matching to the signal.
             *
             * \param signal The signal we should return a
             *       slot for.
             * \return The slot that matches the signal. The
             *        returned QCString is empty/null if
             *        there exists no matching slot.
             */
            virtual QCString getSlot(const QCString& signal);

            /**
             * Connect the event.
             *
             * \param senderobj The sender QObject that emits
             *       the signal.
             * \param signal The signal itself defined with
             *       the Qt SIGNAL(mysignalname()) macro.
             * \param function The name of the function we should
             *        call if the signal got emitted.
             * \param slot The slot to connect with. If empty we
             *        try to determinate the slot from within
             *        the signal by using getSlot(const QCString& signal).
             */
            virtual bool connect(EventManager* eventmanager, QObject* senderobj, const QCString& signal, QString function, const QCString& slot = QCString());

            /**
             * Disconnect the event.
             *
             * \return true if disconnection was successfull
             *        else false.
             */
            virtual bool disconnect();

        private:
            EventManager* m_eventmanager;

            QGuardedPtr<QObject> m_sender;
            QCString m_signal;
            QCString m_slot;
            QString m_function;

            QValueList<EventSlot*> m_slots;

        protected:
            void call(const QVariant&);

        public slots:
            // Stupid signals and slots. To get the passed
            // arguments we need to have all cases of slots
            // avaiable for EventManager::connect() signals.
            void callback();
            void callback(short);
            void callback(int);
            void callback(int, int);
            void callback(int, int, int);
            void callback(int, int, int, int);
            void callback(int, int, int, int, int);
            void callback(int, int, int, int, bool);
            void callback(int, bool);
            void callback(int, int, bool);
            void callback(int, int, const QString&);
            void callback(uint);
            void callback(long);
            void callback(ulong);
            void callback(double);
            void callback(const char*);
            void callback(bool);
            void callback(const QString&);
            void callback(const QString&, int);
            void callback(const QString&, int, int);
            void callback(const QString&, uint);
            void callback(const QString&, bool);
            void callback(const QString&, bool, bool);
            void callback(const QString&, bool, int);
            void callback(const QString&, const QString&);
            void callback(const QString&, const QString&, const QString&);
            void callback(const QStringList&);
            void callback(const QVariant&);
            // The following both slots are more generic to
            // handle Kross::Api::Object instances.
            //void callback(Kross::Api::Object*);
            //void callback(Kross::Api::List*);
    };

/*TODO
    class EventSlotTranslator : protected QObject
    {
            Q_OBJECT
            //friend class EventManager;
        public:
            EventSlotTranslator() {}
            virtual ~EventSlotTranslator() {}
- Nur EINE instanz fuer alle!!!
- connect() muss hier erfolgen. oder ???
  => dann muessen wir uns aber auch QObject, etc. merken :-(
- also mapper;
  IN EVENTSLOT* =>
    void connect(QObject* sender, const QCString& signal, QString function)
    {
      connect(sender,SIGNAL(),EventSlotTranslator*,SLOT())
      connect(EventSlotTranslator*, SIGNAL(callbackSignal()), this, SLOT())
    }
        signals:
            void callbackSignal(Kross::Api::List*);
    };
*/

}}

#endif

