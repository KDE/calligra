/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <kplato@kde.org>

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

#include "kptcalendareditor.h"

#include "kcalendar/kdatepicker.h"
#include "kcalendar/kdatetable.h"
//#include "kptcalendarpanel.h"
#include "kptcommand.h"
#include "kptcalendarmodel.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"

#include <QDragMoveEvent>
#include <QList>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>

#include <kaction.h>
#include <kicon.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <ksystemtimezone.h>
#include <ktimezone.h>
#include <kabc/addressee.h>
#include <kabc/vcardconverter.h>

#include <kdebug.h>

#include <KoDocument.h>

namespace KPlato
{


//--------------------
CalendarTreeView::CalendarTreeView( QWidget *parent )
    : TreeViewBase( parent )
{
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new CalendarItemModel() );
    
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setSelectionModel( new QItemSelectionModel( model() ) );

    setItemDelegateForColumn( 1, new EnumDelegate( this ) ); // timezone
    
    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );


}

void CalendarTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column();
}

void CalendarTreeView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<header()->logicalIndexAt(pos)<<" at"<<pos;
}

void CalendarTreeView::contextMenuEvent ( QContextMenuEvent *event )
{
    //kDebug();
    emit contextMenuRequested( indexAt(event->pos()), event->globalPos() );
}

void CalendarTreeView::focusInEvent ( QFocusEvent *event )
{
    //kDebug();
    TreeViewBase::focusInEvent( event );
    emit focusChanged();
}

void CalendarTreeView::focusOutEvent ( QFocusEvent * event )
{
    //kDebug();
    TreeViewBase::focusInEvent( event );
    emit focusChanged();
}

void CalendarTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    //kDebug()<<sel.indexes().count();
    //foreach( QModelIndex i, selectionModel()->selectedIndexes() ) { kDebug()<<i.row()<<","<<i.column(); }
    TreeViewBase::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void CalendarTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    //kDebug();
    TreeViewBase::currentChanged( current, previous );
    selectionModel()->select( current, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows );
    emit currentChanged( current );
}

Calendar *CalendarTreeView::currentCalendar() const
{
    return model()->calendar( currentIndex() );
}

Calendar *CalendarTreeView::selectedCalendar() const
{
    QModelIndexList lst = selectionModel()->selectedRows();
    if ( lst.count() == 1 ) {
        return model()->calendar( lst.first() );
    }
    return 0;
}

QList<Calendar*> CalendarTreeView::selectedCalendars() const
{
    QList<Calendar *> lst;
    foreach ( QModelIndex i, selectionModel()->selectedRows() ) {
        Calendar *a = model()->calendar( i );
        if ( a ) {
            lst << a;
        }
    }
    return lst;
}

void CalendarTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (dragDropMode() == InternalMove && (event->source() != this || !(event->possibleActions() & Qt::MoveAction))) {
        return;
    }
    TreeViewBase::dragMoveEvent( event );
    if ( ! event->isAccepted() ) {
        return;
    }
    // QTreeView thinks it's ok to drop, but it might not be...
    event->ignore();
    QModelIndex index = indexAt( event->pos() );
    if ( ! index.isValid() ) {
        event->accept();
        return; // always ok to drop on main project
    }
    Calendar *c = model()->calendar( index );
    if ( c == 0 ) {
        kError()<<"no calendar to drop on!"<<endl;
        return; // hmmm
    }
    switch ( dropIndicatorPosition() ) {
        case AboveItem:
        case BelowItem:
            // c == sibling
            // if siblings parent is me or child of me: illegal
            if ( model()->dropAllowed( c->parentCal(), event->mimeData() ) ) {
                event->accept();
            }
            break;
        case OnItem:
            // c == new parent
            if ( model()->dropAllowed( c, event->mimeData() ) ) {
                event->accept();
            }
            break;
        default:
            break;
    }
}


//--------------------
CalendarDayView::CalendarDayView( QWidget *parent )
    : QTableView( parent )
{
     m_model = new CalendarDayItemModel( this );
     setModel(m_model);
     verticalHeader()->hide();
     //resizeColumnsToContents();
}

void CalendarDayView::setCurrentCalendar( Calendar *calendar )
{
    model()->setCalendar( calendar );
}

void CalendarDayView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column();
}

void CalendarDayView::headerContextMenuRequested( const QPoint &pos )
{
//    kDebug()<<k_funcinfo<<header()->logicalIndexAt(pos)<<" at"<<pos;
}

