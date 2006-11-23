/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATO_VIEWBASE
#define KPLATO_VIEWBASE

#include <KoView.h>

class KoDocument;

class KParts::GUIActivateEvent;
class KXMLGUIFactory;

class QWidget;

namespace KPlato
{

class View;
class Project;
class Node;
class Resource;
class ResourceGroup;

class ViewBase : public KoView
{
    Q_OBJECT
public:
    ViewBase(KoDocument *doc, QWidget *parent);
    ViewBase(View *mainview, QWidget *parent);

    View *mainView() const;
    virtual ~ViewBase() {}

    virtual void setZoom(double /*zoom*/) {}
    virtual void draw() {}
    virtual void draw(Project &/*project*/) {}
    virtual void drawChanges(Project &project) { draw(project); }

    virtual void updateReadWrite( bool );

    /// Reimplement if your view handles nodes
    virtual Node* currentNode() const { return 0; }
    /// Reimplement if your view handles resources
    virtual Resource* currentResource() const { return 0; }
    /// Reimplement if your view handles resource groups
    virtual ResourceGroup* currentResourceGroup() const { return 0; }

public slots:
    virtual void setViewActive( bool active, KXMLGUIFactory *factory=0 );

protected:
    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );
    virtual void addActions( KXMLGUIFactory *factory );
    virtual void removeActions();

    View *m_mainview;

};


} // namespace KPlato

#endif
