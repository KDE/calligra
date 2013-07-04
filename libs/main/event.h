/* This file is part of the KDE project
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
             (C) 1999 David Faure <faure@kde.org>

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
   Boston, MA 02110-1301, USA.
*/
#ifndef __kparts_event_h__
#define __kparts_event_h__

#include <QtGui/QKeyEvent>

#include <komain_export.h>

class QWidget;

namespace KoParts
{
class Part;

class EventPrivate;
/**
 * Base class for all KoParts events.
 */
class KOMAIN_EXPORT Event : public QEvent
{
public:
  Event( const char *eventName );
  virtual ~Event();
  virtual const char *eventName() const;

  static bool test( const QEvent *event );
  static bool test( const QEvent *event, const char *name );

private:
  EventPrivate * const d;
};

class GUIActivateEventPrivate;
/**
 * This event is sent to a Part when its GUI has been activated or deactivated.
 * This is related to PartActivateEvent, but the difference is that
 * GUIActivateEvent happens later (when the GUI is actually built),
 * only for parts that have GUI elements, and only if using KoParts::MainWindow.
 * @see KoParts::Part::guiActivateEvent()
 */
class KOMAIN_EXPORT GUIActivateEvent : public Event
{
public:
  GUIActivateEvent( bool activated );
  virtual ~GUIActivateEvent();

  bool activated() const;

  static bool test( const QEvent *event );

private:
  GUIActivateEventPrivate * const d;
};

} // namespace

#endif
