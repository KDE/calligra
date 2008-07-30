/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFORMDESIGNEREVENTS_H
#define KFORMDESIGNEREVENTS_H

#include <q3valuelist.h>
#include <qpointer.h>
//Added by qt3to4:
#include <Q3CString>

#include <kexi_export.h>

class QObject;

//! A simple class to store events
/*! There are three different types of events (an maybe more in the future):
  * signal to slot: sender and receiver are both widgets.
  * signal to user function: whenever the signal is emitted, a function in the form script is called.
  * signal to action: the signal activates an application action (eg addNewRecord in Kexi)
  (other :* global signal to user function: an application global signal (new window opened, etc.) calls a user script function)

 \todo  add aliases for slot()?? (eg actionName())
 */
class KEXICORE_EXPORT Event
{
  public:
    Event(QObject *sender, const Q3CString &signal,
      QObject *receiver, const Q3CString &slot);
    Event(QObject *sender, const Q3CString &signal,
      const Q3CString &functionName);
    Event() : m_type(Slot) {;}
    ~Event() {;}

    enum { Slot=1000, UserFunction, Action }; //! Event types
    int  type() {return  m_type; }
    void  setType(int type) { m_type = type; }

    QObject*  sender() const { return m_sender; }
    QObject*  receiver() const { return m_receiver; }
    Q3CString  signal() const { return m_signal; }
    Q3CString  slot() const { return m_slot; }

    void  setSender(QObject *o) { m_sender = o; }
    void  setReceiver(QObject *o) { m_receiver = o; }
    void  setSignal(const Q3CString &s) { m_signal = s; }
    void  setSlot(const Q3CString &s) { m_slot = s; }

  protected:
    QPointer<QObject> m_sender;
    Q3CString m_signal;
    QPointer<QObject> m_receiver;
    Q3CString m_slot;
    int  m_type;
};

class KEXICORE_EXPORT EventList : protected Q3ValueList<Event*>
{
  public:
    EventList() {;}
    ~EventList() {;}

    /*! Adds an event in list. Other overload are available, so that
     other classes don't have to use Event class in simple cases. */
    void addEvent(Event *event);
    void addEvent(QObject *sender, const Q3CString &signal, QObject *receiver, const Q3CString &slot);
    void addEvent(QObject *sender, const Q3CString &signal, const Q3CString &action);
    /*! Removes the Event \a event from the FormScript's list. */
    void  removeEvent(Event *event);

    /*! \return A list of events related to widget \a name (ie where Event::sender()
     or Event::receiver() == name). */
    EventList*  allEventsForObject(QObject *object);
    /*! Replace all ocurrences of \a oldname with \a newName inside the list. */
    //void  renameWidget(const QCString &oldName, const QCString &newName);
    /*! Removes all events related to widget \a name. Called eg when widget is destroyed. */
    void  removeAllEventsForObject(QObject *object);

    // make some QValueList function accessible by other classes
    Q3ValueListConstIterator<Event*>  constBegin() const { return Q3ValueList<Event*>::constBegin(); } 
    Q3ValueListConstIterator<Event*>  constEnd() const { return Q3ValueList<Event*>::constEnd(); } 
    bool  isEmpty() const { return Q3ValueList<Event*>::isEmpty(); }
};


#endif

