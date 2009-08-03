/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
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

/////////////////// KDateTable widget class //////////////////////
//
// Copyright (C) 1997 Tim D. Gilman
//           (C) 1998-2001 Mirko Boehm
// Written using Qt (http://www.troll.no) for the
// KDE project (http://www.kde.org)
//
// This is a support class for the KDatePicker class.  It just
// draws the calendar table without titles, but could theoretically
// be used as a standalone.
//
// When a date is selected by the user, it emits a signal:
//      dateSelected(QDate)

#include "kdatetable.h"
#include "kdatetable_p.h"

#include <kconfig.h>
#include <kcolorscheme.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>
#include <knotification.h>
#include <kcalendarsystem.h>
#include <kshortcut.h>
#include <kstandardshortcut.h>
#include "kdatepicker.h"
#include "kmenu.h"
#include "kactioncollection.h"
#include "kaction.h"

#include <QtCore/QDate>
#include <QtCore/QCharRef>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtGui/QDialog>
#include <QtGui/QActionEvent>
#include <QtCore/QHash>
#include <QtGui/QApplication>
#include <QToolTip>

#include <assert.h>

#include <cmath>

namespace KPlato
{
    
Frame::Frame( QWidget *parent )
    : QFrame( parent )
{
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    setLineWidth(1);
}

void Frame::updateFocus(QFocusEvent *e)
{
    if ( e->type() == QEvent::FocusIn ) {
        setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    } else {
        setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    }
    update();
}

void Frame::paintEvent(QPaintEvent *e)
{
    //kDebug()<<e;
    QPainter paint(this);
    drawFrame(&paint);
}

void Frame::drawFrame(QPainter *p)
{
    QPoint p1, p2;
    QStyleOptionFrame opt;
    opt.init(this);
    if ( hasFocus() ) {
        opt.state |= QStyle::State_HasFocus;
    }
    int frameShape  = frameStyle() & QFrame::Shape_Mask;
    int frameShadow = frameStyle() & QFrame::Shadow_Mask;

    int lw = 0;
    int mlw = 0;
    opt.rect = frameRect();
    switch (frameShape) {
        case QFrame::Box:
        case QFrame::HLine:
        case QFrame::VLine:
        case QFrame::StyledPanel:
            lw = lineWidth();
            mlw = midLineWidth();
            break;
        default:
            // most frame styles do not handle customized line and midline widths
            // (see updateFrameWidth()).
            lw = frameWidth();
            break;
    }
    opt.lineWidth = lw;
    opt.midLineWidth = mlw;
    if (frameShadow == Sunken)
        opt.state |= QStyle::State_Sunken;
    else if (frameShadow == Raised)
        opt.state |= QStyle::State_Raised;

    switch (frameShape) {
        case Box:
            if (frameShadow == Plain)
                qDrawPlainRect(p, opt.rect, opt.palette.foreground().color(), lw);
            else
                qDrawShadeRect(p, opt.rect, opt.palette, frameShadow == Sunken, lw, mlw);
            break;


        case StyledPanel:
            style()->drawPrimitive(QStyle::PE_Frame, &opt, p, this);
            break;

        case Panel:
            if (frameShadow == Plain)
                qDrawPlainRect(p, opt.rect, opt.palette.foreground().color(), lw);
            else
                qDrawShadePanel(p, opt.rect, opt.palette, frameShadow == Sunken, lw);
            break;

        case WinPanel:
            if (frameShadow == Plain)
                qDrawPlainRect(p, opt.rect, opt.palette.foreground().color(), lw);
            else
                qDrawWinPanel(p, opt.rect, opt.palette, frameShadow == Sunken);
            break;
        case HLine:
        case VLine:
            if (frameShape == HLine) {
                p1 = QPoint(opt.rect.x(), opt.rect.height() / 2);
                p2 = QPoint(opt.rect.x() + opt.rect.width(), p1.y());
            } else {
                p1 = QPoint(opt.rect.x()+opt.rect.width() / 2, 0);
                p2 = QPoint(p1.x(), opt.rect.height());
            }
            if (frameShadow == Plain) {
                QPen oldPen = p->pen();
                p->setPen(QPen(opt.palette.foreground().color(), lw));
                p->drawLine(p1, p2);
                p->setPen(oldPen);
            } else {
                qDrawShadeLine(p, p1, p2, opt.palette, frameShadow == Sunken, lw, mlw);
            }
            break;
    }
}


class KDateTable::KDateTablePrivate
{
public:
   KDateTablePrivate(KDateTable *q): q(q)
   {
      popupMenuEnabled=false;
      m_selectionmode = KDateTable::SingleSelection;
      m_paintweeknumbers = false;
      m_model = 0;
      m_grid = false;
   }

