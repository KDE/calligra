/* This file is part of the KDE libraries
   Copyright (C) 2002 Simon Hausmann <hausmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KOTOOLBARHANDLER_H
#define KOTOOLBARHANDLER_H

#include <QtCore/QLinkedList>
#include <QtCore/QObject>
#include <QtCore/QPointer>

#include <xmlgui/KoXMLGUIClient.h>

class KoXmlGuiWindow;

namespace KDEPrivate
{

class ToolBarHandler : public QObject, public KoXMLGUIClient
{
  Q_OBJECT

  public:
    /**
     * Creates a new tool bar handler for the supplied
     * @param mainWindow.
     */
    explicit ToolBarHandler( KoXmlGuiWindow *mainWindow );

    /**
     * Creates a new tool bar handler for the supplied
     * @param mainWindow and with the supplied parent.
     */
    ToolBarHandler( KoXmlGuiWindow *mainWindow, QObject *parent );

    /**
     * Destroys the tool bar handler.
     */
    virtual ~ToolBarHandler();

    /**
     * Returns the action which is responsible for the tool bar menu.
     */
    QAction *toolBarMenuAction();

  public Q_SLOTS:
    void setupActions();

  private:
    class Private;
    Private* const d;

    Q_PRIVATE_SLOT( d, void clientAdded( KoXMLGUIClient* ) )
};

} // namespace KDEPrivate

#endif // KOTOOLBARHANDLER_H
