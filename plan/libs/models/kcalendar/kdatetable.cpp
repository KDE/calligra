/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2007 John Layt <john@layt.net>
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

#include "kdatetable.h"

#include "kdatepicker.h"
#include "kptdebug.h"

#include <kconfig.h>
#include <knotification.h>
#include <kstandardshortcut.h>
#include <kactioncollection.h>

#include <QLocale>
#include <QFontDatabase>
#include <QDate>
#include <QPen>
#include <QPainter>
#include <QActionEvent>
#include <QMenu>
#include <QHash>
#include <QToolTip>
#include <qdrawutil.h>

#include <cmath>

namespace KPlato
{

class KDateTable::KDateTablePrivate
{
public:
    KDateTablePrivate(KDateTable *q): q(q)
    {
        m_popupMenuEnabled = false;
        m_selectionmode = KDateTable::SingleSelection;
        m_paintweeknumbers = false;
        m_hoveredPos = -1;
        m_model = 0;
        m_grid = false;
    }

    ~KDateTablePrivate()
    {
        qDeleteAll(customPaintingModes);
        delete m_dateDelegate;
        delete m_weekDayDelegate;
        delete m_weekNumberDelegate;
    }

    void nextMonth();
    void previousMonth();
    void beginningOfMonth();
    void endOfMonth();
    void beginningOfWeek();
    void endOfWeek();

    KDateTable *q;

    /**
     * The currently selected date.
     */
    QDate m_date;
    /**
     * The weekday number of the first day in the month [1..7].
     */
    int m_weekDayFirstOfMonth;
    /**
     * Save the size of the largest used cell content.
     */
    QRectF m_maxCell;
    /**
     * The font size of the displayed text.
     */
    int fontsize;

    bool m_popupMenuEnabled;

    //----->
    QHash <qint64, KDateTableDateDelegate*> customPaintingModes;

    int m_hoveredPos;

    KDateTableDataModel *m_model;

    KDateTableDateDelegate *m_dateDelegate;
    KDateTableWeekDayDelegate *m_weekDayDelegate;
    KDateTableWeekNumberDelegate *m_weekNumberDelegate;

    StyleOptionViewItem m_styleOptionDate;
    StyleOptionHeader m_styleOptionWeekDay;
    StyleOptionHeader m_styleOptionWeekNumber;

    QList<QDate> m_selectedDates;
    SelectionMode m_selectionmode;

