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

#include "kspread_tabbar.h"

#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_undo.h"
#include "kspread_view.h"

#include <qtimer.h>
#include <qdrawutil.h>
#include <qvaluevector.h>

#include <kdebug.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <knotifyclient.h>

namespace KSpread
{

class TabBarPrivate
{
public:
    // the view and the tabbar
    KSpreadView* view;
    TabBar* tabbar;

    // list of visible tabs, in order of appearance
    QStringList visibleTabs;

    // list of hidden tabs
    QStringList hiddenTabs;

    // array of QRect for each visible tabs
    QValueVector<QRect> tabRects;

    // the first visible tab on the left of the bar.
    int leftTab;

    // the last fully visible tab on the right of the bar.
    int rightTab;

    // the active tab in the range form 1..n.
    // if this value is 0, that means that no tab is active.
    int activeTab;

    // number of the tab being moved using the mouse.
    // if no tab is being moved this value is 0.
    int moveTab;

    // whether a tab is being moved using the mouse and in which direction
    enum { moveTabNo = 0, moveTabBefore, moveTabAfter } moveTabFlag;

    bool autoScroll;

    void layoutTabs();

    int tabAt( const QPoint& pos );

    void drawTab( QPainter& painter, QRect& rect, const QString& text, bool active );
    void drawMoveMarker( QPainter& painter, int x, int y );
};


};

using namespace KSpread;

void TabBarPrivate::layoutTabs()
{
    tabRects.clear();

    QPainter painter( tabbar );

    QFont f = painter.font();
    f.setBold( true );
    painter.setFont( f );
    QFontMetrics fm = painter.fontMetrics();

    int x = 0;
    for( unsigned c = 0; c < visibleTabs.count(); c++ )
    {
      QRect rect;
      if( c >= leftTab-1 )
      {
          QString text = visibleTabs[ c ];
          int tw = fm.width( text ) + 4;
          rect = QRect( x, 0, tw + 20, tabbar->height() );

          x = x + tw + 10;

          if ( x - 10 < tabbar->width() )
              rightTab = c;
      }
      tabRects.append( rect );
    }
}

int TabBarPrivate::tabAt( const QPoint& pos )
{
    for( unsigned i = 0; i < tabRects.count(); i++ )
    {
      QRect rect = tabRects[ i ];
      if( rect.isNull() ) continue;
      if( rect.contains( pos ) ) return i;
    }

    return -1; // not found
}

void TabBarPrivate::drawTab( QPainter& painter, QRect& rect, const QString& text, bool active )
{
    QPointArray pa;
    pa.setPoints( 4, rect.x(), rect.y(), rect.x()+10, rect.bottom()-1,
      rect.right()-10, rect.bottom()-1, rect.right(), rect.top() );

    QColor bgcolor = view->colorGroup().background();
    if( active ) bgcolor = view->colorGroup().base();

    painter.setClipping( true );
    painter.setClipRegion( QRegion( pa ) );
    painter.setBackgroundColor( bgcolor );
    painter.eraseRect( rect );
    painter.setClipping( false );

    painter.drawLine( rect.x(), rect.y(), rect.x()+10, rect.bottom()-1 );
    painter.drawLine( rect.x()+10, rect.bottom()-1, rect.right()-10, rect.bottom()-1 );
    painter.drawLine( rect.right()-10, rect.bottom()-1, rect.right(), rect.top() );
    if( !active )
      painter.drawLine( rect.x(), rect.y(), rect.right(), rect.y() );

    painter.save();
    QFont f = painter.font();
    if( active ) f.setBold( true );
    painter.setFont( f );
    QFontMetrics fm = painter.fontMetrics();
    int tx =  rect.x() + ( rect.width() - fm.width( text ) ) / 2;
    int ty =  rect.y() + ( rect.height() - fm.ascent() - fm.descent() ) / 2 + fm.ascent();
    painter.drawText( tx, ty, text );
    painter.restore();
}

void TabBarPrivate::drawMoveMarker( QPainter& painter, int x, int y )
{
    QPointArray movmark;
    movmark.setPoints( 3, x, y, x + 7, y, x + 4, y + 6);
    QBrush oldBrush = painter.brush();
    painter.setBrush( Qt::black );
    painter.drawPolygon(movmark);
    painter.setBrush( oldBrush );
}

