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

#include <qtimer.h>
#include <qdrawutil.h>

#include <stdio.h>
#include <stdlib.h>

#include <kmessagebox.h>
#include <knotifyclient.h>
#include <klineeditdlg.h>
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_tabbar.h"
#include "kspread_map.h"
#include "kspread_undo.h"
#include <kdebug.h>

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

TabBar::TabBar( KSpreadView *_parent )
    : QWidget( (QWidget *)_parent )
{
    m_pView = _parent;

    m_pAutoScrollTimer = new QTimer(this);
    connect( m_pAutoScrollTimer, SIGNAL(timeout()), SLOT(slotAutoScroll()));

    leftTab = 1;
    m_rightTab = 0;
    activeTab = 0;
    m_moveTab = 0;
    m_autoScroll = 0;
}

TabBar::~TabBar()
{
}

void TabBar::addTab( const QString& _text )
{
    tabsList.append( _text );

    update();
}

void TabBar::removeTab( const QString& _text )
{
    int i = tabsList.findIndex( _text );
    if ( i == -1 )
    {
        kdError(36001) << "ERROR: KSpreadSheet '" << _text << "' not found" << endl;
        return;
    }

    if ( activeTab == i + 1 )
        activeTab = i;

    if ( activeTab == 0 )
        leftTab = 1;
    else if ( leftTab > activeTab )
        leftTab = activeTab;

    tabsList.remove( _text );

    update();
}

void TabBar::removeAllTabs()
{
    tabsList.clear();
    tablehide.clear();
    activeTab = 0;
    leftTab = 1;

    update();
}

void TabBar::moveTab( int _from, int _to, bool _before )
{
    QStringList::Iterator it;

    it = tabsList.at( _from );
    const QString tabname = *it;

    if ( !_before )
        ++_to;

    if ( _to > (int)tabsList.count() )
    {
        tabsList.append( tabname );
        tabsList.remove( it );
    }
    else if ( _from < _to )
    {
        tabsList.insert( tabsList.at( _to ), tabname );
        tabsList.remove( it );
    }
    else
    {
        tabsList.remove( it );
        tabsList.insert( tabsList.at( _to ), tabname );
    }

    repaint();
}

bool TabBar::canScrollLeft() const
{
    if ( tabsList.count() == 0 )
        return false;

    if ( leftTab == 1 )
        return false;

    return true;
}

bool TabBar::canScrollRight() const
{
    if ( tabsList.count() == 0 )
        return false;

    if ( m_rightTab == (int)tabsList.count() )
        return false;

    if ( (unsigned int )leftTab == tabsList.count() )
        return false;

    return true;
}

void TabBar::scrollLeft()
{
    if ( !canScrollLeft() )
        return;

    leftTab--;
    repaint( false );
}

void TabBar::scrollRight()
{
    if ( !canScrollRight() )
        return;

    leftTab++;
    repaint( false );
}

void TabBar::scrollFirst()
{
    if ( !canScrollLeft() )
        return;

    leftTab = 1;
    repaint( false );
}

void TabBar::scrollLast()
{
    if ( !canScrollRight() )
        return;

    int i = tabsList.count();
    int x = 0;
    QStringList::Iterator it = tabsList.end();
    QPainter painter( this );
    do
    {
      --it;
      x += 10 + tabbar_text_width( painter, *it );
      if ( x > width() )
      {
        leftTab = i + 1;
        break;
      }
      --i;
    } while ( it != tabsList.begin() );

    painter.end();

    repaint( false );
}

void TabBar::setActiveTab( const QString& _text )
{
    int i = tabsList.findIndex( _text );
    if ( i == -1 )
        return;

    if ( i + 1 == activeTab )
        return;

    activeTab = i + 1;
    repaint( false );

    emit tabChanged( _text );
}


void TabBar::slotAdd()
{
    m_pView->insertTable();
    m_pView->editWidget()->setText("");
    m_pView->activeTable()->setHidden(false);
}

