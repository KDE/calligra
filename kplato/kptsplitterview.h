/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATO_SPLITTERVIEW
#define KPLATO_SPLITTERVIEW

#include "kptviewbase.h"

#include <QMap>

class QSplitter;


/// The main namespace
namespace KPlato
{

class View;
class Part;
class Project;
class Resource;
class Context;

/**
 * SplitterView is a view with a vertical QSplitter that can contain
 * other ViewBase based views.
 * This view is created by the main View, and subviews can then be
 * added with addView()
*/
class SplitterView : public ViewBase
{
    Q_OBJECT
public:
    /// Contructor
    SplitterView(Part *doc, QWidget *parent);
    /// Destructor
    virtual ~SplitterView() {}
    
    virtual void setZoom( double zoom );
    /// Set the project this view shall handle.
    virtual void setProject( Project *project );
    /// Draw data from current part / project
    virtual void draw();
    /// Draw data from project.
    virtual void draw( Project &project );
    /// Set readWrite mode
    virtual void updateReadWrite( bool );

    /// Returns the list of action lists that shall be plugged/unplugged
    QStringList actionListNames() const;
    /// Returns the list of actions associated with the action list name
    QList<QAction*> actionList( const QString name ) const;
    
    /// Sets context info to this view. Reimplement.
    virtual bool setContext( const Context &/*context*/ ) { return false; }
    /// Gets context info from this view. Reimplement.
    virtual void getContext( Context &/*context*/ ) const {}
    
    void addView( ViewBase *view );
    ViewBase *findView( const QPoint &pos ) const;

public slots:
    /// Activate/deactivate the gui (also of subviews)
    virtual void setGuiActive( bool activate );

protected slots:
    void slotGuiActivated( ViewBase *v, bool active );
    
private:
    QSplitter *m_splitter;
    ViewBase *m_activeview;
};


} // namespace KPlato

#endif