void CalendarDayView::contextMenuEvent ( QContextMenuEvent *event )
{
    //kDebug();
    emit contextMenuRequested( indexAt(event->pos()), event->globalPos() );
}

void CalendarDayView::focusInEvent ( QFocusEvent *event )
{
    //kDebug();
//    TreeViewBase::focusInEvent( event );
    emit focusChanged();
}

void CalendarDayView::focusOutEvent ( QFocusEvent * event )
{
    //kDebug();
//    TreeViewBase::focusInEvent( event );
    emit focusChanged();
}

void CalendarDayView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    //kDebug()<<sel.indexes().count();
    //foreach( QModelIndex i, selectionModel()->selectedIndexes() ) { kDebug()<<i.row()<<","<<i.column(); }
//    TreeViewBase::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void CalendarDayView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    //kDebug();
//    TreeViewBase::currentChanged( current, previous );
    selectionModel()->select( current, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows );
    emit currentChanged( current );
}

CalendarDay *CalendarDayView::selectedDay() const
{
    QModelIndexList lst = selectionModel()->selectedRows();
    if ( lst.count() == 1 ) {
        return model()->day( lst.first() );
    }
    return 0;
}

//-----------------------------------
CalendarEditor::CalendarEditor( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_model( new DateTableDataModel( this ) )
{
    setupGui();
    
    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    QSplitter *sp = new QSplitter( this );
    l->addWidget( sp );

    m_calendarview = new CalendarTreeView( sp );
    sp = new QSplitter( sp );
    sp->setOrientation( Qt::Vertical );
    
    sp = new QSplitter( sp );
    sp->setOrientation( Qt::Vertical );
    
    m_dayview = new CalendarDayView( sp );
    
    QFrame *f = new QFrame( sp );
    f->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    l = new QVBoxLayout( f );
    l->setMargin( 0 );

    KDatePicker *w = new KDatePicker( f );
    l->addWidget( w );
    w->dateTable()->setWeekNumbersEnabled( true );
    w->dateTable()->setGridEnabled( true );
    w->dateTable()->setSelectionMode( KDateTable::ExtendedSelection );
    w->dateTable()->setDateDelegate( new DateTableDateDelegate() );
    w->dateTable()->setModel( m_model );
    
/*    const QDate date(2007,7,19);
    const QColor fgColor(Qt::darkGray);
    KDateTable::BackgroundMode bgMode = KDateTable::CircleMode;
    const QColor bgColor( Qt::lightGray);
    w->dateTable()->setCustomDatePainting( date, fgColor, bgMode, bgColor );*/
    
    
    m_calendarview->setEditTriggers( m_calendarview->editTriggers() | QAbstractItemView::EditKeyPressed );
    
    m_dayview->setEditTriggers( m_dayview->editTriggers() | QAbstractItemView::EditKeyPressed );

    m_calendarview->setDragDropMode( QAbstractItemView::InternalMove );
    m_calendarview->setDropIndicatorShown ( true );
    m_calendarview->setDragEnabled ( true );
    m_calendarview->setAcceptDrops( true );
    
    connect( m_calendarview->model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );
    connect( m_dayview->model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );

    connect( m_calendarview, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentCalendarChanged( QModelIndex ) ) );
    connect( m_calendarview, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotCalendarSelectionChanged( const QModelIndexList ) ) );
    connect( m_calendarview, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuCalendar( QModelIndex, const QPoint& ) ) );
    
    connect( m_dayview, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentDayChanged( QModelIndex ) ) );
    connect( m_dayview, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotDaySelectionChanged( const QModelIndexList ) ) );
    connect( m_dayview, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuDay( QModelIndex, const QPoint& ) ) );

    connect( m_dayview->model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotEnableActions() ) );
    
    connect( m_calendarview, SIGNAL( focusChanged() ), this, SLOT( slotEnableActions() ) );
    connect( m_dayview, SIGNAL( focusChanged() ), this, SLOT( slotEnableActions() ) );
    
}

void CalendarEditor::draw( Project &project )
{
    m_calendarview->setProject( &project );
    m_dayview->setProject( &project );
}

void CalendarEditor::draw()
{
}

void CalendarEditor::setGuiActive( bool activate )
{
    //kDebug()<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate ) {
        if ( !m_calendarview->currentIndex().isValid() ) {
            m_calendarview->selectionModel()->setCurrentIndex(m_calendarview->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
        }
        //slotSelectionChanged( m_calendarview->selectionModel()->selectedRows() );
    }
}

