/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef HANDLER_H
#define HANDLER_H

#include <qobject.h>
#include <koDocumentChild.h>

class QWMatrix;

class KoView;
class PartResizeHandlerPrivate;
class PartMoveHandlerPrivate;

class EventHandler : public QObject
{
    Q_OBJECT
public:
    EventHandler( QObject* target );
    ~EventHandler();

    QObject* target();

private:
    QObject* m_target;
};

class PartResizeHandler : public EventHandler
{
    Q_OBJECT
public:
    PartResizeHandler( QWidget* widget, const QWMatrix& matrix, KoChild* child,
		       KoChild::Gadget gadget, const QPoint& point );
    ~PartResizeHandler();

protected:
    bool eventFilter( QObject*, QEvent* );

private:
    PartResizeHandlerPrivate *d;
};

class PartMoveHandler : public EventHandler
{
    Q_OBJECT
public:
    PartMoveHandler( QWidget* widget, const QWMatrix& matrix, KoChild* child,
		     const QPoint& point );
    ~PartMoveHandler();

protected:
    bool eventFilter( QObject*, QEvent* );

private:
    PartMoveHandlerPrivate *d;
};

class ContainerHandler : public EventHandler
{
    Q_OBJECT
public:
    ContainerHandler( KoView* view, QWidget* widget );
    ~ContainerHandler();

protected:
    bool eventFilter( QObject*, QEvent* );

private:
    KoView* m_view;
};

#endif