    bool m_paintweeknumbers;
    bool m_grid;
};


KDateTable::KDateTable(const QDate& date, QWidget* parent)
  : QWidget(parent),
    d(new KDateTablePrivate(this))
{
    if (!date.isValid()) {
        debugPlan << "KDateTable ctor: WARNING: Given date is invalid, using current date.";
        initWidget(QDate::currentDate()); // this initializes m_weekDayFirstOfMonth, m_numDaysThisMonth, numDaysPrevMonth
    } else {
        initWidget(date); // this initializes m_weekDayFirstOfMonth, m_numDaysThisMonth, numDaysPrevMonth
    }
}

KDateTable::KDateTable(QWidget *parent)
  : QWidget(parent),
    d(new KDateTablePrivate(this))
{
    initWidget(QDate::currentDate());
}

KDateTable::~KDateTable()
{
  delete d;
}

void KDateTable::initWidget(const QDate &date)
{
    setFontSize(10);
    setFocusPolicy(Qt::StrongFocus);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    initAccels();
    setAttribute(Qt::WA_Hover, true);

    d->m_dateDelegate = new KDateTableDateDelegate( this );
    d->m_weekDayDelegate = new KDateTableWeekDayDelegate( this );
    d->m_weekNumberDelegate = new KDateTableWeekNumberDelegate( this );

    d->m_styleOptionDate.initFrom( this );
    d->m_styleOptionDate.displayAlignment = Qt::AlignCenter;

    d->m_styleOptionWeekDay.initFrom( this );
    d->m_styleOptionWeekDay.textAlignment = Qt::AlignCenter;

    d->m_styleOptionWeekNumber.initFrom( this );
    d->m_styleOptionWeekNumber.textAlignment = Qt::AlignCenter;

    //setModel( new KDateTableDataModel( this ) );
    setDate(date);
}

void KDateTable::setStyleOptionDate( const StyleOptionViewItem &so )
{
    d->m_styleOptionDate = so;
}

void KDateTable::setStyleOptionWeekDay( const StyleOptionHeader &so )
{
    d->m_styleOptionWeekDay = so;
}

void KDateTable::setStyleOptionWeekNumber( const StyleOptionHeader &so )
{
    d->m_styleOptionWeekNumber = so;
}

void KDateTable::slotReset()
{
    update();
}

void KDateTable::slotDataChanged( const QDate &start, const QDate &end )
{
    Q_UNUSED(start);
    Q_UNUSED(end);
    update();
}

void KDateTable::setModel( KDateTableDataModel *model )
{
    if ( d->m_model )
    {
      disconnect( d->m_model, SIGNAL( reset() ), this, SLOT( slotReset() ) );
    }
    d->m_model = model;
    if ( d->m_model )
    {
      connect( d->m_model, SIGNAL( reset() ), this, SLOT( slotReset() ) );
    }
    update();
}

KDateTableDataModel *KDateTable::model() const
{
    return d->m_model;
}

void KDateTable::setDateDelegate( KDateTableDateDelegate *delegate )
{
    delete d->m_dateDelegate;
    d->m_dateDelegate = delegate;
}

void KDateTable::setDateDelegate( const QDate &date, KDateTableDateDelegate *delegate )
{
    delete d->customPaintingModes.take(date.toJulianDay());
    d->customPaintingModes.insert(date.toJulianDay(), delegate);
}

void KDateTable::setWeekDayDelegate( KDateTableWeekDayDelegate *delegate )
{
    delete d->m_weekDayDelegate;
    d->m_weekDayDelegate = delegate;
}

void KDateTable::setWeekNumberDelegate( KDateTableWeekNumberDelegate *delegate )
{
    delete d->m_weekNumberDelegate;
    d->m_weekNumberDelegate = delegate;
}

void KDateTable::setWeekNumbersEnabled( bool enable )
{
    d->m_paintweeknumbers = enable;
}

void KDateTable::setGridEnabled( bool enable )
{
    d->m_grid = enable;
}

void KDateTable::initAccels()
{
  KActionCollection* localCollection = new KActionCollection(this);
  localCollection->addAssociatedWidget(this);

  QAction* next = localCollection->addAction(QLatin1String("next"));
  next->setShortcuts(KStandardShortcut::next());
  connect(next, SIGNAL(triggered(bool)), SLOT(nextMonth()));

  QAction* prior = localCollection->addAction(QLatin1String("prior"));
  prior->setShortcuts(KStandardShortcut::prior());
  connect(prior, SIGNAL(triggered(bool)), SLOT(previousMonth()));

  QAction* beginMonth = localCollection->addAction(QLatin1String("beginMonth"));
  beginMonth->setShortcuts(KStandardShortcut::home());
  connect(beginMonth, SIGNAL(triggered(bool)), SLOT(beginningOfMonth()));

  QAction* endMonth = localCollection->addAction(QLatin1String("endMonth"));
  endMonth->setShortcuts(KStandardShortcut::end());
  connect(endMonth, SIGNAL(triggered(bool)), SLOT(endOfMonth()));

  QAction* beginWeek = localCollection->addAction(QLatin1String("beginWeek"));
  beginWeek->setShortcuts(KStandardShortcut::beginningOfLine());
  connect(beginWeek, SIGNAL(triggered(bool)), SLOT(beginningOfWeek()));

  QAction* endWeek = localCollection->addAction("endWeek");
  endWeek->setShortcuts(KStandardShortcut::endOfLine());
  connect(endWeek, SIGNAL(triggered(bool)), SLOT(endOfWeek()));

  localCollection->readSettings();
}

int KDateTable::posFromDate( const QDate &date )
{
    int initialPosition = date.day();
    int offset = (d->m_weekDayFirstOfMonth - QLocale().firstDayOfWeek() + 7) % 7;

    // make sure at least one day of the previous month is visible.
    // adjust this <1 if more days should be forced visible:
    if ( offset < 1 ) {
        offset += 7;
    }

    return initialPosition + offset;
}

QDate KDateTable::dateFromPos( int position )
{
    int offset = (d->m_weekDayFirstOfMonth - QLocale().firstDayOfWeek() + 7) % 7;

    // make sure at least one day of the previous month is visible.
    // adjust this <1 if more days should be forced visible:
    if ( offset < 1 ) {
        offset += 7;
    }

    return QDate(d->m_date.year(), d->m_date.month(), 1).addDays(position - offset);
}

void KDateTable::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    const QRect &rectToUpdate = e->rect();
    double cellWidth = width() / ( d->m_paintweeknumbers ?  8.0 : 7.0 );
    double cellHeight = height() / 7.0;
    int leftCol = (int)std::floor(rectToUpdate.left() / cellWidth);
    int topRow = (int)std::floor(rectToUpdate.top() / cellHeight);
    int rightCol = (int)std::ceil(rectToUpdate.right() / cellWidth);
    int bottomRow = (int)std::ceil(rectToUpdate.bottom() / cellHeight);
    bottomRow = qMin(bottomRow, 7 - 1);
    rightCol = qMin(rightCol, ( d->m_paintweeknumbers ?  8 : 7 ) - 1);
    if (layoutDirection() == Qt::RightToLeft) {
        p.translate((( d->m_paintweeknumbers ?  8 : 7 ) - leftCol - 1) * cellWidth, topRow * cellHeight);
    } else {
        p.translate(leftCol * cellWidth, topRow * cellHeight);
    }
    for (int i = leftCol; i <= rightCol; ++i) {
        for (int j = topRow; j <= bottomRow; ++j) {
            paintCell(&p, j, i);
            p.translate(0, cellHeight);
        }

        if (layoutDirection() == Qt::RightToLeft) {
            p.translate(-cellWidth, 0);
        } else {
            p.translate(cellWidth, 0);
        }
        p.translate(0, -cellHeight * (bottomRow - topRow + 1));
    }
}

