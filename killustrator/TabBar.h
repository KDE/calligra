/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef __tabbar_h__
#define __tabbar_h__

#include <qwidget.h>
#include <qpainter.h>
#include <qstrlist.h>

//class KivioPageName;
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
     * Adds a tab to the bar and paints it. The tab does not become active.
     * call @ref #setActiveTab to do so.
     */
    void addTab( const QString& _text );
    /**
     * Adds a hidden tab.
     *
     * @see KivioView::setActivePage
     */
    void addHiddenTab( const QString & text );
    /**
     * Removes the tab from the bar. If the tab was the active one then the one
     * left of it ( or if not available ) the one right of it will become active.
     * It is recommended to call @ref #setActiveTab after a call to this function.
     */
    void removeTab( const QString& _text );

    /**
     * Renames a tab.
     */
    void renameTab( const QString& old_name, const QString& new_name );

    /**
     * Moves the tab with number _from befor tab number _to if @param _before is
     * true _from is inserted before _to. If false it is inserted after.
     */
    void moveTab( int _from, int _to, bool _before = true );

    /**
     * Removes all tabs from the bar and repaints the widget.
     */
    void removeAllTabs();

    /**
     * Highlights this tab.
     */
    void setActiveTab( int a );

    /**
     * Open a context menu.
     */
    void openPopupMenu( const QPoint &_global );
    /**
    * Remove page name from tabList and
    * put pagename in pagehide
    * and highlights first name in tabList
    */
    void hidePage();

    /**
     * Shows the page. This makes only sense if
     * the page was hiddem before.
     *
     * The page does not become automatically the active one.
     */
    void showPage(const QString& _text);

    /**
     * @return the name of all visible pages.
     */
    QStringList listshow(){return  tabsList;}
    /**
     * @return the name of all hidden pages.
     */
    QStringList listhide(){return  pagehide;}

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
    void slotAutoScroll( );

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
     * List with the names of all tabs. The order in this list determines the
     * order of appearance.
     */
    QStringList tabsList;
     /*
    * list which contain names of page hide
    */
    QStringList pagehide;
    /**
     * Timer that causes the tabbar to scroll when the user drag a tab.
     */
    QTimer* m_pAutoScrollTimer;

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
    int m_autoScroll;
};

#endif
