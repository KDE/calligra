/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#ifndef __kspread_tabbar_h__
#define __kspread_tabbar_h__

#include <qwidget.h>
#include <qpainter.h>
#include <qstrlist.h>

class KSpreadTableName;

/**
 * This tab bar is used by @ref KSpreadView. It is used to choose between all
 * available tables.
 *
 * @short A bar with tabs and scroll buttons.
 */
class KSpreadTabBar : public QWidget
{
    Q_OBJECT
public:
    KSpreadTabBar( KSpreadView *_parent );

    /**
     * Adds a tab to the bar and paints it. The tab does not become active.
     * call @ref #setActiveTab to do so.
     */
    void addTab( const QString& _text );
    /**
     * Removes the tab from the bar. If the tab was the active one then the one
     * left of it ( or if not available ) the one right of it will become active.
     * It is recommended to call @ref #setActiveTab after a call to this function.
     */
    void removeTab( const QString& _text );
    
	/**
	 * Moves the tab with number _from befor tab number _to
	*/
	void moveTab( int _from, int _to );

	/**
     * Removes all tabs from the bar and repaints the widget.
     */
    void removeAllTabs();
    
    void scrollLeft();
    void scrollRight();    
    void scrollFirst();
    void scrollLast();

    /**
     * Highlights this tab.
     */
    void setActiveTab( const QString& _text );

    /**
     * Opens a dialog to rename active tab.
     */
    void renameTab();

    /**
     * Open a context menu.
     */
    void openPopupMenu( const QPoint &_global );
 
signals:
    void tabChanged( const QString& _text );
   
protected slots:
    void slotRename( );
	void slotRemove( );
 
protected:
    virtual void paintEvent ( QPaintEvent* _ev );
    virtual void mousePressEvent ( QMouseEvent* _ev );
    virtual void mouseReleaseEvent ( QMouseEvent* _ev );
    virtual void mouseDoubleClickEvent ( QMouseEvent* _ev );
	virtual void mouseMoveEvent( QMouseEvent* _ev );

    void paintTab( QPainter & painter, int x, const QString& text, int text_width, int text_y, bool isactive, bool ismovemarked = false );

    void openPopupMenu( QPoint &_global );

    KSpreadView* m_pView;

    /**
     * Pointer to the last popup menu.
     * If this pointer is not 0L delete before usage.
     */
    QPopupMenu *m_pPopupMenu;
    
    /**
     * List with the names of all tabs. The order in this list determines the
     * order of appearance.
     */
    QStringList tabsList;

    /**
     * This is the first visible tab on the left of the bar.
     */
    int leftTab;
    /**
     * The active tab in the range form 1..n.
     * If this value is 0, that means that no tab is active.
     */
    int activeTab;

	/**
	 * Indicates wheter a tab is being moved using the mouse.
	 */ 	
	bool m_bTabMoveFlag;

	/**
	 * The number of the tab being moved using the mouse.
     * If this value is 0, that means that no tab is being moved.
	 */
	int m_moveTab;
};

#endif