void CalendarEditor::slotContextMenuCalendar( QModelIndex index, const QPoint& pos )
{
    //kDebug()<<index.row()<<","<<index.column()<<":"<<pos;
    QString name;
    if ( index.isValid() ) {
        Calendar *a = m_calendarview->model()->calendar( index );
        if ( a ) {
            name = "calendareditor_calendar_popup";
        } else if ( m_dayview->model()->day( index ) ) {
            name = "calendareditor_day_popup";
        }
    }
    //kDebug()<<name;
    if ( name.isEmpty() ) {
        return;
    }
    emit requestPopupMenu( name, pos );
}

void CalendarEditor::slotContextMenuDay( QModelIndex index, const QPoint& pos )
{
    kDebug()<<index.row()<<","<<index.column()<<":"<<pos;
/*    QString name;
    if ( index.isValid() ) {
        if ( m_dayview->model()->day( index ) ) {
            name = "calendareditor_day_popup";
        }
    }
    kDebug()<<name;
    if ( name.isEmpty() ) {
        return;
    }
    emit requestPopupMenu( name, pos );*/
}

Calendar *CalendarEditor::currentCalendar() const
{
    return m_calendarview->currentCalendar();
}

void CalendarEditor::slotCurrentCalendarChanged(  const QModelIndex & )
{
    //kDebug()<<curr.row()<<","<<curr.column();
    m_dayview->setCurrentCalendar( currentCalendar() );
    if ( m_model ) {
        m_model->setCalendar( currentCalendar() );
    }
}

void CalendarEditor::slotCalendarSelectionChanged( const QModelIndexList list )
{
    //kDebug()<<list.count();
    updateActionsEnabled( true );
}

void CalendarEditor::slotCurrentDayChanged(  const QModelIndex & )
{
    //kDebug()<<curr.row()<<","<<curr.column();
}

void CalendarEditor::slotDaySelectionChanged( const QModelIndexList )
{
    //kDebug()<<list.count();
    updateActionsEnabled( true );
}

void CalendarEditor::slotEnableActions()
{
    updateActionsEnabled( true );
}

void CalendarEditor::updateActionsEnabled(  bool on )
{
    QList<Calendar *> lst = m_calendarview->selectedCalendars();
    bool one = lst.count() == 1;
    bool more = lst.count() > 1;
    bool cal = m_calendarview->hasFocus();
    bool day = m_dayview->hasFocus();
    actionAddCalendar ->setEnabled( on && !more && cal );
    actionAddSubCalendar ->setEnabled( on && one && cal );
    actionDeleteSelection->setEnabled( on && ( one || more ) && cal );

    bool o = false;
    TimeInterval *ti = 0;
    CalendarDay *d = m_dayview->selectedDay();
    if ( on ) {
        o = d == 0 ? false : d->state() == CalendarDay::Working;
    }
    actionAddDay->setEnabled( on && day && d == 0 );
    actionAddWorkInterval->setEnabled( on && o && day );
    
    bool act = on && day && d;
    actionDeleteDaySelection->setEnabled( act );
}

void CalendarEditor::setupGui()
{
    KActionCollection *coll = actionCollection();
    QString name = "calendareditor_calendar_list";
    
    actionAddCalendar   = new KAction(KIcon( "document-new" ), i18n("Add Calendar"), this);
    coll->addAction("add_calendar", actionAddCalendar  );
    actionAddCalendar ->setShortcut( KShortcut( Qt::CTRL + Qt::Key_I ) );
    connect( actionAddCalendar , SIGNAL( triggered( bool ) ), SLOT( slotAddCalendar () ) );
    
    actionAddSubCalendar   = new KAction(KIcon( "document-new" ), i18n("Add Subcalendar"), this);
    coll->addAction("add_subcalendar", actionAddSubCalendar  );
    actionAddSubCalendar ->setShortcut( KShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_I ) );
    connect( actionAddSubCalendar , SIGNAL( triggered( bool ) ), SLOT( slotAddSubCalendar () ) );
    
    actionDeleteSelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Calendar"), this);
    coll->addAction("delete_calendar_selection", actionDeleteSelection );
    actionDeleteSelection->setShortcut( KShortcut( Qt::Key_Delete ) );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteCalendar() ) );
    
    addAction( name, actionAddSubCalendar  );
    addAction( name, actionAddCalendar  );
    addAction( name, actionDeleteSelection );
    
    name = "calendareditor_day_list";
    
    actionAddDay   = new KAction(KIcon( "document-new" ), i18n("Add Calendar Day"), this);
    coll->addAction("add_calendarday", actionAddDay  );
    connect( actionAddDay , SIGNAL( triggered( bool ) ), SLOT( slotAddDay() ) );
    
    actionAddWorkInterval   = new KAction(KIcon( "document-new" ), i18n("Add Work Interval"), this);
    coll->addAction("add_workinterval", actionAddWorkInterval  );
    connect( actionAddWorkInterval , SIGNAL( triggered( bool ) ), SLOT( slotAddInterval() ) );
    
    actionDeleteDaySelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Item"), this);
    coll->addAction("delete_day_selection", actionDeleteDaySelection );
    connect( actionDeleteDaySelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteDaySelection() ) );
    
