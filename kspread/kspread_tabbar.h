/* This file is part of the KDE project
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
   Copyright (C) 2002 Laurent Montel <montel@kde.org>
   Copyright (C) 1999 David Faure <faure@kde.org>
   Copyright (C) 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright (C) 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_TABBAR
#define KSPREAD_TABBAR

#include <qwidget.h>
#include <qstringlist.h>

namespace KSpread
{

class TabBarPrivate;

/**
 * This tab bar is used by @ref KSpreadView. It is used to choose between all
 * available tables.
 *
 * Adding, removing or renaming of tabs does not automatically add, rename or
 * remove KSpreadSheet objects. The tabbar is just a GUI element.
 *
 * But activating a tab emits a signal which in turn will show this table
 * in the associated KSpreadView.
 *
 * @short A bar with tabs and scroll buttons.
 */
class TabBar : public QWidget
{
    Q_OBJECT
public:

    /**
     * Creates a new tabbar.
     */
    TabBar( QWidget* parent = 0, const char *name = 0 );

    /**
     * Destroy the tabbar.
     */
    virtual ~TabBar();

    /**
     * Sets the tab bar to be read only.
     *
     * If the tab bar is read only, tab reordering is not allowed.
     * This means that signal tabMoved, contextMenu and doubleClicked
     * would not be emitted.
     */
    void setReadOnly( bool ro );

    /**
     * Returns true if the tab bar is read only.
     */
    bool readOnly() const;

    /**
     * Adds a tab to the tab bar.
     */
    void addTab( const QString& text );

    /**
     * Removes a tab from the bar. If the tab was the active one then
     * no tab will be active.
     * It is recommended to call setActiveTab after a call to this function.
     */
    void removeTab( const QString& text );

    /**
     * Replaces all tabs with the list of strings.
     */
    void setTabs( const QStringList& list );

    /*
     * Returns all the tab as list of strings.
     */
    QStringList tabs() const;

    /**
     * Renames a tab.
     */
    void renameTab( const QString& old_name, const QString& new_name );

    /**
     * Returns true if it is possible to scroll one tab left.
     */
    bool canScrollLeft() const;

    /**
     * Returns true if it is possible to scroll one tab right.
     */
    bool canScrollRight() const;

    /**
     * Moves a tab to another position and reorder other tabs.
     *
     * Example 1: if there are four tabs A - B - C - D, then
     * moveTab(2,1) will yield A - C - B - D. This is because
     * 2nd tab (i.e C) is moved to a position before 1th tab (i.e B).
     *
     * Example 2: for these tabs: X - Y - Z, moveTab(0,3) will
     * move tab X after tab Z so that the result is Y - Z - X.
     */
    void moveTab( unsigned tab, unsigned target );

public slots:

    void scrollLeft();
    void scrollRight();
    void scrollFirst();
    void scrollLast();

    /**
     * Sets active tab.
     */
    void setActiveTab( const QString& text );

    /**
     * Removes all tabs.
     */
    void clear();

signals:

    /**
     * Emitted if the active tab changed. This will cause the
     * KSpreadView to change its active table, too.
     */
    void tabChanged( const QString& _text );

    /**
     * This signal is emitted whenever a tab is dragged, moved and
     * released. This means that the user wants to move a tab into
     * another position (right before target).
     *
     * When the signal is emitted, the tabs are not reordered.
     * Therefore if you just ignore this signal, than no tab reorder
     * is possible. Call moveTab (from the slot connected to this signal)
     * to perform the actual tab reorder.
     */
    void tabMoved( unsigned tab, unsigned target );

    /**
     * This signal is emitted whenever the tab bar is right-clicked.
     * Typically it is used to popup a context menu.
     */
    void contextMenu( const QPoint& pos );

    /**
     * This signal is emitted whenever the tab bar is double-clicked.
     */
    void doubleClicked();

protected slots:
    void autoScrollLeft();
    void autoScrollRight();

protected:
    virtual void paintEvent ( QPaintEvent* ev );
    virtual void resizeEvent( QResizeEvent* ev );
    virtual void mousePressEvent ( QMouseEvent* ev );
    virtual void mouseReleaseEvent ( QMouseEvent* ev );
    virtual void mouseDoubleClickEvent ( QMouseEvent* ev );
    virtual void mouseMoveEvent( QMouseEvent* ev );

private:
    TabBarPrivate *d;

    // don't allow copy or assignment
    TabBar( const TabBar& );
    TabBar& operator=( const TabBar& );
};

};

// for source compatibility only, remove in the future
typedef KSpread::TabBar KSpreadTabBar;

#endif // KSPREAD_TABBAR
