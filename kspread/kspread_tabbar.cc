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

#include <kdebug.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <knotifyclient.h>

namespace KSpread
{

class TabBarPrivate
{
public:
    // the view (also the parent)
    KSpreadView* view;

    // list of visible tabs, in order of appearance
    QStringList visibleTabs;

    // list of hidden tabs
    QStringList hiddenTabs;

    // timer that causes the tabbar to scroll when the user drag a tab.
    QTimer* autoScrollTimer;

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
    int moveTabFlag;

    // indicates the direction the tabs are scrolled to.
    int autoScroll;
};


};

using namespace KSpread;

static int tabbar_text_width( QPainter& painter, const QString& text )
{
    painter.save();

    QFont f = painter.font();
    f.setBold( true );
    painter.setFont( f );

    QFontMetrics fm = painter.fontMetrics();
    int width = fm.width( text );

    painter.restore();

    return width;
}

// creates a new tabbar
TabBar::TabBar( KSpreadView *view )
    : QWidget( view )
{
    d = new TabBarPrivate;
    d->view = view;
    d->leftTab = 1;
    d->rightTab = 0;
    d->activeTab = 0;
    d->moveTab = 0;

    d->autoScroll = 0;
    d->autoScrollTimer = new QTimer(this);
    connect( d->autoScrollTimer, SIGNAL(timeout()), SLOT(slotAutoScroll()));
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

    int i = d->visibleTabs.count();
    int x = 0;
    QStringList::Iterator it = d->visibleTabs.end();
    QPainter painter( this );
    do
    {
      --it;
      x += 10 + tabbar_text_width( painter, *it );
      if ( x > width() )
      {
        d->leftTab = i + 1;
        break;
      }
      --i;
    } while ( it != d->visibleTabs.begin() );

    painter.end();

    repaint( false );
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


void TabBar::slotAdd()
{
    d->view->insertTable();
    d->view->editWidget()->setText( "" );
    d->view->activeTable()->setHidden( false );
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

    if ( d->leftTab > 1 )
        paintTab( painter, -10, QString(""), 0, 0, FALSE );

    int i = 1;
    int x = 0;
    QString text;
    QString active_text;
    int active_x = -1;
    int active_width = 0;
    int active_y = 0;
    bool paint_active = false;

    QStringList::Iterator it;
    for ( it = d->visibleTabs.begin(); it != d->visibleTabs.end(); ++it )
    {
        text = *it;
        int text_width = tabbar_text_width( painter, text );
        QFontMetrics fm = painter.fontMetrics();
        int text_y = ( height() - fm.ascent() - fm.descent() ) / 2 + fm.ascent();

        if ( i >= d->leftTab )
        {
            // the tab is visible
            if( i != d->activeTab )
            {
                if ( d->moveTab == i )
                    paintTab( painter, x, text, text_width, text_y, false, true );
                else
                    paintTab( painter, x, text, text_width, text_y, false );
            }
            else
            {
                // don't paint active tab now
                // it will be painter later
                active_text = text;
                active_x = x;
                active_y = text_y;
                active_width = text_width;
                paint_active = true;
            }

            x += 10 + text_width;
        }

        if ( x - 10 < width() )
                d->rightTab = i;
        i++;
    }

    if( paint_active )
      paintTab( painter, active_x, active_text, active_width, active_y, TRUE );

    painter.end();
    bitBlt( this, 0, 0, &pm );
}


void TabBar::paintTab( QPainter & painter, int x, const QString& text, int text_width, int text_y,
                              bool isactive, bool ismovemarked )
{
    QPointArray parr;
    parr.setPoints( 4, x,0, x+10,height()-1, x+10+text_width,height()-1, x+20+text_width,0 );
    QRegion reg( parr );
    painter.setClipping( TRUE );
    painter.setClipRegion( reg );
    if ( isactive )
        painter.setBackgroundColor( colorGroup().base() );
    else
        painter.setBackgroundColor( colorGroup().background() );
    painter.eraseRect( x, 0, text_width + 20, height() );
    painter.setClipping( FALSE );

    painter.drawLine( x, 0, x + 10, height() - 1 );
    painter.drawLine( x + 10, height() - 1, x + text_width + 10, height() - 1 );
    painter.drawLine( x + 10 + text_width, height() - 1, x + 20 + text_width, 0 );
    if ( !isactive )
        painter.drawLine( x, 0, x + 20 + text_width, 0 );
    if ( ismovemarked )
    {
            if ( d->moveTabFlag == moveTabBefore )
            {
                QPointArray movmark;
                movmark.setPoints(3, x, 0, x + 7, 0, x + 4, 6);
                QBrush oldBrush = painter.brush();
                painter.setBrush( QColor( 0, 0, 0 ) );
                painter.drawPolygon(movmark);
                painter.setBrush( oldBrush );
            }
            else
            {
                QPointArray movmark;
                movmark.setPoints(3, x + 20 + text_width, 0, x + 13 + text_width, 0, x + 16 + text_width, 6);
                QBrush oldBrush = painter.brush();
                painter.setBrush( QColor( 0, 0, 0 ) );
                painter.drawPolygon(movmark);
                painter.setBrush( oldBrush );
            }
    }

    if( isactive )
    {
      painter.save();
      QFont f = painter.font();
      f.setBold( true );
      painter.setFont( f );
      painter.drawText( x + 10, text_y , text );
      painter.restore();
    }
    else
      painter.drawText( x + 10, text_y , text );
}


void TabBar::openPopupMenu( const QPoint &_global )
{
    if ( !d->view->koDocument()->isReadWrite() )
      return;
    d->view->openPopupMenuMenuPage( _global );
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
    int old_active = d->activeTab;

    if ( d->visibleTabs.count() == 0 )
    {
        erase();
        return;
    }

    QPainter painter;
    painter.begin( this );

    int i = 1;
    int x = 0;
    QString text;

    QString active_text = 0L;

    QStringList::Iterator it;
    for ( it = d->visibleTabs.begin(); it != d->visibleTabs.end(); ++it )
    {
        text = *it;
        int text_width = tabbar_text_width( painter, text );

        if ( i >= d->leftTab )
        {
            if ( x <= _ev->pos().x() && _ev->pos().y() <= x + 20 + text_width )
            {
                d->activeTab = i;
                active_text = text ;//text.latin1();
            }

            x += 10 + text_width;
        }
        i++;
    }

    painter.end();

    if ( d->activeTab != old_active )
    {
        repaint( false );
        emit tabChanged( active_text );
    }

    if ( _ev->button() == LeftButton )
    {
        d->moveTabFlag = moveTabBefore;
    }
    else if ( _ev->button() == RightButton )
    {
        openPopupMenu( _ev->globalPos() );
    }
}


void TabBar::mouseReleaseEvent( QMouseEvent* _ev )
{

    if ( !d->view->koDocument()->isReadWrite())
        return;

    if ( _ev->button() == LeftButton && d->moveTab != 0 )
    {
        if ( d->autoScroll != 0 )
        {
            d->autoScrollTimer->stop();
            d->autoScroll = 0;
        }
        d->view->doc()->map()->moveTable( (*d->visibleTabs.at( d->activeTab - 1 )),
                                          (*d->visibleTabs.at( d->moveTab - 1 )),
                                          d->moveTabFlag == moveTabBefore );
        moveTab( d->activeTab - 1, d->moveTab - 1, d->moveTabFlag == moveTabBefore );

        d->moveTabFlag = moveTabNo;
        if ( d->activeTab < d->moveTab && d->moveTabFlag == moveTabBefore )
            d->moveTab--;
        d->activeTab = d->moveTab;

        d->moveTab = 0;
        repaint( false );
    }
}

void TabBar::slotAutoScroll( )
{
    if ( d->autoScroll == autoScrollLeft && d->leftTab > 1 )
    {
        d->moveTab = d->leftTab - 1;
        scrollLeft();
    }
    else if ( d->autoScroll == autoScrollRight )
    {
        scrollRight();
    }
    if ( d->leftTab <= 1 )
    {
        d->autoScrollTimer->stop();
        d->autoScroll = 0;
    }
}


void TabBar::mouseMoveEvent( QMouseEvent* _ev )
{

  if ( !d->view->koDocument()->isReadWrite() )
         return;
    if ( d->moveTabFlag == 0)
        return;

    QPainter painter;
    painter.begin( this );

    if ( _ev->pos().x() < 0 && d->leftTab > 1 && d->autoScroll == 0 )
    {
        d->autoScroll = autoScrollLeft;
        d->moveTab = d->leftTab - 1;
        scrollLeft();
        d->autoScrollTimer->start( 400 );
    }
    else if ( _ev->pos().x() > size().width() )
    {
        int i = d->visibleTabs.count();
        if ( d->activeTab != i && d->moveTab != i && d->activeTab != i - 1 )
        {
            d->moveTabFlag = moveTabAfter;
            d->moveTab = d->visibleTabs.count();
            repaint( false );
        }
        if ( d->rightTab != (int)d->visibleTabs.count() && d->autoScroll == 0 )
        {
            d->autoScroll = autoScrollRight;
            d->moveTab = d->leftTab;
            scrollRight();
            d->autoScrollTimer->start( 400 );
        }
    }
    else // ftf
    {
        int i = 1;
            int x = 0;

        QStringList::Iterator it;
            for ( it = d->visibleTabs.begin(); it != d->visibleTabs.end(); ++it )
        {
            int text_width = tabbar_text_width( painter, *it );

            if ( i >= d->leftTab )
            {
                if ( x <= _ev->pos().x() && _ev->pos().x() <= x + 20 + text_width )
                {
                    if ( d->autoScroll != 0 )
                    {
                        d->autoScrollTimer->stop();
                        d->autoScroll = 0;
                    }

                    if ( ( d->activeTab != i && d->activeTab != i - 1 && d->moveTab != i ) || d->moveTabFlag == moveTabAfter )
                    {
                        d->moveTabFlag = moveTabBefore;
                        d->moveTab = i;
                        repaint( false );
                    }
                    else if ( (d->moveTab != i && d->moveTab != 0) || (d->activeTab == i - 1 && d->moveTab != 0) )
                    {
                        d->moveTab = 0;
                        repaint( false );
                    }
                }
                x += 10 + text_width;
            }
            i++;
        }
        --i;

        if ( x + 10 <= _ev->pos().x() && _ev->pos().x() < size().width() )
        {
            if ( d->activeTab != i && d->moveTabFlag != moveTabAfter )
            {
                d->moveTabFlag = moveTabAfter;
                d->moveTab = i;
                repaint( false );
            }
        }
    }
    painter.end();
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