void KDateTable::paintCell(QPainter *painter, int row, int column)
{
  //debugPlan;

    double w = (width() / ( d->m_paintweeknumbers ? 8.0 : 7.0 )) - 1;
    double h = (height() / 7.0) - 1;
    QRectF rect( 0, 0, w, h );
    QSizeF cell;

    if ( row == 0 ) {
        if (column == 0 && d->m_paintweeknumbers ) {
            // paint something in the corner??
    /*        painter->setPen(palette().color(QPalette::Text));
            painter->drawRect( rect );*/
            return;
        }
        // we are drawing the headline (weekdays)
        d->m_styleOptionWeekDay.rectF = rect;
        d->m_styleOptionWeekDay.state = QStyle::State_None;

        int col = d->m_paintweeknumbers ? column - 1 : column;

        int day = col + QLocale().firstDayOfWeek();
        if (day >= 8 ) {
            day -= 7;
        }
        if ( d->m_weekDayDelegate ) {
            cell = d->m_weekDayDelegate->paint( painter, d->m_styleOptionWeekDay, day, d->m_model ).size();
        }
    } else {
        if ( d->m_paintweeknumbers && column == 0 ) {
            d->m_styleOptionWeekNumber.rectF = rect;
            d->m_styleOptionWeekNumber.state = QStyle::State_None;

            int pos = 7 * (row-1);
            QDate pCellDate = dateFromPos( pos );
            if ( d->m_weekNumberDelegate ) {
                cell = d->m_weekNumberDelegate->paint( painter, d->m_styleOptionWeekNumber, pCellDate.weekNumber(), d->m_model ).size();
            }
        } else {
            // draw the dates
            int col = d->m_paintweeknumbers ? column - 1 : column;
            int pos = 7 * (row-1) + col;

            if ( d->m_grid ) {
                painter->save();
                // TODO: do not hardcode color!
                QPen pen( "lightgrey" );
                pen.setWidthF( 0.5 );
                painter->setPen( pen );
                double pw = painter->pen().width();
                if ( col > 0 ) {
                    painter->drawLine( rect.topLeft(), rect.bottomLeft() );
                }
                if ( row > 1 ) {
                    painter->drawLine( rect.topLeft(), rect.topRight() );
                }
                rect = rect.adjusted(pw, pw, 0, 0 );
                painter->restore();
                //debugPlan<<d->m_grid<<" "<<pw<<" "<<rect;
            }

            d->m_styleOptionDate.rectF = rect;
            d->m_styleOptionDate.state = QStyle::State_None;

            QDate pCellDate = dateFromPos( pos );
            if( pCellDate.month() == d->m_date.month() ) {
                d->m_styleOptionDate.state |= QStyle::State_Active;
            }
            if ( d->m_selectedDates.contains( pCellDate ) ) {
                d->m_styleOptionDate.state |= QStyle::State_Selected;
            }
            if ( isEnabled() ) {
                d->m_styleOptionDate.state |= QStyle::State_Enabled;
                if (pos == d->m_hoveredPos) {
                    d->m_styleOptionDate.state |= QStyle::State_MouseOver;
                }
            }
            if ( pCellDate == d->m_date ) {
                d->m_styleOptionDate.state |= QStyle::State_Active;
                if ( d->m_selectionmode != SingleSelection && hasFocus() ) {
                    d->m_styleOptionDate.state |= QStyle::State_HasFocus;
                }
            }
            KDateTableDateDelegate *del = d->customPaintingModes.value( pCellDate.toJulianDay() );
            if ( del == 0 ) {
                del = d->m_dateDelegate;
            }
            if ( del ) {
                //debugPlan<<del;
                cell = del->paint( painter, d->m_styleOptionDate, pCellDate, d->m_model ).size();
            } else {
                warnPlan<<"No delegate!";
            }
        }
    }

    // If the day cell we just drew is bigger than the current max cell sizes,
    // then adjust the max to the current cell
    if (cell.width() > d->m_maxCell.width()) {
        d->m_maxCell.setWidth(cell.width());
    }
    if (cell.height() > d->m_maxCell.height()) {
        d->m_maxCell.setHeight(cell.height());
    }
}

void KDateTable::KDateTablePrivate::nextMonth()
{
    // setDate does validity checking for us
    q->setDate(m_date.addMonths(1));
}