void TabBar::paintEvent( QPaintEvent* )
{

   if ( tabsList.count() == 0 )
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

    if ( leftTab > 1 )
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
    for ( it = tabsList.begin(); it != tabsList.end(); ++it )
    {
        text = *it;
        int text_width = tabbar_text_width( painter, text );
        QFontMetrics fm = painter.fontMetrics();
        int text_y = ( height() - fm.ascent() - fm.descent() ) / 2 + fm.ascent();

        if ( i >= leftTab )
        {
            // the tab is visible
            if( i != activeTab )
            {
                if ( m_moveTab == i )
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
                m_rightTab = i;
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
            if ( m_moveTabFlag == moveTabBefore )
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
    if ( !m_pView->koDocument()->isReadWrite() )
      return;
    m_pView->openPopupMenuMenuPage( _global );
}

void TabBar::renameTab( const QString& old_name, const QString& new_name )
{
    QStringList::Iterator it = tabsList.find( old_name );
    (*it) = new_name;

    update();
}

void TabBar::slotRename()
{
    // Store the current name of the active table
    KSpreadSheet * table = m_pView->activeTable();

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

            m_pView->updateEditWidget();
            m_pView->doc()->setModified( true );
        }
    }
}

void TabBar::mousePressEvent( QMouseEvent* _ev )
{
    int old_active = activeTab;

    if ( tabsList.count() == 0 )
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
    for ( it = tabsList.begin(); it != tabsList.end(); ++it )
    {
        text = *it;
        int text_width = tabbar_text_width( painter, text );

        if ( i >= leftTab )
        {
            if ( x <= _ev->pos().x() && _ev->pos().y() <= x + 20 + text_width )
            {
                activeTab = i;
                active_text = text ;//text.latin1();
            }

            x += 10 + text_width;
        }
        i++;
    }

    painter.end();

    if ( activeTab != old_active )
    {
        repaint( false );
        emit tabChanged( active_text );
    }

    if ( _ev->button() == LeftButton )
    {
        m_moveTabFlag = moveTabBefore;
    }
    else if ( _ev->button() == RightButton )
    {
        openPopupMenu( _ev->globalPos() );
    }
}


void TabBar::mouseReleaseEvent( QMouseEvent* _ev )
{

    if ( !m_pView->koDocument()->isReadWrite())
        return;

    if ( _ev->button() == LeftButton && m_moveTab != 0 )
    {
        if ( m_autoScroll != 0 )
        {
            m_pAutoScrollTimer->stop();
            m_autoScroll = 0;
        }
        m_pView->doc()->map()->moveTable( (*tabsList.at( activeTab - 1 )),
                                          (*tabsList.at( m_moveTab - 1 )),
                                          m_moveTabFlag == moveTabBefore );
        moveTab( activeTab - 1, m_moveTab - 1, m_moveTabFlag == moveTabBefore );

        m_moveTabFlag = moveTabNo;
        if ( activeTab < m_moveTab && m_moveTabFlag == moveTabBefore )
            m_moveTab--;
        activeTab = m_moveTab;

        m_moveTab = 0;
        repaint( false );
    }
}

void TabBar::slotAutoScroll( )
{
    if ( m_autoScroll == autoScrollLeft && leftTab > 1 )
    {
        m_moveTab = leftTab - 1;
        scrollLeft();
    }
    else if ( m_autoScroll == autoScrollRight )
    {
        scrollRight();
    }
    if ( leftTab <= 1 )
    {
        m_pAutoScrollTimer->stop();
        m_autoScroll = 0;
    }
}


void TabBar::mouseMoveEvent( QMouseEvent* _ev )
{

  if ( !m_pView->koDocument()->isReadWrite() )
         return;
    if ( m_moveTabFlag == 0)
        return;

    QPainter painter;
    painter.begin( this );

    if ( _ev->pos().x() < 0 && leftTab > 1 && m_autoScroll == 0 )
    {
        m_autoScroll = autoScrollLeft;
        m_moveTab = leftTab - 1;
        scrollLeft();
        m_pAutoScrollTimer->start( 400 );
    }
    else if ( _ev->pos().x() > size().width() )
    {
        int i = tabsList.count();
        if ( activeTab != i && m_moveTab != i && activeTab != i - 1 )
        {
            m_moveTabFlag = moveTabAfter;
            m_moveTab = tabsList.count();
            repaint( false );
        }
        if ( m_rightTab != (int)tabsList.count() && m_autoScroll == 0 )
        {
            m_autoScroll = autoScrollRight;
            m_moveTab = leftTab;
            scrollRight();
            m_pAutoScrollTimer->start( 400 );
        }
    }
    else // ftf
    {
        int i = 1;
            int x = 0;

        QStringList::Iterator it;
            for ( it = tabsList.begin(); it != tabsList.end(); ++it )
        {
            int text_width = tabbar_text_width( painter, *it );

            if ( i >= leftTab )
            {
                if ( x <= _ev->pos().x() && _ev->pos().x() <= x + 20 + text_width )
                {
                    if ( m_autoScroll != 0 )
                    {
                        m_pAutoScrollTimer->stop();
                        m_autoScroll = 0;
                    }

                    if ( ( activeTab != i && activeTab != i - 1 && m_moveTab != i ) || m_moveTabFlag == moveTabAfter )
                    {
                        m_moveTabFlag = moveTabBefore;
                        m_moveTab = i;
                        repaint( false );
                    }
                    else if ( (m_moveTab != i && m_moveTab != 0) || (activeTab == i - 1 && m_moveTab != 0) )
                    {
                        m_moveTab = 0;
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
            if ( activeTab != i && m_moveTabFlag != moveTabAfter )
            {
                m_moveTabFlag = moveTabAfter;
                m_moveTab = i;
                repaint( false );
            }
        }
    }
    painter.end();
}

void TabBar::mouseDoubleClickEvent( QMouseEvent*  )
{
  if ( !m_pView->koDocument()->isReadWrite()|| !m_pView->doc()->getShowTabBar() || m_pView->activeTable()->isProtected())
        return;
    slotRename();
}

void TabBar::hideTable()
{
    if ( tabsList.count() ==  1)
    {
        KMessageBox::error( this, i18n("You cannot hide the last visible table.") );
        return;
    }
    else
    {
        if ( !m_pView->doc()->undoBuffer()->isLocked() )
        {
                KSpreadUndoHideTable* undo = new KSpreadUndoHideTable( m_pView->doc(), m_pView->activeTable() );
                m_pView->doc()->undoBuffer()->appendUndo( undo );
        }
        m_pView->activeTable()->hideTable(true);
    }
}

void TabBar::hideTable(const QString& tableName )
{
  removeTab( tableName );
  tablehide.append( tableName );
  emit tabChanged( tabsList.first() );
}


void TabBar::showTable(const QString& text)
{
    KSpreadSheet *table;
    table=m_pView->doc()->map()->findTable( text);
    if ( !m_pView->doc()->undoBuffer()->isLocked() )
    {
        KSpreadUndoShowTable* undo = new KSpreadUndoShowTable( m_pView->doc(), table );
        m_pView->doc()->undoBuffer()->appendUndo( undo );
    }
    table->hideTable(false);
}

void TabBar::showTable(QStringList list)
{
    if ( list.count()==0 )
        return;
    KSpreadSheet *table;
    KSpreadMacroUndoAction *macroUndo=new KSpreadMacroUndoAction( m_pView->doc(),i18n("Show Table"));
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    {
        table=m_pView->doc()->map()->findTable( *it );
        if ( !m_pView->doc()->undoBuffer()->isLocked() )
        {
            KSpreadUndoShowTable* undo = new KSpreadUndoShowTable( m_pView->doc(), table );
            macroUndo->addCommand( undo );
        }
        table->hideTable(false);
    }
    m_pView->doc()->undoBuffer()->appendUndo( macroUndo );
}


void TabBar::displayTable(const QString& text)
{
    tablehide.remove( text );
    addTab( text );
    emit tabChanged( text );
}
void TabBar::addHiddenTab(const QString & text)
{
    tablehide.append( text );
}

void TabBar::removeHiddenTab(const QString & text)
{
    tablehide.remove( text );
}

#include "kspread_tabbar.moc"