   ~KDateTablePrivate()
   {
       QList<QString> lst = customPaintingModes.keys();
       foreach( QString k, lst ) {
           delete customPaintingModes.take( k );
       }
   }

   void nextMonth();
   void previousMonth();
   void beginningOfMonth();
   void endOfMonth();
   void beginningOfWeek();
   void endOfWeek();
  
   KDateTable *q;

   /**
   * The font size of the displayed text.
   */
   int fontsize;
   /**
   * The currently selected date.
   */
   QDate mDate;
   /**
    * The day of the first day in the month [1..7].
    */
   int firstday;
   /**
    * The number of days in the current month.
    */
   int numdays;
   /**
    * The number of days in the previous month.
    */
   int numDaysPrevMonth;
   /**
    * Save the size of the largest used cell content.
    */
   QRectF maxCell;
  
   bool popupMenuEnabled;
   
    //----->
    QHash <QString, KDateTableDateDelegate*> customPaintingModes;

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

class KPopupFrame::KPopupFramePrivate
{
public:
  KPopupFramePrivate(KPopupFrame *q):
    q(q),
    result(0), // rejected
    main(0) {}
  
  KPopupFrame *q;
  
  /**
   * The result. It is returned from exec() when the popup window closes.
   */
  int result;
  /**
   * The only subwidget that uses the whole dialog window.
   */
  QWidget *main;
};


KDateValidator::KDateValidator(QWidget* parent)
    : QValidator(parent)
{
}

QValidator::State
KDateValidator::validate(QString& text, int&) const
{
  QDate temp;
  // ----- everything is tested in date():
  return date(text, temp);
}

QValidator::State
KDateValidator::date(const QString& text, QDate& d) const
{
  QDate tmp = KGlobal::locale()->readDate(text);
  if (!tmp.isNull())
    {
      d = tmp;
      return Acceptable;
    } else
      return QValidator::Intermediate;
}

void
KDateValidator::fixup( QString& ) const
{

}

KDateTable::KDateTable(const QDate& date_, QWidget* parent)
  : QWidget(parent), d(new KDateTablePrivate(this))
{
  setFontSize(10);
  setFocusPolicy(Qt::StrongFocus);
  QPalette palette;
  palette.setColor(backgroundRole(), KColorScheme(QPalette::Active, KColorScheme::View).background().color() );

  setPalette(palette);

  if(!date_.isValid())
  {
    kDebug() << "KDateTable ctor: WARNING: Given date is invalid, using current date.";
    setDate(QDate::currentDate()); // this initializes firstday, numdays, numDaysPrevMonth
  }
  else
    setDate(date_); // this initializes firstday, numdays, numDaysPrevMonth
  initAccels();
  
  init();
}

KDateTable::KDateTable(QWidget *parent)
  : QWidget(parent), d(new KDateTablePrivate(this))
{
  setFontSize(10);
  setFocusPolicy(Qt::StrongFocus);
  QPalette palette;
  palette.setColor(backgroundRole(),  KColorScheme(QPalette::Active, KColorScheme::View).background().color() );
  setPalette(palette);
  setDate(QDate::currentDate()); // this initializes firstday, numdays, numDaysPrevMonth
  initAccels();
  
  init();
}

KDateTable::~KDateTable()
{
  delete d;
}

void KDateTable::init()
{
    d->m_dateDelegate = new KDateTableDateDelegate();
    d->m_weekDayDelegate = new KDateTableWeekDayDelegate();
    d->m_weekNumberDelegate = new KDateTableWeekNumberDelegate();
    
    d->m_styleOptionDate.initFrom( this );
    d->m_styleOptionDate.displayAlignment = Qt::AlignCenter;
    
    d->m_styleOptionWeekDay.initFrom( this );
    d->m_styleOptionWeekDay.textAlignment = Qt::AlignCenter;
    
    d->m_styleOptionWeekNumber.initFrom( this );
    d->m_styleOptionWeekNumber.textAlignment = Qt::AlignCenter;
    
    //setModel( new KDateTableDataModel( this ) );
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
    delete d->customPaintingModes.take( date.toString() );
    d->customPaintingModes[ date.toString() ] = delegate;
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

int KDateTable::posFromDate( const QDate &dt )
{
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();
  const int firstWeekDay = KGlobal::locale()->weekStartDay();
  int pos = calendar->day( dt );
  int offset = (d->firstday - firstWeekDay + 7) % 7;
  // make sure at least one day of the previous month is visible.
  // adjust this <1 if more days should be forced visible:
  if ( offset < 1 ) offset += 7;
  return pos + offset;
}

QDate KDateTable::dateFromPos( int pos )
{
  QDate pCellDate;
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();
  calendar->setYMD(pCellDate, calendar->year(d->mDate), calendar->month(d->mDate), 1);

  int firstWeekDay = KGlobal::locale()->weekStartDay();
  int offset = (d->firstday - firstWeekDay + 7) % 7;
  // make sure at least one day of the previous month is visible.
  // adjust this <1 if more days should be forced visible:
  if ( offset < 1 ) offset += 7;
  pCellDate = calendar->addDays( pCellDate, pos - offset );
  return pCellDate;
}

bool KDateTable::event( QEvent *event )
{
    if ( event->type() == QEvent::ToolTip ) {
        //kDebug()<<"Tooltip";
        QHelpEvent *e = static_cast<QHelpEvent*>( event );

        double cellWidth = width() / ( d->m_paintweeknumbers ?  8.0 : 7.0 );
        double cellHeight = height() / 7.0;
        int column = (int)floor(e->pos().x() / cellWidth);
        int row = (int)floor(e->pos().y() / cellHeight);
        QString text;
        if ( row == 0 && column == 0 && d->m_paintweeknumbers ) {
            // corner
        } else if ( row == 0 ) { // we are drawing the headline (weekdays)
            int col = d->m_paintweeknumbers ? column - 1 : column;
            int firstWeekDay = KGlobal::locale()->weekStartDay();
            int day = ( col+firstWeekDay < 8 ) ? col+firstWeekDay : col+firstWeekDay-7;
            if ( d->m_weekDayDelegate )
            {
                text = d->m_weekDayDelegate->data( day, Qt::ToolTipRole, d->m_model ).toString();
            }
        }
        else if ( d->m_paintweeknumbers && column == 0 )
        {
            int pos=7*(row-1);
            QDate pCellDate = dateFromPos( pos );
            if ( d->m_weekNumberDelegate )
            {
                const KCalendarSystem * calendar = KGlobal::locale()->calendar();
                text = d->m_weekNumberDelegate->data( calendar->weekNumber( pCellDate ), Qt::ToolTipRole, d->m_model ).toString();
            }
        }
        else
        {
            // draw the dates
            int col = d->m_paintweeknumbers ? column - 1 : column;
            int pos=7*(row-1)+col;
            QDate pCellDate = dateFromPos( pos );
            if ( d->m_dateDelegate ) {
                text = d->m_dateDelegate->data( pCellDate, Qt::ToolTipRole, d->m_model ).toString();
            }
        }
        //kDebug()<<row<<column<<text;
        if ( text.isEmpty() ) {
            QToolTip::hideText();
        } else {
            QToolTip::showText( e->globalPos(), text );
        }
        e->accept();
        return true;
    }
    return QWidget::event( event );
}

void KDateTable::paintEvent(QPaintEvent *e)
{
  QPainter p(this);
  const QRect &rectToUpdate = e->rect();
  double cellWidth = width() / ( d->m_paintweeknumbers ?  8.0 : 7.0 );
  double cellHeight = height() / 7.0;
  int leftCol = (int)floor(rectToUpdate.left() / cellWidth);
  int topRow = (int)floor(rectToUpdate.top() / cellHeight);
  int rightCol = (int)ceil(rectToUpdate.right() / cellWidth);
  int bottomRow = (int)ceil(rectToUpdate.bottom() / cellHeight);
  bottomRow = qMin(bottomRow, 6);
  rightCol = qMin(rightCol, ( d->m_paintweeknumbers ?  7 : 6 ) );
  p.translate(leftCol * cellWidth, topRow * cellHeight);
  for (int i = leftCol; i <= rightCol; ++i) {
    for (int j = topRow; j <= bottomRow; ++j) {
      paintCell(&p, j, i);
      p.translate(0, cellHeight);
    }
    
    p.translate(cellWidth, 0);
    p.translate(0, -cellHeight * (bottomRow - topRow + 1));
  }
}

void
KDateTable::paintCell(QPainter *painter, int row, int column)
{
  //kDebug();
  
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();

  QSizeF size;
  QString text;
  QPen pen;
  double w = width() / ( d->m_paintweeknumbers ? 8.0 : 7.0 );
  double h = height() / 7.0;
  w -= 1;
  h -= 1;
  QRectF rect( 0, 0, w, h );
  if ( row == 0 && column == 0 && d->m_paintweeknumbers )
  {
    // paint something in the corner??
/*    painter->setPen(palette().color(QPalette::Text));
    painter->drawRect( rect );*/
    return;
  }
  if ( row == 0 )
  { // we are drawing the headline (weekdays)
    d->m_styleOptionWeekDay.rectF = rect;
    d->m_styleOptionWeekDay.state = QStyle::State_None;
    
    int col = d->m_paintweeknumbers ? column - 1 : column;
    int firstWeekDay = KGlobal::locale()->weekStartDay();
    int day = ( col+firstWeekDay < 8 ) ? col+firstWeekDay : col+firstWeekDay-7;
    if ( d->m_weekDayDelegate )
    {
        size = d->m_weekDayDelegate->paint( painter, d->m_styleOptionWeekDay, day, d->m_model ).size();
    }
  }
  else if ( d->m_paintweeknumbers && column == 0 )
  {
    d->m_styleOptionWeekNumber.rectF = rect;
    d->m_styleOptionWeekNumber.state = QStyle::State_None;
    
    int pos=7*(row-1);
    QDate pCellDate = dateFromPos( pos );
    if ( d->m_weekNumberDelegate )
    {
      size = d->m_weekNumberDelegate->paint( painter, d->m_styleOptionWeekNumber, calendar->weekNumber( pCellDate ), d->m_model ).size();
    }
  }
  else
  {
    // draw the dates
    int col = d->m_paintweeknumbers ? column - 1 : column;
    int pos=7*(row-1)+col;

    if ( d->m_grid )
    {
        painter->save();
        QPen pen( "lightgrey" );
        pen.setWidthF( 0.5 );
        painter->setPen( pen );
        double pw = painter->pen().width();
        if ( col > 0 )
        {
          painter->drawLine( rect.topLeft(), rect.bottomLeft() );
        }
        if ( row > 1 )
        {
          painter->drawLine( rect.topLeft(), rect.topRight() );
        }
        rect = rect.adjusted(pw, pw, 0, 0 );
        painter->restore();
        //kDebug()<<d->m_grid<<" "<<pw<<" "<<rect;
    }
    
    d->m_styleOptionDate.rectF = rect;
    d->m_styleOptionDate.state = QStyle::State_None;

    
    QDate pCellDate = dateFromPos( pos );
    if( calendar->month(pCellDate) == calendar->month(d->mDate) )
    {
        d->m_styleOptionDate.state |= QStyle::State_Active;
    }
    if ( d->m_selectedDates.contains( pCellDate ) )
    {
        d->m_styleOptionDate.state |= QStyle::State_Selected;
    }
    if ( isEnabled() )
    {
        d->m_styleOptionDate.state |= QStyle::State_Enabled;
    }
    if ( pCellDate == d->mDate )
    {
        d->m_styleOptionDate.state |= QStyle::State_Active;
        if ( d->m_selectionmode != SingleSelection && hasFocus() )
        {
            d->m_styleOptionDate.state |= QStyle::State_HasFocus;
        }
    }
    KDateTableDateDelegate *del = d->customPaintingModes.value( pCellDate.toString() );
    if ( del == 0 ) {
      del = d->m_dateDelegate;
    }
    if ( del ) {
        //kDebug()<<del;
        size = del->paint( painter, d->m_styleOptionDate, pCellDate, d->m_model ).size();
    } else kWarning()<<"No delegate!";
  }
  if(size.width() > d->maxCell.width()) d->maxCell.setWidth(size.width());
  if(size.height() > d->maxCell.height()) d->maxCell.setHeight(size.height());
}

void KDateTable::KDateTablePrivate::nextMonth()
{
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();
  q->setDate(calendar->addMonths( mDate, 1 ));
}

void KDateTable::KDateTablePrivate::previousMonth()
{
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();
  q->setDate(calendar->addMonths( mDate, -1 ));
}

void KDateTable::KDateTablePrivate::beginningOfMonth()
{
  q->setDate(mDate.addDays(1 - mDate.day()));
}

void KDateTable::KDateTablePrivate::endOfMonth()
{
  q->setDate(mDate.addDays(mDate.daysInMonth() - mDate.day()));
}

void KDateTable::KDateTablePrivate::beginningOfWeek()
{
  q->setDate(mDate.addDays(1 - mDate.dayOfWeek()));
}

void KDateTable::KDateTablePrivate::endOfWeek()
{
  q->setDate(mDate.addDays(7 - mDate.dayOfWeek()));
}

void
KDateTable::keyPressEvent( QKeyEvent *e )
{
    QDate cd = d->mDate;
    switch( e->key() ) {
    case Qt::Key_Up:
        setDate(d->mDate.addDays(-7));
        break;
    case Qt::Key_Down:
        setDate(d->mDate.addDays(7));
        break;
    case Qt::Key_Left:
        setDate(d->mDate.addDays(-1));
        break;
    case Qt::Key_Right:
        setDate(d->mDate.addDays(1));
        break;
    case Qt::Key_Minus:
        setDate(d->mDate.addDays(-1));
        break;
    case Qt::Key_Plus:
        setDate(d->mDate.addDays(1));
        break;
    case Qt::Key_N:
        setDate(QDate::currentDate());
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit tableClicked();
        break;
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
                int inc = cd > d->mDate ? 1 : -1;
                for ( QDate dd = d->mDate;  dd != cd; dd = dd.addDays( inc ) ) {
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
                if ( d->m_selectedDates.contains( d->mDate ) ) {
                    d->m_selectedDates.removeAt( d->m_selectedDates.indexOf( d->mDate ) );
                } else {
                    d->m_selectedDates << d->mDate;
                }
            } else if ( ! d->m_selectedDates.contains( d->mDate ) ) {
                d->m_selectedDates << d->mDate;
            }
            update();
        }
        break;
    case Qt::Key_Menu:
        if (  d->popupMenuEnabled )
        {
            KMenu *menu = new KMenu();
            if ( d->m_selectionmode == ExtendedSelection ) {
                emit aboutToShowContextMenu( menu, d->m_selectedDates );
            } else {
                menu->addTitle( KGlobal::locale()->formatDate(d->mDate) );
                emit aboutToShowContextMenu( menu, d->mDate );
            }
            if ( menu->isEmpty() ) {
                delete menu;
            } else {
                int p = posFromDate( d->mDate ) - 1;
                int col = p % 7;
                int row = p / 7;
                QPoint pos = geometry().topLeft();
                QSize size = geometry().size();
                int sx = size.width() / 8;
                int sy = size.height() / 7;
                pos = QPoint( pos.x() + sx + sx / 2 + sx * col, pos.y() + sy + sy * row );
                kDebug()<<pos<<p<<col<<row;
                menu->popup(mapToGlobal(pos));
            }
        }
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
}

void
KDateTable::setFontSize(int size)
{
  int count;
  QFontMetricsF metrics(fontMetrics());
  QRectF rect;
  // ----- store rectangles:
  d->fontsize=size;
  // ----- find largest day name:
  d->maxCell.setWidth(0);
  d->maxCell.setHeight(0);
  for(count=0; count<7; ++count)
    {
      rect=metrics.boundingRect(KGlobal::locale()->calendar()
                                ->weekDayName(count+1, KCalendarSystem::ShortDayName));
      d->maxCell.setWidth(qMax(d->maxCell.width(), rect.width()));
      d->maxCell.setHeight(qMax(d->maxCell.height(), rect.height()));
    }
  // ----- compare with a real wide number and add some space:
  rect=metrics.boundingRect(QLatin1String("88"));
  d->maxCell.setWidth(qMax(d->maxCell.width()+2, rect.width()));
  d->maxCell.setHeight(qMax(d->maxCell.height()+4, rect.height()));
}

void
KDateTable::wheelEvent ( QWheelEvent * e )
{
    setDate(d->mDate.addMonths( -(int)(e->delta()/120)) );
    e->accept();
}

void
KDateTable::mousePressEvent(QMouseEvent *e)
{
  if(e->type()!=QEvent::MouseButtonPress)
    { // the KDatePicker only reacts on mouse press events:
      return;
    }
  if(!isEnabled())
    {
      KNotification::beep();
      return;
    }

  // -----
  int row, col, pos, temp;
  QPoint mouseCoord;
  // -----
  mouseCoord = e->pos();
  row=mouseCoord.y() / ( height() / 7 );
  col=mouseCoord.x() / ( width() / ( d->m_paintweeknumbers ? 8 : 7 ) );
  //kDebug()<<d->maxCell<<", "<<size()<<row<<", "<<col<<", "<<mouseCoord;
  if(row<1 )
  { // the user clicked on the frame of the table
      //kDebug()<<"weekday "<<col;
      return;
  }
  if ( col < ( d->m_paintweeknumbers ? 1 : 0 ) )
  {
      //kDebug()<<"weeknumber "<<row;
      return;
  }
  if ( d->m_paintweeknumbers ) {
      --col;
  }

  // Rows and columns are zero indexed.  The (row - 1) below is to avoid counting
  // the row with the days of the week in the calculation.

  // old selected date:
  temp = posFromDate( d->mDate );
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
            //kDebug()<<"extended "<<e->modifiers()<<", "<<clickedDate;
            if ( e->modifiers() & Qt::ShiftModifier )
            {
                if ( d->m_selectedDates.isEmpty() )
                {
                    d->m_selectedDates << clickedDate;
                }
                else if ( d->mDate != clickedDate )
                {
                    QDate dt = d->mDate;
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
    setDate( clickedDate );

    // This could be optimized to only call update over the regions
    // of old and new cell, but 99% of times there is also a call to
    // setDate that already calls update() so no need to optimize that
    // much here
    update();
  }
  emit tableClicked();

  if (  e->button() == Qt::RightButton && d->popupMenuEnabled )
  {
        KMenu *menu = new KMenu();
        if ( d->m_selectionmode == ExtendedSelection ) {
            emit aboutToShowContextMenu( menu, d->m_selectedDates );
        } else {
            menu->addTitle( KGlobal::locale()->formatDate(clickedDate) );
            emit aboutToShowContextMenu( menu, clickedDate );
        }
        menu->popup(e->globalPos());
  }
}

bool
KDateTable::setDate(const QDate& date_)
{
  bool changed=false;
  QDate temp;
  // -----
  if(!date_.isValid())
    {
      kDebug() << "KDateTable::setDate: refusing to set invalid date.";
      return false;
    }
  if(d->mDate!=date_)
    {
      emit(dateChanged(d->mDate, date_));
      d->mDate=date_;
      emit(dateChanged(d->mDate));
      changed=true;
    }
  if ( d->m_selectionmode == KDateTable::SingleSelection )
  {
      d->m_selectedDates.clear();
      d->m_selectedDates << date_;
      emit selectionChanged( d->m_selectedDates );
  }
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();

  calendar->setYMD(temp, calendar->year(d->mDate), calendar->month(d->mDate), 1);
  //temp.setYMD(d->mDate.year(), d->mDate.month(), 1);
  //kDebug() << "firstDayInWeek: " << temp.toString();
  d->firstday=temp.dayOfWeek();
  d->numdays=calendar->daysInMonth(d->mDate);

  temp = calendar->addMonths(temp, -1);
  d->numDaysPrevMonth=calendar->daysInMonth(temp);
  if(changed)
    {
      update();
    }
  return true;
}

const QDate&
KDateTable::date() const
{
  return d->mDate;
}

// what are those repaintContents() good for? (pfeiffer)
void KDateTable::focusInEvent( QFocusEvent *e )
{
//    repaintContents(false);
    QWidget::focusInEvent( e );
    emit focusChanged( e );
}

void KDateTable::focusOutEvent( QFocusEvent *e )
{
//    repaintContents(false);
    QWidget::focusOutEvent( e );
    emit focusChanged( e );
}

QSize
KDateTable::sizeHint() const
{
  if(d->maxCell.height()>0 && d->maxCell.width()>0)
    {
      int s = d->m_paintweeknumbers ? 8 : 7;
      return QSize(qRound(d->maxCell.width()*s),
             (qRound(d->maxCell.height()+2)*7));
    } else {
      kDebug() << "KDateTable::sizeHint: obscure failure - ";
      return QSize(-1, -1);
    }
}

void KDateTable::setPopupMenuEnabled( bool enable )
{
   d->popupMenuEnabled=enable;
}

bool KDateTable::popupMenuEnabled() const
{
   return d->popupMenuEnabled;
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

void KDateTable::unsetCustomDatePainting( const QDate &date )
{
    d->customPaintingModes.remove( date.toString() );
}

void KDateTable::setSelectionMode( SelectionMode mode )
{
    d->m_selectionmode = mode;
}

KDateInternalYearSelector::KDateInternalYearSelector
(QWidget* parent)
  : QLineEdit(parent),
    val(new QIntValidator(this)),
    result(0)
{
  QFont font;
  // -----
  font=KGlobalSettings::generalFont();
  setFont(font);
  setFrame(false);
  // we have to respect the limits of QDate here, I fear:
  val->setRange(0, 8000);
  setValidator(val);
  connect(this, SIGNAL(returnPressed()), SLOT(yearEnteredSlot()));
}

void KDateInternalYearSelector::focusOutEvent(QFocusEvent*)
{
    emit(closeMe(1));
}

void
KDateInternalYearSelector::yearEnteredSlot()
{
  bool ok;
  int year;
  QDate date;
  // ----- check if this is a valid year:
  year=text().toInt(&ok);
  if(!ok)
    {
      KNotification::beep();
      return;
    }
  //date.setYMD(year, 1, 1);
  KGlobal::locale()->calendar()->setYMD(date, year, 1, 1);
  if(!date.isValid())
    {
      KNotification::beep();
      return;
    }
  result=year;
  emit(closeMe(1));
}

int
KDateInternalYearSelector::getYear()
{
  return result;
}

void
KDateInternalYearSelector::setYear(int year)
{
  QString temp;
  // -----
  temp.setNum(year);
  setText(temp);
}

KPopupFrame::KPopupFrame(QWidget* parent)
  : QFrame(parent, Qt::Popup), d(new KPopupFramePrivate(this))
{
  setFrameStyle(QFrame::Box|QFrame::Raised);
  setMidLineWidth(2);
}

KPopupFrame::~KPopupFrame()
{
  delete d;
}

void
KPopupFrame::keyPressEvent(QKeyEvent* e)
{
  if(e->key()==Qt::Key_Escape)
    {
      d->result=0; // rejected
      emit leaveModality();
      //qApp->exit_loop();
    }
}

void
KPopupFrame::close(int r)
{
  d->result=r;
  emit leaveModality();
  //qApp->exit_loop();
}

void
KPopupFrame::setMainWidget(QWidget* m)
{
  d->main=m;
  if(d->main)
    {
      resize(d->main->width()+2*frameWidth(), d->main->height()+2*frameWidth());
    }
}

void
KPopupFrame::resizeEvent(QResizeEvent*)
{
  if(d->main)
    {
      d->main->setGeometry(frameWidth(), frameWidth(),
          width()-2*frameWidth(), height()-2*frameWidth());
    }
}

void
KPopupFrame::popup(const QPoint &pos)
{
  // Make sure the whole popup is visible.
  QRect d = KGlobalSettings::desktopGeometry(pos);

  int x = pos.x();
  int y = pos.y();
  int w = width();
  int h = height();
  if (x+w > d.x()+d.width())
    x = d.width() - w;
  if (y+h > d.y()+d.height())
    y = d.height() - h;
  if (x < d.x())
    x = 0;
  if (y < d.y())
    y = 0;

  // Pop the thingy up.
  move(x, y);
  show();
}

int
KPopupFrame::exec(const QPoint &pos)
{
  popup(pos);
  repaint();
  QEventLoop eventLoop;
  connect(this, SIGNAL(leaveModality()),
          &eventLoop, SLOT(quit()));
  eventLoop.exec();

  hide();
  kDebug()<<d->result;
  return d->result;
}

int
KPopupFrame::exec(int x, int y)
{
  return exec(QPoint(x, y));
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
    return QVariant();
}

QVariant KDateTableDataModel::weekDayData( int day, int role ) const
{
    return QVariant();
}

QVariant KDateTableDataModel::weekNumberData( int week, int role ) const
{
    return QVariant();
}

//-------------
KDateTableDateDelegate::KDateTableDateDelegate()
{
}

QVariant KDateTableDateDelegate::data( const QDate &date, int role, KDateTableDataModel *model )
{
    //kDebug()<<date<<role<<model;
    if ( model == 0 ) {
        return QVariant();
    }
    return model->data( date, role );
}

QRectF KDateTableDateDelegate::paint( QPainter *painter, const StyleOptionViewItem &option, const QDate &date, KDateTableDataModel *model )
{
    //kDebug()<<date;
    painter->save();
    const KCalendarSystem * calendar = KGlobal::locale()->calendar();
    QRectF r;
    
    QPalette palette = option.palette;
    if ( option.state & QStyle::State_Enabled && option.state & QStyle::State_Active ) {
        palette.setCurrentColorGroup( QPalette::Active );
    } else {
        palette.setCurrentColorGroup( QPalette::Inactive );
    }

    QFont font = option.font;
    QColor textColor = palette.text();
    QBrush bg( palette.base() );
    Qt::Alignment align = option.displayAlignment;
    QString text = calendar->dayString(date, KCalendarSystem::ShortFormat);
    
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
        painter->setPen( palette.text() );
        painter->setPen( Qt::DotLine );
        painter->drawRect( option.rectF );
    } else if ( date == QDate::currentDate() ) {
        painter->setPen( palette.text() );
        painter->drawRect( option.rectF );
    }

    if ( option.state & QStyle::State_Selected ) {
        pen.setColor( palette.highlightedText() );
    }
    painter->setFont( font );
    painter->setPen( pen );
    painter->drawText( option.rectF, align, text, &r );
    
    painter->restore();
    return r;
}

//---------

KDateTableCustomDateDelegate::KDateTableCustomDateDelegate()
    : KDateTableDateDelegate()
{
}

QRectF KDateTableCustomDateDelegate::paint( QPainter *painter, const StyleOptionViewItem &option, const QDate &date, KDateTableDataModel *model )
{
    //kDebug()<<date;
    painter->save();
    const KCalendarSystem * calendar = KGlobal::locale()->calendar();
    QRectF r;
    bool paintRect=true;
    QColor textColor = KGlobalSettings::activeTextColor();
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
        //kDebug()<<"selected: "<<date;
        if ( option.state & QStyle::State_Enabled )
        {
          //kDebug()<<"enabled & selected: "<<date;
          painter->setPen(option.palette.color(QPalette::Highlight));
          painter->setBrush(option.palette.color(QPalette::Highlight));
        }
        else
        {
          //kDebug()<<"disabled & selected: "<<date;
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
      QString text = calendar->dayString(date, KCalendarSystem::ShortFormat);
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
KDateTableWeekDayDelegate::KDateTableWeekDayDelegate()
{
}

QVariant KDateTableWeekDayDelegate::data( int day, int role, KDateTableDataModel *model )
{
    //kDebug()<<day<<role<<model;
    if ( model == 0 ) {
        return QVariant();
    }
    return model->weekDayData( day, role );
}

QRectF KDateTableWeekDayDelegate::paint( QPainter *painter, const StyleOptionHeader &option, int daynum, KDateTableDataModel *model )
{
    //kDebug()<<daynum;
    painter->save();
    const KCalendarSystem * calendar = KGlobal::locale()->calendar();
    
    QPalette palette = option.palette;
    if ( option.state & QStyle::State_Active ) {
        palette.setCurrentColorGroup( QPalette::Active );
    } else {
        palette.setCurrentColorGroup( QPalette::Inactive );
    }
    QRectF rect;
    QFont font = KGlobalSettings::generalFont();
//    font.setBold(true);
    painter->setFont(font);
    
    QColor titleColor( palette.button() );
    QColor textColor( palette.buttonText() );

    painter->setPen(titleColor);
    painter->setBrush(titleColor);
    painter->drawRect(option.rectF);
    
    QString value = calendar->weekDayName( daynum, KCalendarSystem::ShortDayName );
    //kDebug()<<daynum<<": "<<value;
    if ( model ) {
        QVariant v = model->weekDayData( daynum, Qt::DisplayRole );
        if ( v.isValid() ) {
            value = v.toString();
        }
    }
    painter->setPen( textColor );
    painter->drawText(option.rectF, option.textAlignment, value, &rect);
    
//     painter->setPen( palette.color(QPalette::Text) );
//     painter->drawLine(QPointF(0, option.rectF.height()), QPointF(option.rectF.width(), option.rectF.height()));

    painter->restore();
    return rect;
}

//---------
KDateTableWeekNumberDelegate::KDateTableWeekNumberDelegate()
{
}

QVariant KDateTableWeekNumberDelegate::data( int week, int role, KDateTableDataModel *model )
{
    //kDebug()<<week<<role<<model;
    if ( model == 0 ) {
        return QVariant();
    }
    return model->weekNumberData( week, role );
}

QRectF KDateTableWeekNumberDelegate::paint( QPainter *painter, const StyleOptionHeader &option, int week, KDateTableDataModel *model )
{
    //kDebug();
    painter->save();
    QRectF result;
    QFont font = KGlobalSettings::generalFont();
    painter->setFont(font);

    QColor titleColor( option.palette.button() );
    QColor textColor( option.palette.buttonText() );

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

#include "kdatetable.moc"
#include "kdatetable_p.moc"