void KDateTable::KDateTablePrivate::previousMonth()
{
    // setDate does validity checking for us
    q->setDate(m_date.addMonths(-1));
}

void KDateTable::KDateTablePrivate::beginningOfMonth()
{
    // setDate does validity checking for us
    q->setDate(QDate(m_date.year(), m_date.month(), 1));
}

void KDateTable::KDateTablePrivate::endOfMonth()
{
    // setDate does validity checking for us
    q->setDate(QDate(m_date.year(), m_date.month() + 1, 0));
}

void KDateTable::KDateTablePrivate::beginningOfWeek()
{
    // setDate does validity checking for us
    q->setDate(m_date.addDays(1 - m_date.dayOfWeek()));
}

void KDateTable::KDateTablePrivate::endOfWeek()
{
    // setDate does validity checking for us
    q->setDate(m_date.addDays(7 - m_date.dayOfWeek()));
}

void KDateTable::keyPressEvent( QKeyEvent *e )
{
    QDate cd = d->m_date;

    switch( e->key() ) {
    case Qt::Key_Up:
        // setDate does validity checking for us
        setDate(d->m_date.addDays(-7));
        break;
    case Qt::Key_Down:
        // setDate does validity checking for us
        setDate(d->m_date.addDays(7));
        break;
    case Qt::Key_Left:
        // setDate does validity checking for us
        setDate(d->m_date.addDays(-1));
        break;
    case Qt::Key_Right:
        // setDate does validity checking for us
        setDate(d->m_date.addDays(1));
        break;
    case Qt::Key_Minus:
        // setDate does validity checking for us
        setDate(d->m_date.addDays(-1));
        break;
    case Qt::Key_Plus:
        // setDate does validity checking for us
        setDate(d->m_date.addDays(1));
        break;
    case Qt::Key_N:
        // setDate does validity checking for us
        setDate(QDate::currentDate());
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit tableClicked();
        break;
    case Qt::Key_Control:
    case Qt::Key_Alt:
    case Qt::Key_Meta:
    case Qt::Key_Shift:
        // Don't beep for modifiers
        break;
    default:
        if (!e->modifiers()) { // hm
            KNotification::beep();
        }
    }

    switch( e->key() ) {
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Minus:
    case Qt::Key_Plus: {
        if ( d->m_selectionmode == ExtendedSelection ) {
            if ( e->modifiers() & Qt::ShiftModifier ) {
                int inc = cd > d->m_date ? 1 : -1;
                for ( QDate dd = d->m_date;  dd != cd; dd = dd.addDays( inc ) ) {
                    if ( ! d->m_selectedDates.contains( dd ) ) {
                        d->m_selectedDates << dd;
                    }
                }
            } else if ( e->modifiers() & Qt::ControlModifier ) {
                // keep selection, just move on
            } else {
                d->m_selectedDates.clear();
            }
        }
        break;}
    case Qt::Key_Space:
    case Qt::Key_Select:
        if ( d->m_selectionmode == ExtendedSelection ) {
            if ( e->modifiers() & Qt::ControlModifier ) {
                if ( d->m_selectedDates.contains( d->m_date ) ) {
                    d->m_selectedDates.removeAt( d->m_selectedDates.indexOf( d->m_date ) );
                } else {
                    d->m_selectedDates << d->m_date;
                }
            } else if ( ! d->m_selectedDates.contains( d->m_date ) ) {
                d->m_selectedDates << d->m_date;
            }
            update();
        }
        break;
    case Qt::Key_Menu:
        if ( d->m_popupMenuEnabled )
        {
            QMenu *menu = new QMenu();
            if ( d->m_selectionmode == ExtendedSelection ) {
                emit aboutToShowContextMenu( menu, d->m_selectedDates );
            } else {
                menu->setTitle( QLocale().toString(d->m_date, QLocale::ShortFormat) );
                emit aboutToShowContextMenu( menu, d->m_date );
            }
            if ( menu->isEmpty() ) {
                delete menu;
            } else {
                int p = posFromDate( d->m_date ) - 1;
                int col = p % 7;
                int row = p / 7;
                QPoint pos = geometry().topLeft();
                QSize size = geometry().size();
                int sx = size.width() / 8;
                int sy = size.height() / 7;
                pos = QPoint( pos.x() + sx + sx / 2 + sx * col, pos.y() + sy + sy * row );
                debugPlan<<pos<<p<<col<<row;
                menu->popup(mapToGlobal(pos));
            }
        }
        break;
    }
}