/*    addAction( name, actionAddWorkInterval  );
    addAction( name, actionAddDay  );
    addAction( name, actionDeleteDaySelection );*/
    
}

void CalendarEditor::updateReadWrite( bool readwrite )
{
    m_calendarview->setReadWrite( readwrite );
//    m_dayview->setReadWrite( readwrite );
}

void CalendarEditor::slotAddCalendar ()
{
    //kDebug();
    // get parent through sibling
    Calendar *parent = m_calendarview->selectedCalendar ();
    if ( parent ) {
        parent = parent->parentCal();
    }
    insertCalendar ( new Calendar (), parent );
}

void CalendarEditor::slotAddSubCalendar ()
{
    //kDebug();
    insertCalendar ( new Calendar (), m_calendarview->selectedCalendar () );
}

void CalendarEditor::insertCalendar ( Calendar *calendar, Calendar *parent )
{
    QModelIndex i = m_calendarview->model()->insertCalendar ( calendar, parent );
    if ( i.isValid() ) {
        QModelIndex p = m_calendarview->model()->parent( i );
        //if (parent) kDebug()<<" parent="<<parent->name()<<":"<<p.row()<<","<<p.column();
        //kDebug()<<i.row()<<","<<i.column();
        m_calendarview->setExpanded( p, true );
        m_calendarview->setCurrentIndex( i );
        m_calendarview->edit( i );
    }
}

void CalendarEditor::slotDeleteCalendar()
{
    //kDebug();
    m_calendarview->model()->removeCalendar( m_calendarview->selectedCalendar() );
}

void CalendarEditor::slotAddInterval ()
{
    //kDebug();
/*    CalendarDay *parent = m_dayview->selectedDay ();
    if ( parent == 0 ) {
        TimeInterval *ti = m_dayview->selectedInterval();
        if ( ti == 0 ) {
            return;
        }
        parent = m_dayview->model()->parentDay( ti );
        if ( parent == 0 ) {
            return;
        }
    }
    QModelIndex i = m_dayview->model()->insertInterval( new TimeInterval(), parent );
    if ( i.isValid() ) {
        QModelIndex p = m_dayview->model()->index( parent );
        m_dayview->setExpanded( p, true );
        m_dayview->setCurrentIndex( i );
        m_dayview->edit( i );
    }*/
}

void CalendarEditor::slotDeleteDaySelection()
{
    //kDebug();
/*    TimeInterval *ti = m_dayview->selectedInterval();
    if ( ti != 0 ) {
        m_dayview->model()->removeInterval( ti );
        return;
    }
    CalendarDay *day = m_dayview->selectedDay();
    if ( day != 0 ) {
        m_dayview->model()->removeDay( day );
    }*/
}

void CalendarEditor::slotAddDay ()
{
    //kDebug();
/*    Calendar *c = currentCalendar();
    if ( c == 0 ) {
        return;
    }
    QDate date = QDate::currentDate();
    while ( c->day( date ) ) {
        date = date.addDays( 1 );
    }
    QModelIndex i = m_dayview->model()->insertDay( new CalendarDay(date,  CalendarDay::NonWorking ) );
    if ( i.isValid() ) {
        QModelIndex p = m_dayview->model()->parent( i );
        m_dayview->setExpanded( p, true );
        m_dayview->setCurrentIndex( i );
        m_dayview->edit( i );
    }*/
}


} // namespace KPlato

#include "kptcalendareditor.moc"
