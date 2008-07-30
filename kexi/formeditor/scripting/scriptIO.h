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

#ifndef SCRIPTIO_H
#define SCRIPTIO_H

#include <qdom.h>

class QObject;
class Event;
class EventList;
class ScriptManager;
class FormScript;

namespace KFormDesigner {
  class Form;
}

using namespace KFormDesigner;

//! A static class to deal with loading/saving events from/to XML
class ScriptIO
{
  public:
    /*! Save the evnts of a form.
     Creates an \<events\> tag, and then one \<event\> tag for each event.
     Each event contains \<sender\> and \<receiver\> tags, with attributes depending on event type. */
    static bool  saveFormEvents(QDomNode &parentNode, FormScript *script);
    /*! Reads the \<events\> tag (\a parentNode), then creates and fills a FormScript object linked to this \a form.
     The new FormScript object is then added  to ScriptManager list.*/
    static bool  loadFormEvents(QDomNode &parentNode, Form *form, ScriptManager *manager);

    /*! Save only the events related to widget \a name in the FormScript \a fscript.
     Used eg when copying/pasting widgets to keep also events related to it.*/
    static bool  saveAllEventsForWidget(QObject *widget, FormScript *fscript, QDomNode &node);

    static void  saveEvent(Event *event, QDomNode &parentNode);
    static void  saveEventList(EventList *list, QDomNode &parentNode);
    static void  loadEvent(QDomNode &node, EventList *list, Form *form);

  protected:
    ScriptIO() {;}
    ~ScriptIO() {;}
};

#endif

