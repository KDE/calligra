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

#include "scriptIO.h"
#include "formscript.h"
#include "kexievents.h"

#include "form.h"
#include "objecttree.h"
// Kross includes
#include "main/scriptcontainer.h"

bool
ScriptIO::saveFormEvents(QDomNode &parentNode, FormScript *formScript)
{
  QDomDocument domDoc = parentNode.ownerDocument();

  // Save the form's code
  if(!formScript->getCode().isEmpty()) {
    QDomElement  script = domDoc.createElement("script");
    script.setAttribute("language", formScript->scriptContainer()->getInterpreterName());
    parentNode.appendChild(script);
    QDomText scriptCode = domDoc.createTextNode(formScript->getCode());
    script.appendChild(scriptCode);
  }

  // Save all form events
  if(!formScript->eventList()->isEmpty())
    saveEventList(formScript->eventList(), parentNode);
  return true;
}

bool
ScriptIO::loadFormEvents(QDomNode &parentNode, Form *form, ScriptManager *manager)
{
  QDomElement script = parentNode.namedItem("script").toElement();
  QDomElement events = parentNode.namedItem("events").toElement();

  // Load script code
  FormScript *formScript = new FormScript(form, manager);
  if(!script.isNull()) {
    formScript->scriptContainer()->setInterpreterName(script.attribute("language"));
    formScript->setCode(script.text());
  }

  // Load all events in the EventList
  if(!events.isNull()) {
    for(QDomNode n = events.firstChild(); !n.isNull(); n = n.nextSibling())
      loadEvent(n, formScript->eventList(), form);
  }
  return true;
}

bool
ScriptIO::saveAllEventsForWidget(QObject *widget, FormScript *formScript, QDomNode &node)
{
  EventList *l = formScript->eventList()->allEventsForObject(widget);
  saveEventList(l, node);
  return true;
}

void
ScriptIO::saveEvent(Event *event, QDomNode &parentNode)
{
  if(!event)
    return;

  QDomDocument domDoc = parentNode.ownerDocument();
  QDomElement eventNode = domDoc.createElement("event");
  eventNode.setAttribute("type", event->type());
  parentNode.appendChild(eventNode);

  switch(event->type()) {
    case Event::Slot: {
      QDomElement sender = domDoc.createElement("sender");
      eventNode.appendChild(sender);
      QDomText senderText = domDoc.createTextNode(event->sender() ? event->sender()->name() : "");
      sender.appendChild(senderText);

      QDomElement signal = domDoc.createElement("signal");
      eventNode.appendChild(signal);
      QDomText signalText = domDoc.createTextNode(event->signal());
      signal.appendChild(signalText);

      QDomElement receiver = domDoc.createElement("receiver");
      eventNode.appendChild(receiver);
      QDomText receiverText = domDoc.createTextNode(event->receiver() ? event->receiver()->name() : "");
      receiver.appendChild(receiverText);

      QDomElement slot = domDoc.createElement("slot");
      eventNode.appendChild(slot);
      QDomText slotText = domDoc.createTextNode(event->slot());
      slot.appendChild(slotText);
      break;
    }

    case Event::UserFunction: {
      QDomElement sender = domDoc.createElement("sender");
      eventNode.appendChild(sender);
      QDomText senderText = domDoc.createTextNode(event->sender() ? event->sender()->name() : "");
      sender.appendChild(senderText);

      QDomElement signal = domDoc.createElement("signal");
      eventNode.appendChild(signal);
      QDomText signalText = domDoc.createTextNode(event->signal());
      signal.appendChild(signalText);

      QDomElement function = domDoc.createElement("function");
      eventNode.appendChild(function);
      QDomText functionText = domDoc.createTextNode(event->slot());
      function.appendChild(functionText);
      break;
    }

    case Event::Action:
      // !\todo
      break;

    default:
      break;
  }
}

void
ScriptIO::saveEventList(EventList *list, QDomNode &parentNode)
{
  if(!list || list->isEmpty())
    return;

  QDomDocument domDoc = parentNode.ownerDocument();
  QDomElement events = domDoc.createElement("events");
  parentNode.appendChild(events);

  Q3ValueListConstIterator<Event*> endIt = list->constEnd();
  for(Q3ValueListConstIterator<Event*> it =  list->constBegin(); it != endIt; ++it)
    saveEvent((*it), events);
}

void
ScriptIO::loadEvent(QDomNode &node, EventList *list, Form *form)
{
  int type = node.toElement().attribute("type").toInt();
  Event *event = new Event();

  switch(type) {
    case Event::Slot: {
      ObjectTreeItem *sender = form->objectTree()->lookup(node.namedItem("sender").toElement().text());
      event->setSender(sender ? sender->widget() : 0);
      event->setSignal(node.namedItem("signal").toElement().text().local8Bit());
      ObjectTreeItem *receiver = form->objectTree()->lookup(node.namedItem("receiver").toElement().text());
      event->setReceiver(receiver ? receiver->widget() : 0);
      event->setSlot(node.namedItem("slot").toElement().text().local8Bit());
      event->setType(Event::Slot);
      break;
    }

    case Event::UserFunction: {
      ObjectTreeItem *sender = form->objectTree()->lookup(node.namedItem("sender").toElement().text());
      event->setSender(sender ? sender->widget() : 0);
      event->setSignal(node.namedItem("signal").toElement().text().local8Bit());
      event->setSlot(node.namedItem("function").toElement().text().local8Bit());
      event->setType(Event::UserFunction);
      break;
    }
    default:  break;
  }

  list->addEvent(event);
}

