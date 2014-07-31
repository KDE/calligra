/* This file is part of the KDE project
   Copyright (C) 2006  Olivier Goffart  <ogoffart@kde.org>

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


#ifndef KoMenuMenuHandler_p_h
#define KoMenuMenuHandler_p_h


#include <QObject>


class KoXMLGUIBuilder;
class KMenu;
class KSelectAction;

namespace KDEPrivate {

/**
 * @internal
 * This class handle the context menu of KMenu.
 * Used by KoXmlGuiBuilder
 * @author Olivier Goffart <ogoffart@kde.org>
 */
class KoMenuMenuHandler : public QObject
{
  Q_OBJECT
public:
  KoMenuMenuHandler(KoXMLGUIBuilder *b);
  ~KoMenuMenuHandler() {}
  void insertKMenu( KMenu *menu );
  
  private Q_SLOTS:
    void slotSetShortcut();
    void buildToolbarAction();
    void slotAddToToolBar(int);
    
  private:
    KoXMLGUIBuilder *m_builder;
    KSelectAction *m_toolbarAction;
    
    
};

} //END namespace KDEPrivate

#endif
