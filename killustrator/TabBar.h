/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __tabbar_h__
#define __tabbar_h__

#include <qwidget.h>
#include <qpainter.h>
#include <qstrlist.h>

class KIllustratorView;
class GDocument;

/**
 * This tab bar is used by @ref KivioView. It is used to choose between all
 * available pages.
 *
 * Adding, removing or renaming of tabs does not automatically add, rename or
 * remove KivioPage objects. The tabbar is just a GUI element.
 *
 * But activating a tab emits a signal which in turn will show this page
 * in the associated KivioView.
 *
 * @short A bar with tabs and scroll buttons.
 */
class TabBar : public QWidget {
  Q_OBJECT
public:
    TabBar( QWidget* parent, KIllustratorView *view );

    /**
     * Moves the tab with number _from befor tab number _to if @param _before is
     * true _from is inserted before _to. If false it is inserted after.
     */
    void moveTab( int _from, int _to, bool _before = true );

    /**
     * Highlights this tab.
     */
    void setActiveTab();

    /**
     * Open a context menu.
     */
    void openPopupMenu( const QPoint &_global );

signals:
    /**
     * Emitted if the active tab changed. This will cause the
     * KivioView to change its active page, too.
     */
    void tabChanged( int );


public slots:
    void scrollLeft();
    void scrollRight();
    void scrollFirst();
    void scrollLast();

protected slots:
    /**
     * Opens a dialog to rename active tab.
     */
    void slotRename( );
    void slotRemove( );
    void slotAdd();

protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent ( QMouseEvent* _ev );
    virtual void mouseReleaseEvent ( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent ( QMouseEvent* _ev );
    virtual void mouseMoveEvent( QMouseEvent* _ev );

    void paintTab( QPainter & painter, int x, const QString& text, int text_width, int text_y, bool isactive, bool ismovemarked = false );

    void openPopupMenu( QPoint &_global );

    KIllustratorView* m_pView;
    GDocument *doc;

    enum { autoScrollNo = 0, autoScrollLeft, autoScrollRight };
    enum { moveTabNo = 0, moveTabBefore, moveTabAfter };
    /**
     * Pointer to the last popup menu.
     * If this pointer is not 0L delete before usage.
     */
    QPopupMenu *m_pPopupMenu;

    /**
     * Timer that causes the tabbar to scroll when the user drag a tab.
     */
//    QTimer* m_pAutoScrollTimer;

    /**
     * This is the first visible tab on the left of the bar.
     */
    int leftTab;

    /**
     * This is the last fully visible tab on the right of the bar.
     */
    int m_rightTab;

    /**
     * The active tab in the range form 1..n.
     * If this value is 0, that means that no tab is active.
     */
    int activeTab;

    /**
     * Indicates whether a tab may be scrolled while moving a page.
     * Used to provide a timeout.
     */
    bool m_mayScroll;

    /**
     * The number of the tab being moved using the mouse.
     * If no tab is being moved this value is 0.
     */
    int m_moveTab;
	
    /**
     * Indicates whether a tab is being moved using the mouse and in which
     * direction.
     */ 	
    int m_moveTabFlag;

    /**
     * Indicates the direction the tabs are scrolled to.
     */
//    int m_autoScroll;
};

#endif
