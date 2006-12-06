/* This file is part of the KDE project
  Copyright (C) 2003 - 2006 Dag Andersen <kplato@kde.org>

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

#ifndef KPTRESOURCEVIEW_H
#define KPTRESOURCEVIEW_H

#include <kptviewbase.h>
#include <QTreeWidget>

#include "kptcontext.h"

class QPoint;
class QTreeWidgetItem;
class QSplitter;

class KToggleAction;
class KPrinter;

namespace KPlato
{
class ResListView;

class View;
class Part;
class Project;
class Resource;
class Node;

class ResourceAppointmentsView;
class ResourceGroup;
class Resource;
class ResourceItemPrivate;

class ResListView : public QTreeWidget
{
    Q_OBJECT
public:
    ResListView( QWidget * parent = 0 );

    int headerHeight() const;
    virtual void paintToPrinter( QPainter *p, int x, int y, int w, int h );
    int calculateY( int ymin, int ymax ) const;
    class DrawableItem
    {
    public:
        DrawableItem( int level, int ypos, QTreeWidgetItem *item ) { y = ypos; l = level; i = item; };
        int y;
        int l;
        QTreeWidgetItem * i;
    };
signals:
    void contextMenuRequested( QTreeWidgetItem*, const QPoint&, int );
protected:
    int buildDrawables( QList<ResListView::DrawableItem*> &lst, int level, int ypos, QTreeWidgetItem *item, int ymin, int ymax ) const;
    // This is a copy of QListView::drawContentsOffset(), with a few changes
    // because drawContentsOffset() only draws *visible* items,
    // we want to draw *all* items.
    // FIXME: Haven't got paintBranches() to work, atm live without it.
    virtual void drawAllContents( QPainter * p, int cx, int cy, int cw, int ch );
private slots:
    void slotContextMenuRequested( const QPoint &p );
};


class ResourceView : public ViewBase
{
    Q_OBJECT

public:
    ResourceView( Part *part, QWidget *parent );

    //~ResourceView();

    void zoom( double zoom );

    void draw( Project &project );

    virtual Resource *currentResource() const;

    QList<int> listOffsets( int pageHeight ) const;
    void print( KPrinter &printer );

    virtual bool setContext( Context::Resourceview &context );
    virtual void getContext( Context::Resourceview &context ) const;

    virtual QSize sizeHint() const;

public slots:
    void setShowAppointments( bool on ) { m_showAppointments = on; }

signals:
    void itemDoubleClicked();
    void requestPopupMenu( const QString&, const QPoint& );
    
protected slots:
    void resSelectionChanged();
    void resSelectionChanged( QTreeWidgetItem *item );
    void slotItemActivated( QTreeWidgetItem* );
    void popupMenuRequested( QTreeWidgetItem * item, const QPoint & pos, int );

private:
    void drawResources( const Project &proj, QTreeWidgetItem *parent, ResourceGroup *group );
    void clearResList();

private:
    int m_defaultFontSize;
    QSplitter *m_splitter;

    Project *m_project;
    ResourceItemPrivate *m_selectedItem;
    ResListView *m_resListView;
    QTreeWidgetItem *m_header;
    ResourceAppointmentsView *m_appview;
    QDate m_start;
    QDate m_end;

    bool m_showAppointments;

};

}  //KPlato namespace

#endif