void KDateTable::setFontSize(int size)
{
    QFontMetricsF metrics(fontMetrics());
    QRectF rect;
    // ----- store rectangles:
    d->fontsize = size;
    // ----- find largest day name:
    d->m_maxCell.setWidth(0);
    d->m_maxCell.setHeight(0);
    QLocale locale;
    for (int weekday = 1; weekday <= 7; ++weekday) {
        rect = metrics.boundingRect(locale.dayName(weekday, QLocale::ShortFormat));
        d->m_maxCell.setWidth(qMax(d->m_maxCell.width(), rect.width()));
        d->m_maxCell.setHeight(qMax(d->m_maxCell.height(), rect.height()));
    }
    // ----- compare with a real wide number and add some space:
    rect = metrics.boundingRect(QStringLiteral("88"));
    d->m_maxCell.setWidth(qMax(d->m_maxCell.width() + 2, rect.width()));
    d->m_maxCell.setHeight(qMax(d->m_maxCell.height() + 4, rect.height()));
}

void KDateTable::wheelEvent ( QWheelEvent * e )
{
    setDate(d->m_date.addMonths( -(int)(e->delta()/120)) );
    e->accept();
}

bool KDateTable::event( QEvent *ev )
{
    switch (ev->type()) {
    case QEvent::HoverMove: {
        QHoverEvent *e = static_cast<QHoverEvent *>(ev);
        const int row = e->pos().y() * 7 / height();
        int col;
        if (layoutDirection() == Qt::RightToLeft) {
            col = (d->m_paintweeknumbers ? 8 : 7) - (e->pos().x() * (d->m_paintweeknumbers ? 8 : 7) / width()) - 1;
        } else {
            col = e->pos().x() * (d->m_paintweeknumbers ? 8 : 7) / width();
        }

        const int pos = row < 1 ? -1 : ((d->m_paintweeknumbers ? 8 : 7) * (row - 1)) + col;

        if (pos != d->m_hoveredPos) {
            d->m_hoveredPos = pos;
            update();
        }
        break;
    }
    case QEvent::HoverLeave:
        if (d->m_hoveredPos != -1) {
            d->m_hoveredPos = -1;
            update();
        }
        break;
    case QEvent::ToolTip: {
        //debugPlan<<"Tooltip";
        QHelpEvent *e = static_cast<QHelpEvent*>( ev );

        double cellWidth = width() / ( d->m_paintweeknumbers ?  8.0 : 7.0 );
        double cellHeight = height() / 7.0;
        int column = (int)std::floor(e->pos().x() / cellWidth);
        int row = (int)std::floor(e->pos().y() / cellHeight);
        QString text;
        if ( row == 0 ) {
            if (column == 0 && d->m_paintweeknumbers ) {
                // corner
            } else {
                // we are drawing the headline (weekdays)
                int col = d->m_paintweeknumbers ? column - 1 : column;

                int day = col + QLocale().firstDayOfWeek();
                if (day >= 8 ) {
                    day -= 7;
                }
                if ( d->m_weekDayDelegate ) {
                    text = d->m_weekDayDelegate->data( day, Qt::ToolTipRole, d->m_model ).toString();
                }
            }
        } else {
            if ( d->m_paintweeknumbers && column == 0 ) {
                int pos = 7 * (row-1);
                QDate pCellDate = dateFromPos( pos );
                if ( d->m_weekNumberDelegate ) {
                    text = d->m_weekNumberDelegate->data( pCellDate.weekNumber(), Qt::ToolTipRole, d->m_model ).toString();
                }
            } else {
                // draw the dates
                int col = d->m_paintweeknumbers ? column - 1 : column;
                int pos=7*(row-1)+col;
                QDate pCellDate = dateFromPos( pos );
                if ( d->m_dateDelegate ) {
                    text = d->m_dateDelegate->data( pCellDate, Qt::ToolTipRole, d->m_model ).toString();
                }
            }
        }
        //debugPlan<<row<<column<<text;
        if ( text.isEmpty() ) {
            QToolTip::hideText();
        } else {
            QToolTip::showText( e->globalPos(), text );
        }
        e->accept();
        return true;
        break;
    }
    default:
        break;
    }
    return QWidget::event(ev);
}

