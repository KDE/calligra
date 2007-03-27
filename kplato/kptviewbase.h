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
#include <QMap>

class KParts::GUIActivateEvent;
class KXMLGUIFactory;

class QWidget;

/// The main namespace
namespace KPlato
{

class View;
class Part;
class Project;
class Node;
class Resource;
class ResourceGroup;
class Calendar;

/** 
 ViewBase is the baseclass of all sub-views to View.

*/
class ViewBase : public KoView
{
    Q_OBJECT
public:
    /// Contructor
    ViewBase(Part *doc, QWidget *parent);
    /// Destructor
    virtual ~ViewBase() {};
    /// Return the part (document) this view handles
    Part *part() const;
    
    virtual void setZoom(double /*zoom*/) {}
    /// Set the project this view shall handle.
    virtual void setProject( Project */*project*/ ) {}
    /// Draw data from current part / project
    virtual void draw() {}
    /// Draw data from project.
    virtual void draw(Project &/*project*/) {}
    /// Draw changed data from project.
    virtual void drawChanges(Project &project) { draw(project); }
    /// Set readWrite mode
    virtual void updateReadWrite( bool );

    /// Reimplement if your view handles nodes
    virtual Node* currentNode() const { return 0; }
    /// Reimplement if your view handles resources
    virtual Resource* currentResource() const { return 0; }
    /// Reimplement if your view handles resource groups
    virtual ResourceGroup* currentResourceGroup() const { return 0; }
    /// Reimplement if your view handles calendars
    virtual Calendar* currentCalendar() const { return 0; }

    /// Returns the list of action lists that shall be plugged/unplugged
    QStringList actionListNames() const { return m_actionListMap.keys(); }
    /// Returns the list of actions associated with the action list name
    QList<QAction*> actionList( const QString name ) const { return m_actionListMap[name]; }
    /// Add an action to the specified action list
    void addAction( const QString list, QAction *action ) { m_actionListMap[list].append( action ); }
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

signals:
    /// Emitted when the gui has been activated or deactivated
    void guiActivated( ViewBase*, bool );
    
protected:
    /// List of all menu/toolbar actions (used for plug/unplug)
    QMap<QString, QList<QAction*> > m_actionListMap;

};


} // namespace KPlato

#endif