// creates a new tabbar
TabBar::TabBar( KSpreadView *view )
    : QWidget( view )
{
    d = new TabBarPrivate;
    d->view = view;
    d->tabbar = this;
    d->leftTab = 1;
    d->rightTab = 0;
    d->activeTab = 0;

    d->moveTab = 0;
    d->moveTabFlag = TabBarPrivate::moveTabNo;

    d->autoScroll = false;
}

// destroys the tabbar
TabBar::~TabBar()
{
    delete d;
}

// adds a new visible tab
void TabBar::addTab( const QString& text )
{
    d->visibleTabs.append( text );

    update();
}

// removes a tab
void TabBar::removeTab( const QString& text )
{
    int i = d->visibleTabs.findIndex( text );
    if ( i == -1 )
    {
        kdError(36001) << "ERROR: KSpreadSheet '" << text << "' not found" << endl;
        return;
    }

    if ( d->activeTab == i + 1 )
        d->activeTab = i;

    if ( d->activeTab == 0 )
        d->leftTab = 1;
    else if ( d->leftTab > d->activeTab )
        d->leftTab = d->activeTab;

    d->visibleTabs.remove( text );

    update();
}

// removes all tabs
void TabBar::removeAllTabs()
{
    d->visibleTabs.clear();
    d->hiddenTabs.clear();
    d->activeTab = 0;
    d->leftTab = 1;

    update();
}

// returns list of visible tabs
QStringList TabBar::visibleTabs()
{
    return d->visibleTabs;
}

// returns list of hidden tabs
QStringList TabBar::hiddenTabs()
{
    return d->hiddenTabs;
}

// moves a tab to another position
void TabBar::moveTab( int from, int to, bool before )
{
    QStringList::Iterator it;

    it = d->visibleTabs.at( from );
    const QString tabname = *it;

    if ( !before )
        ++to;

    if ( to > (int)d->visibleTabs.count() )
    {
        d->visibleTabs.append( tabname );
        d->visibleTabs.remove( it );
    }
    else if ( from < to )
    {
        d->visibleTabs.insert( d->visibleTabs.at( to ), tabname );
        d->visibleTabs.remove( it );
    }
    else
    {
        d->visibleTabs.remove( it );
        d->visibleTabs.insert( d->visibleTabs.at( to ), tabname );
    }

    repaint();
}

bool TabBar::canScrollLeft() const
{
    if ( d->visibleTabs.count() == 0 )
        return false;

    if ( d->leftTab == 1 )
        return false;

    return true;
}

bool TabBar::canScrollRight() const
{
    if ( d->visibleTabs.count() == 0 )
        return false;

    if ( d->rightTab == (int)d->visibleTabs.count() )
        return false;

    if ( (unsigned int )d->leftTab == d->visibleTabs.count() )
        return false;

    return true;
}

void TabBar::scrollLeft()
{
    if ( !canScrollLeft() )
        return;

    d->leftTab--;
    repaint( false );
}

void TabBar::scrollRight()
{
    if ( !canScrollRight() )
        return;

    d->leftTab++;
    repaint( false );
}

void TabBar::scrollFirst()
{
    if ( !canScrollLeft() )
        return;

    d->leftTab = 1;
    repaint( false );
}

void TabBar::scrollLast()
{
    if ( !canScrollRight() )
        return;

    d->layoutTabs();
    int fullWidth = d->tabRects[ d->tabRects.count()-1 ].right();
    int delta = fullWidth - width();
    for( unsigned i = 0; i < d->tabRects.count(); i++ )
        if( d->tabRects[i].x() > delta ) d->leftTab = i+1;

    update();
}

void TabBar::setActiveTab( const QString& text )
{
    int i = d->visibleTabs.findIndex( text );
    if ( i == -1 )
        return;

    if ( i + 1 == d->activeTab )
        return;

    d->activeTab = i + 1;
    repaint( false );

    emit tabChanged( text );
}

void TabBar::autoScrollLeft()
{
    if( !d->autoScroll ) return;

    scrollLeft();

    if( !canScrollLeft() )
        d->autoScroll = false;
    else
        QTimer::singleShot( 400, this, SLOT( autoScrollLeft() ) );
}

void TabBar::autoScrollRight()
{
    if( !d->autoScroll ) return;

    scrollRight();

    if( !canScrollRight() )
        d->autoScroll = false;
    else
        QTimer::singleShot( 400, this, SLOT( autoScrollRight() ) );
}