void KDateTable::mousePressEvent(QMouseEvent *e)
{
    if(e->type()!=QEvent::MouseButtonPress) { // the KDatePicker only reacts on mouse press events:
        return;
    }
    if(!isEnabled()) {
        KNotification::beep();
        return;
    }

    int row, col, pos;

    QPoint mouseCoord = e->pos();
    row = mouseCoord.y() * 7 / height();
    if (layoutDirection() == Qt::RightToLeft) {
        col = ( d->m_paintweeknumbers ? 8 : 7 ) - (mouseCoord.x() * ( d->m_paintweeknumbers ? 8 : 7 ) / width()) - 1;
    } else {
        col = mouseCoord.x() * ( d->m_paintweeknumbers ? 8 : 7 ) / width();
    }

    //debugPlan<<d->m_maxCell<<", "<<size()<<row<<", "<<col<<", "<<mouseCoord;
    if ( d->m_paintweeknumbers ) {
        --col;
    }
    if (row < 1 || col < 0) {  // the user clicked on the frame of the table
        return;
    }

    // Rows and columns are zero indexed.  The (row - 1) below is to avoid counting
    // the row with the days of the week in the calculation.

    // new position and date
    pos = (7 * (row - 1)) + col;
    QDate clickedDate = dateFromPos( pos );

  if ( d->m_selectionmode != ExtendedSelection || e->button() !=  Qt::RightButton || ! d->m_selectedDates.contains( clickedDate ) )
  {
    switch ( d->m_selectionmode )
    {
        case SingleSelection:
            break;
        case ExtendedSelection:
            //debugPlan<<"extended "<<e->modifiers()<<", "<<clickedDate;
            if ( e->modifiers() & Qt::ShiftModifier )
            {
                if ( d->m_selectedDates.isEmpty() )
                {
                    d->m_selectedDates << clickedDate;
                }
                else if ( d->m_date != clickedDate )
                {
                    QDate dt = d->m_date;
                    int nxt = dt < clickedDate ? 1 : -1;
                    if ( d->m_selectedDates.contains( clickedDate ) )
                    {
                        d->m_selectedDates.removeAt( d->m_selectedDates.indexOf( clickedDate ) );
                    }
                    while ( dt != clickedDate )
                    {
                        if ( ! d->m_selectedDates.contains( dt ) )
                        {
                            d->m_selectedDates << dt;
                        }
                        dt = dt.addDays( nxt );
                    }
                    d->m_selectedDates << clickedDate;
                }
                else
                {
                    break; // selection not changed
                }
            }
            else if ( e->modifiers() & Qt::ControlModifier )
            {
                if ( d->m_selectedDates.contains( clickedDate ) )
                {
                    d->m_selectedDates.removeAt( d->m_selectedDates.indexOf( clickedDate ) );
                }
                else
                {
                    d->m_selectedDates << clickedDate;
                }
            }
            else
            {
                d->m_selectedDates.clear();
                d->m_selectedDates << clickedDate;
            }
            emit selectionChanged( d->m_selectedDates );
            break;
        default: break;
    }
    // set the new date. If it is in the previous or next month, the month will
    // automatically be changed, no need to do that manually...
    // validity checking done inside setDate
    setDate( clickedDate );

    // This could be optimized to only call update over the regions
    // of old and new cell, but 99% of times there is also a call to
    // setDate that already calls update() so no need to optimize that
    // much here
    update();
  }
  emit tableClicked();

  if (e->button() == Qt::RightButton && d->m_popupMenuEnabled ) {
        QMenu *menu = new QMenu();
        if ( d->m_selectionmode == ExtendedSelection ) {
            emit aboutToShowContextMenu( menu, d->m_selectedDates );
        } else {
            menu->setTitle( QLocale().toString(clickedDate, QLocale::ShortFormat) );
            emit aboutToShowContextMenu( menu, clickedDate );
        }
        menu->popup(e->globalPos());
  }
}

bool KDateTable::setDate(const QDate& date_)
{
    if (!date_.isValid()) {
        debugPlan << "KDateTable::setDate: refusing to set invalid date.";
        return false;
    }

    if (d->m_date != date_) {
        const QDate oldDate = d->m_date;
        d->m_date = date_;
        emit(dateChanged(oldDate, date_));
        emit(dateChanged(date_));
    }

    if ( d->m_selectionmode == KDateTable::SingleSelection )
    {
        d->m_selectedDates.clear();
        d->m_selectedDates << date_;
        emit selectionChanged( d->m_selectedDates );
    }

    update();

    return true;
}

const QDate &KDateTable::date() const
{
  return d->m_date;
}

void KDateTable::focusInEvent( QFocusEvent *e )
{
    QWidget::focusInEvent( e );
}

void KDateTable::focusOutEvent( QFocusEvent *e )
{
    QWidget::focusOutEvent( e );
}

QSize KDateTable::sizeHint() const
{
    if(d->m_maxCell.height() > 0 && d->m_maxCell.width() > 0) {
        int s = d->m_paintweeknumbers ? 8 : 7;
        return QSize(qRound(d->m_maxCell.width() * s),
                     (qRound(d->m_maxCell.height() + 2) * s));
    } else {
        debugPlan << "KDateTable::sizeHint: obscure failure - ";
        return QSize(-1, -1);
    }
}

void KDateTable::setPopupMenuEnabled( bool enable )
{
   d->m_popupMenuEnabled=enable;
}

bool KDateTable::popupMenuEnabled() const
{
   return d->m_popupMenuEnabled;
}

void KDateTable::setCustomDatePainting(const QDate &date, const QColor &fgColor, BackgroundMode bgMode, const QColor &bgColor)
{
    KDateTableCustomDateDelegate *del = new KDateTableCustomDateDelegate();
    del->fgColor = fgColor;
    del->bgMode = bgMode;
    del->bgColor = bgColor;
    setDateDelegate( date, del );
    update();
}

void KDateTable::unsetCustomDatePainting(const QDate &date)
{
    d->customPaintingModes.remove(date.toJulianDay());
}

void KDateTable::setSelectionMode( SelectionMode mode )
{
    d->m_selectionmode = mode;
}

//-----------------------
KDateTableDataModel::KDateTableDataModel( QObject *parent )
    : QObject( parent )
{
}

KDateTableDataModel::~KDateTableDataModel()
{
}

QVariant KDateTableDataModel::data( const QDate &date, int role, int dataType ) const
{
    Q_UNUSED(date);
    Q_UNUSED(role);
    Q_UNUSED(dataType);
    return QVariant();
}

QVariant KDateTableDataModel::weekDayData( int day, int role ) const
{
    Q_UNUSED(day);
    Q_UNUSED(role);
    return QVariant();
}

QVariant KDateTableDataModel::weekNumberData( int week, int role ) const
{
    Q_UNUSED(week);
    Q_UNUSED(role);
    return QVariant();
}

//-------------
KDateTableDateDelegate::KDateTableDateDelegate( QObject *parent )
    : QObject( parent )
{
}

QVariant KDateTableDateDelegate::data( const QDate &date, int role, KDateTableDataModel *model )
{
    //debugPlan<<date<<role<<model;
    if ( model == 0 ) {
        return QVariant();
    }
    return model->data( date, role );
}

QRectF KDateTableDateDelegate::paint( QPainter *painter, const StyleOptionViewItem &option, const QDate &date, KDateTableDataModel *model )
{
    //debugPlan<<date;
    painter->save();
    QRectF r;

    QPalette palette = option.palette;
    if ( option.state & QStyle::State_Enabled && option.state & QStyle::State_Active ) {
        palette.setCurrentColorGroup( QPalette::Active );
    } else {
        palette.setCurrentColorGroup( QPalette::Inactive );
    }

    // TODO: honor QStyle::State_MouseOver, and perhaps switch to style()->drawPrimitive(QStyle::PE_PanelItemViewItem, ...
    QFont font = option.font;
    QColor textColor = palette.text().color();
    QBrush bg( palette.base() );
    Qt::Alignment align = option.displayAlignment;
    QString text = QLocale().toString(date.day());

    if ( model )
    {
        QVariant v = model->data( date, Qt::ForegroundRole );
        if ( v.isValid() )
        {
          textColor = v.value<QColor>();
        }
        v = model->data( date, Qt::BackgroundRole );
        if ( v.isValid() )
        {
          bg.setColor( v.value<QColor>() );
        }
        v = model->data( date );
        if ( v.isValid() )
        {
          text = v.toString();
        }
        v = model->data( date, Qt::TextAlignmentRole );
        if ( v.isValid() )
        {
          align = (Qt::Alignment)v.toInt();
        }
        v = model->data( date, Qt::FontRole );
        if ( v.isValid() )
        {
          font = v.value<QFont>();
        }
    }

    QPen pen = painter->pen();
    pen.setColor( textColor );

    if ( option.state & QStyle::State_Selected ) {
        bg = palette.highlight();
    }
    painter->fillRect( option.rectF, bg );
    painter->setBrush( bg );


    if ( option.state & QStyle::State_HasFocus ) {
        painter->setPen( palette.text().color() );
        painter->setPen( Qt::DotLine );
        painter->drawRect( option.rectF );
    } else if ( date == QDate::currentDate() ) {
        painter->setPen( palette.text().color() );
        painter->drawRect( option.rectF );
    }

    if ( option.state & QStyle::State_Selected ) {
        pen.setColor( palette.highlightedText().color() );
    }
    painter->setFont( font );
    painter->setPen( pen );
    painter->drawText( option.rectF, align, text, &r );

    painter->restore();
    return r;
}

//---------

KDateTableCustomDateDelegate::KDateTableCustomDateDelegate( QObject *parent )
    : KDateTableDateDelegate( parent )
{
}