void TabBar::paintEvent( QPaintEvent* )
{
    if ( d->visibleTabs.count() == 0 )
    {
        erase();
        return;
    }

    QPainter painter;
    QPixmap pm(size());
    pm.fill( colorGroup().background() );
    painter.begin( &pm, this );

    QBrush fill( colorGroup().brush( QColorGroup::Background ) );
    qDrawShadePanel( &painter, 0, 0, width(),
                     height(), colorGroup(), FALSE, 1, &fill );

    d->layoutTabs();

    // draw first all non-active, visible tabs
    for( unsigned c = 0; c < d->tabRects.count(); c++ )
    {
        QRect rect = d->tabRects[ c ];
        if( rect.isNull() ) continue;
        QString text = d->visibleTabs[ c ];
        d->drawTab( painter, rect, text, false );
    }

    // draw the active tab
    if( d->activeTab > 0 )
    {
        QRect rect = d->tabRects[ d->activeTab-1 ];
        if( !rect.isNull() )
        {
            QString text = d->visibleTabs[ d->activeTab-1 ];
            d->drawTab( painter, rect, text, true );
        }
    }

    // draw the move marker
    if( d->moveTab > 0 )
    {
        QRect rect = d->tabRects[ d->moveTab-1 ];
        if( !rect.isNull() )
        {
            int x = rect.x();
            if ( d->moveTabFlag == TabBarPrivate::moveTabAfter )
              x = rect.right();
            d->drawMoveMarker( painter, x, rect.y() );
        }
    }

    painter.end();
    bitBlt( this, 0, 0, &pm );
}




void TabBar::renameTab( const QString& old_name, const QString& new_name )
{
    QStringList::Iterator it = d->visibleTabs.find( old_name );
    (*it) = new_name;

    update();
}

void TabBar::slotRename()
{
    // Store the current name of the active table
    KSpreadSheet * table = d->view->activeTable();

    bool ok;
    QString activeName = table->tableName();
    QString newName = KLineEditDlg::getText( i18n("Rename Sheet"),i18n("Enter name:"), activeName, &ok, this );

    rename( table, newName, activeName, ok );
}

void TabBar::rename( KSpreadSheet * table, QString newName, QString const & activeName, bool ok )
{
    // Have a different name ?
    if ( ok ) // User pushed an OK button.
    {
        while (!util_validateTableName(newName))
        {
            KNotifyClient::beep();
            KMessageBox::information( this, i18n("Sheet name contains illegal characters. Only numbers and letters are allowed."),
                                      i18n("Change Sheet Name") );

            newName = newName.simplifyWhiteSpace();

            int n = newName.find('-');
            if ( n > -1 )
              newName[n] = '_';

            n = newName.find('!');
            if ( n > -1 )
              newName[n] = '_';

            n = newName.find('$');
            if ( n > -1 )
              newName[n] = '_';

            newName = KLineEditDlg::getText( i18n("Rename Sheet"),i18n("Enter name:"), newName, &ok, this );

            if ( !ok )
              return;
        }

        if ( (newName.stripWhiteSpace()).isEmpty() ) // Table name is empty.
        {
            KNotifyClient::beep();
            KMessageBox::information( this, i18n("Sheet name cannot be empty."), i18n("Change Sheet Name") );
            // Recursion
            slotRename();
        }
        else if ( newName != activeName ) // Table name changed.
        {
            // Is the name already used
            if ( !table->setTableName( newName ) )
            {
                KNotifyClient::beep();
                KMessageBox::information( this, i18n("This name is already used."), i18n("Change Sheet Name") );
                // Recursion
                slotRename();
                return;
            }

            d->view->updateEditWidget();
            d->view->doc()->setModified( true );
        }
    }
}

void TabBar::mousePressEvent( QMouseEvent* _ev )
{
    if ( d->visibleTabs.count() == 0 )
    {
        erase();
        return;
    }

    d->layoutTabs();

    int old_active = d->activeTab;
    for( unsigned i = 0; i < d->tabRects.count(); i++ )
    {
      QRect rect = d->tabRects[ i ];
      if( rect.isNull() ) continue;
      if( rect.contains( _ev->pos() ) )
        d->activeTab = i+1;
    }

    if ( d->activeTab != old_active )
    {
        update();
        emit tabChanged( d->visibleTabs[ d->activeTab-1] );
    }

    if ( _ev->button() == LeftButton )
    {
        d->moveTabFlag = TabBarPrivate::moveTabBefore;
    }
    else if ( _ev->button() == RightButton )
    {
        emit contextMenu( _ev->globalPos() );
    }
}