QRectF KDateTableCustomDateDelegate::paint( QPainter *painter, const StyleOptionViewItem &option, const QDate &date, KDateTableDataModel *model )
{
    //debugPlan<<date;
    painter->save();

    QRectF r;
    bool paintRect=true;
    QBrush bg(option.palette.base());

    if( (option.state & QStyle::State_Active) == 0 )
    { // we are either
      // ° painting a day of the previous month or
      // ° painting a day of the following month
      // TODO: don't hardcode gray here! Use a color with less contrast to the background than normal text.
      painter->setPen( option.palette.color(QPalette::Mid) );
//          painter->setPen(gray);
    }
    else
    { // paint a day of the current month
      if (bgMode != KDateTable::NoBgMode)
      {
        QBrush oldbrush=painter->brush();
        painter->setBrush( bgColor );
        switch(bgMode)
        {
            case(KDateTable::CircleMode) : painter->drawEllipse(option.rectF);break;
            case(KDateTable::RectangleMode) : painter->drawRect(option.rectF);break;
            case(KDateTable::NoBgMode) : // Should never be here, but just to get one
                        // less warning when compiling
            default: break;
        }
        painter->setBrush( oldbrush );
        paintRect=false;
        }
      painter->setPen( fgColor );

      QPen pen=painter->pen();
      if ( option.state & QStyle::State_Selected )
      {
        // draw the currently selected date
        //debugPlan<<"selected: "<<date;
        if ( option.state & QStyle::State_Enabled )
        {
          //debugPlan<<"enabled & selected: "<<date;
          painter->setPen(option.palette.color(QPalette::Highlight));
          painter->setBrush(option.palette.color(QPalette::Highlight));
        }
        else
        {
          //debugPlan<<"disabled & selected: "<<date;
          painter->setPen(option.palette.color(QPalette::Text));
          painter->setBrush(option.palette.color(QPalette::Text));
        }
        pen=option.palette.color(QPalette::HighlightedText);
      }
      else
      {
        painter->setBrush(option.palette.color(QPalette::Background));
        painter->setPen(option.palette.color(QPalette::Background));
      }

      if ( date == QDate::currentDate() )
      {
          painter->setPen(option.palette.color(QPalette::Text));
      }

      if ( paintRect )
      {
        painter->drawRect(option.rectF);
      }
      painter->setPen(pen);
      QString text = QLocale().toString(date.day());
      if ( model )
      {
        QVariant v = model->data( date );
        if ( v.isValid() )
        {
            text = v.toString();
        }
      }
      painter->drawText(option.rectF, Qt::AlignCenter, text, &r);
    }
    painter->restore();
    return r;
}

//---------
KDateTableWeekDayDelegate::KDateTableWeekDayDelegate( QObject *parent )
    : QObject( parent )
{
}

QVariant KDateTableWeekDayDelegate::data( int day, int role, KDateTableDataModel *model )
{
    //debugPlan<<day<<role<<model;
    if ( model == 0 ) {
        return QVariant();
    }
    return model->weekDayData( day, role );
}

QRectF KDateTableWeekDayDelegate::paint( QPainter *painter, const StyleOptionHeader &option, int daynum, KDateTableDataModel *model )
{
    //debugPlan<<daynum;
    painter->save();

    QPalette palette = option.palette;
    if ( option.state & QStyle::State_Active ) {
        palette.setCurrentColorGroup( QPalette::Active );
    } else {
        palette.setCurrentColorGroup( QPalette::Inactive );
    }
    QRectF rect;
    QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
//    font.setBold(true);
    painter->setFont(font);

    QColor titleColor( palette.button().color() );
    QColor textColor( palette.buttonText().color() );

    painter->setPen(titleColor);
    painter->setBrush(titleColor);
    painter->drawRect(option.rectF);

    QString value;
    if ( model ) {
        QVariant v = model->weekDayData( daynum, Qt::DisplayRole );
        if ( v.isValid() ) {
            value = v.toString();
        }
    }
    if (value.isEmpty()) {
         value = QLocale().dayName(daynum, QLocale::ShortFormat);
    }
    //debugPlan<<daynum<<": "<<value;
    painter->setPen( textColor );
    painter->drawText(option.rectF, option.textAlignment, value, &rect);

//     painter->setPen( palette.color(QPalette::Text) );
//     painter->drawLine(QPointF(0, option.rectF.height()), QPointF(option.rectF.width(), option.rectF.height()));

    painter->restore();
    return rect;
}

//---------
KDateTableWeekNumberDelegate::KDateTableWeekNumberDelegate( QObject *parent )
    : QObject( parent )
{
}

QVariant KDateTableWeekNumberDelegate::data( int week, int role, KDateTableDataModel *model )
{
    //debugPlan<<week<<role<<model;
    if ( model == 0 ) {
        return QVariant();
    }
    return model->weekNumberData( week, role );
}

QRectF KDateTableWeekNumberDelegate::paint( QPainter *painter, const StyleOptionHeader &option, int week, KDateTableDataModel *model )
{
    //debugPlan;
    painter->save();
    QRectF result;
    QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    painter->setFont(font);

    QColor titleColor( option.palette.button().color() );
    QColor textColor( option.palette.buttonText().color() );

    painter->setPen(titleColor);
    painter->setBrush(titleColor);
    painter->drawRect(option.rectF);
    painter->setPen(textColor);

    QString value = QString("%1").arg( week );
    if ( model ) {
        QVariant v = model->weekNumberData( week, Qt::DisplayRole );
        if ( v.isValid() ) {
            value = v.toString();
        }
    }
    painter->drawText(option.rectF, option.textAlignment, value, &result);

//     painter->setPen(option.palette.color(QPalette::Text));
//     painter->drawLine(QPointF(option.rectF.width(), 0), QPointF(option.rectF.width(), option.rectF.height()));

    painter->restore();
    return result;
}

} //namespace KPlato

#include "moc_kdatetable.cpp"