void TabBar::mouseReleaseEvent( QMouseEvent* _ev )
{
    if ( !d->view->koDocument()->isReadWrite())
        return;

    if ( _ev->button() == LeftButton && d->moveTab != 0 )
    {
        d->autoScroll = false;
        d->view->doc()->map()->moveTable( (*d->visibleTabs.at( d->activeTab - 1 )),
                                          (*d->visibleTabs.at( d->moveTab - 1 )),
                                          d->moveTabFlag == TabBarPrivate::moveTabBefore );
        moveTab( d->activeTab - 1, d->moveTab - 1, d->moveTabFlag == TabBarPrivate::moveTabBefore );

        d->moveTabFlag = TabBarPrivate::moveTabNo;
        if ( d->activeTab < d->moveTab && d->moveTabFlag == TabBarPrivate::moveTabBefore )
            d->moveTab--;
        d->activeTab = d->moveTab;

        d->moveTab = 0;
        repaint( false );
    }
}

void TabBar::mouseMoveEvent( QMouseEvent* _ev )
{
    if ( !d->view->koDocument()->isReadWrite() )
         return;

    if ( d->moveTabFlag == 0)
        return;

    int i = d->tabAt( _ev->pos() ) + 1;
    if( i > 0 )
    {
        if ( ( d->activeTab != i && d->activeTab != i - 1 && d->moveTab != i ) ||
             d->moveTabFlag == TabBarPrivate::moveTabAfter )
        {
            d->moveTabFlag = TabBarPrivate::moveTabBefore;
            d->moveTab = i;
            repaint( false );
        }
        else if ( (d->moveTab != i && d->moveTab != 0) ||
                  (d->activeTab == i - 1 && d->moveTab != 0) )
        {
            d->moveTab = 0;
            repaint( false );
        }
    }

    if ( _ev->pos().x() < 0 && !d->autoScroll  )
    {
        d->autoScroll = true;
        autoScrollLeft();
    }

    if ( _ev->pos().x() > width() && !d->autoScroll )
    {
        d->autoScroll = true;
        autoScrollRight();
    }

    if( rect().contains( _ev->pos() ) ) d->autoScroll = false;
}

void TabBar::mouseDoubleClickEvent( QMouseEvent*  )
{
  if ( !d->view->koDocument()->isReadWrite()|| !d->view->doc()->getShowTabBar() || d->view->activeTable()->isProtected())
        return;
    slotRename();
}

void TabBar::hideTable()
{
    if ( d->visibleTabs.count() ==  1)
    {
        KMessageBox::error( this, i18n("You cannot hide the last visible table.") );
        return;
    }
    else
    {
        if ( !d->view->doc()->undoBuffer()->isLocked() )
        {
                KSpreadUndoHideTable* undo = new KSpreadUndoHideTable( d->view->doc(), d->view->activeTable() );
                d->view->doc()->undoBuffer()->appendUndo( undo );
        }
        d->view->activeTable()->hideTable(true);
    }
}

void TabBar::hideTable(const QString& tableName )
{
  removeTab( tableName );
  d->hiddenTabs.append( tableName );
  emit tabChanged( d->visibleTabs.first() );
}


void TabBar::showTable(const QString& text)
{
    KSpreadSheet *table;
    table=d->view->doc()->map()->findTable( text);
    if ( !d->view->doc()->undoBuffer()->isLocked() )
    {
        KSpreadUndoShowTable* undo = new KSpreadUndoShowTable( d->view->doc(), table );
        d->view->doc()->undoBuffer()->appendUndo( undo );
    }
    table->hideTable(false);
}

void TabBar::showTable(QStringList list)
{
    if ( list.count()==0 )
        return;
    KSpreadSheet *table;
    KSpreadMacroUndoAction *macroUndo=new KSpreadMacroUndoAction( d->view->doc(),i18n("Show Table"));
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    {
        table=d->view->doc()->map()->findTable( *it );
        if ( !d->view->doc()->undoBuffer()->isLocked() )
        {
            KSpreadUndoShowTable* undo = new KSpreadUndoShowTable( d->view->doc(), table );
            macroUndo->addCommand( undo );
        }
        table->hideTable(false);
    }
    d->view->doc()->undoBuffer()->appendUndo( macroUndo );
}


void TabBar::displayTable(const QString& text)
{
    d->hiddenTabs.remove( text );
    addTab( text );
    emit tabChanged( text );
}

void TabBar::addHiddenTab(const QString& text)
{
    d->hiddenTabs.append( text );
}

void TabBar::removeHiddenTab(const QString& text)
{
    d->hiddenTabs.remove( text );
}

#include "kspread_tabbar.moc"
