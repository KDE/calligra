/* This file is part of the KDE project
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2004 Dag Andersen <danders@get2net.dk>

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

/* This is based on KDatePicker. */

#include "kptcalendarpanel.h"
#include "kptdatetable.h"
#include "kptcalendar.h"


#include <kglobal.h>
#include <kapplication.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <kiconloader.h>
#include <q3frame.h>
#include <qpainter.h>
#include <qdialog.h>
#include <qstyle.h>
#include <qtoolbutton.h>
#include <QToolTip>
#include <QFont>
//Added by qt3to4:
#include <QKeyEvent>
#include <QEvent>
#include <QResizeEvent>
#include <klineedit.h>
#include <qvalidator.h>
#include <kdebug.h>
#include <knotifyclient.h>

namespace KPlato
{

class CalendarPanel::CalendarPanelPrivate
{
public:
    CalendarPanelPrivate() : closeButton(0L), selectWeek(0L) {}

    QToolButton *closeButton;
    QToolButton *selectWeek;
};


CalendarPanel::CalendarPanel(QWidget *parent, QDate dt, const char *name, Qt::WFlags f)
  : Q3Frame(parent,name, f)
{
  init( dt );
}

CalendarPanel::CalendarPanel( QWidget *parent, const char *name )
  : Q3Frame(parent,name)
{
    init( QDate::currentDate() );
}

void CalendarPanel::init( const QDate &dt )
{
  yearForward = new QToolButton(this);
  yearBackward = new QToolButton(this);
  monthForward = new QToolButton(this);
  monthBackward = new QToolButton(this);
  selectMonth = new QToolButton(this);
  selectYear = new QToolButton(this);
  line = new KLineEdit(this);
  val = new DateValidator(this);
  table = new DateTable(this, dt, "Calendar table", 0);
  fontsize = 10;

  d = new CalendarPanelPrivate();
  d->selectWeek = new QToolButton( this );

  yearForward->setToolTip( i18n("Next year"));
  yearBackward->setToolTip( i18n("Previous year"));
  monthForward->setToolTip( i18n("Next month"));
  monthBackward->setToolTip( i18n("Previous month"));
  d->selectWeek->setToolTip( i18n("Select a week"));
  selectMonth->setToolTip( i18n("Select a month"));
  selectYear->setToolTip( i18n("Select a year"));

  // -----
  setFontSize(10);
  line->setValidator(val);
  line->installEventFilter( this );
  yearForward->setPixmap(BarIcon(QString::fromLatin1("2rightarrow")));
  yearBackward->setPixmap(BarIcon(QString::fromLatin1("2leftarrow")));
  monthForward->setPixmap(BarIcon(QString::fromLatin1("1rightarrow")));
  monthBackward->setPixmap(BarIcon(QString::fromLatin1("1leftarrow")));
  setDate(dt); // set button texts
  connect(table, SIGNAL(dateChanged(QDate)), SLOT(dateChangedSlot(QDate)));
  connect(table, SIGNAL(tableClicked()), SLOT(tableClickedSlot()));
  connect(monthForward, SIGNAL(clicked()), SLOT(monthForwardClicked()));
  connect(monthBackward, SIGNAL(clicked()), SLOT(monthBackwardClicked()));
  connect(yearForward, SIGNAL(clicked()), SLOT(yearForwardClicked()));
  connect(yearBackward, SIGNAL(clicked()), SLOT(yearBackwardClicked()));
  connect(d->selectWeek, SIGNAL(clicked()), SLOT(selectWeekClicked()));
  connect(selectMonth, SIGNAL(clicked()), SLOT(selectMonthClicked()));
  connect(selectYear, SIGNAL(clicked()), SLOT(selectYearClicked()));
  connect(line, SIGNAL(returnPressed()), SLOT(lineEnterPressed()));

  connect(table, SIGNAL(weekdaySelected(int)), SLOT(slotWeekdaySelected(int)));
  connect(table, SIGNAL(weekSelected(int, int)), SLOT(slotWeekSelected(int, int)));
  connect(table, SIGNAL(selectionCleared()), SLOT(slotSelectionCleared()));

  table->setFocus();
}

CalendarPanel::~CalendarPanel()
{
  delete d;
}

bool
CalendarPanel::eventFilter(QObject *o, QEvent *e )
{
   if ( e->type() == QEvent::KeyPress ) {
      QKeyEvent *k = (QKeyEvent *)e;

      if ( (k->key() == Qt::Key_PageUp) ||
           (k->key() == Qt::Key_PageDown)  ||
           (k->key() == Qt::Key_Up)    ||
           (k->key() == Qt::Key_Down) )
       {
          QApplication::sendEvent( table, e );
          table->setFocus();
          return TRUE; // eat event
       }
   }
   return Q3Frame::eventFilter( o, e );
}

void
CalendarPanel::resizeEvent(QResizeEvent*)
{
    QWidget *buttons[] = {
	yearBackward,
	    monthBackward,
	    selectMonth,
	    selectYear,
	    monthForward,
	    yearForward,
	    d->closeButton
    };
    const int NoOfButtons=sizeof(buttons)/sizeof(buttons[0]);
    QSize sizes[NoOfButtons];
    int buttonHeight=0;
    int count;
    int w=0;
    int x=0;
    // ----- calculate button row height:
    for(count=0; count<NoOfButtons; ++count) {
        if ( buttons[count] ) { // closeButton may be 0L
            sizes[count]=buttons[count]->sizeHint();
            buttonHeight=qMax(buttonHeight, sizes[count].height());
        }
        else
            sizes[count] = QSize(0,0); // closeButton
    }

    // ----- calculate size of the month button:
    for(count=0; count<NoOfButtons; ++count) {
        if(buttons[count]==selectMonth) {
            //QSize metricBound = style()->sizeFromContents(QStyle::CT_ToolButton, selectMonth, maxMonthRect);
            //sizes[count].setWidth(qMax(metricBound.width(), maxMonthRect.width()+2*QApplication::style()->pixelMetric(QStyle::PM_ButtonMargin)));
            sizes[count].setWidth( selectMonth->sizeHint().width() );
        }
    }
    // ----- center buttons
    w=0;
    for(count=0; count<NoOfButtons; ++count)
    {
        w +=sizes[count].width();
    }
    x = (qMax(w, width())-w)/2;

    // ----- place the buttons:
    for(count=0; count<NoOfButtons; ++count)
    {
        w=sizes[count].width();
        if ( buttons[count] )
            buttons[count]->setGeometry(x, 0, w, buttonHeight);
        x+=w;
    }
    // ----- place the line edit for direct input:
    sizes[0]=line->sizeHint();
    int week_width=d->selectWeek->fontMetrics().width(i18n("Week XX"))+((d->closeButton != 0L) ? 50 : 20);
    line->setGeometry(0, height()-sizes[0].height(), width()-week_width, sizes[0].height());
    d->selectWeek->setGeometry(width()-week_width, height()-sizes[0].height(), week_width, sizes[0].height());
    // ----- adjust the table:
    table->setGeometry(0, buttonHeight, width(),
		       height()-buttonHeight-sizes[0].height());
}

void
CalendarPanel::dateChangedSlot(QDate date)
{
    //kDebug() << "CalendarPanel::dateChangedSlot: date changed (" << date.year() << "/" << date.month() << "/" << date.day() << ")." << endl;
    line->setText(KGlobal::locale()->formatDate(date, true));
    d->selectWeek->setText(i18n("Week %1", weekOfYear(date)));
    selectMonth->setText(KGlobal::locale()->calendar()->monthName(date.month(), false));
    selectYear->setText(date.toString("yyyy"));
    emit(dateChanged(date));
}

void
CalendarPanel::tableClickedSlot()
{
  //kDebug() << "CalendarPanel::tableClickedSlot: table clicked." << endl;
  emit(dateSelected(table->getDate()));
  emit(tableClicked());
}

const QDate&
CalendarPanel::getDate() const
{
  return table->getDate();
}

const QDate &
CalendarPanel::date() const
{
    return table->getDate();
}

bool
CalendarPanel::setDate(const QDate& date)
{
    if(date.isValid()) {
	QString temp;
	// -----
	table->setDate(date);
	d->selectWeek->setText(i18n("Week %1", weekOfYear(date)));
	selectMonth->setText(KGlobal::locale()->calendar()->monthName(date.month(), false));
	temp.setNum(date.year());
	selectYear->setText(temp);
	line->setText(KGlobal::locale()->formatDate(date, true));
	return true;
    } else {
	kDebug() << "CalendarPanel::setDate: refusing to set invalid date." << endl;
	return false;
    }
}

void
CalendarPanel::monthForwardClicked()
{
    setDate( table->getDate().addMonths(1) );
}

void
CalendarPanel::monthBackwardClicked()
{
    setDate( table->getDate().addMonths(-1) );
}

void
CalendarPanel::yearForwardClicked()
{
    setDate( table->getDate().addYears(1) );
}

void
CalendarPanel::yearBackwardClicked()
{
    setDate( table->getDate().addYears(-1) );
}

void
CalendarPanel::selectWeekClicked()
{
  int week;
  PopupFrame* popup = new PopupFrame(this);
  DateInternalWeekSelector* picker = new DateInternalWeekSelector(fontsize, popup);
  // -----
  picker->resize(picker->sizeHint());
  popup->setMainWidget(picker);
  connect(picker, SIGNAL(closeMe(int)), popup, SLOT(close(int)));
  picker->setFocus();
  if(popup->exec(d->selectWeek->mapToGlobal(QPoint(0, d->selectWeek->height()))))
    {
      QDate date;
      int year;
      // -----
      week=picker->getWeek();
      date=table->getDate();
      year=date.year();
      // ----- find the first selectable day in this week (hacky solution :)
      date.setYMD(year, 1, 1);
      while (weekOfYear(date)>50)
          date=date.addDays(1);
      while (weekOfYear(date)<week && (week!=53 || (week==53 &&
            (weekOfYear(date)!=52 || weekOfYear(date.addDays(1))!=1))))
          date=date.addDays(1);
      if (week==53 && weekOfYear(date)==52)
          while (weekOfYear(date.addDays(-1))==52)
              date=date.addDays(-1);
      // ----- set this date
      setDate(date);
    } else {
         KNotifyClient::beep();
    }
  delete popup;
}

void
CalendarPanel::selectMonthClicked()
{
  int month;
  PopupFrame* popup = new PopupFrame(this);
  DateInternalMonthPicker* picker = new DateInternalMonthPicker(fontsize, popup);
  // -----
  picker->resize(picker->sizeHint());
  popup->setMainWidget(picker);
  picker->setFocus();
  connect(picker, SIGNAL(closeMe(int)), popup, SLOT(close(int)));
  if(popup->exec(selectMonth->mapToGlobal(QPoint(0, selectMonth->height()))))
    {
      QDate date;
      int day;
      // -----
      month=picker->getResult();
      date=table->getDate();
      day=date.day();
      // ----- construct a valid date in this month:
      date.setYMD(date.year(), month, 1);
      date.setYMD(date.year(), month, qMin(day, date.daysInMonth()));
      // ----- set this month
      setDate(date);
    } else {
      KNotifyClient::beep();
    }
  delete popup;
}

void
CalendarPanel::selectYearClicked()
{
  int year;
  PopupFrame* popup = new PopupFrame(this);
  DateInternalYearSelector* picker = new DateInternalYearSelector(fontsize, popup);
  // -----
  picker->resize(picker->sizeHint());
  popup->setMainWidget(picker);
  connect(picker, SIGNAL(closeMe(int)), popup, SLOT(close(int)));
  picker->setFocus();
  if(popup->exec(selectYear->mapToGlobal(QPoint(0, selectMonth->height()))))
    {
      QDate date;
      int day;
      // -----
      year=picker->getYear();
      date=table->getDate();
      day=date.day();
      // ----- construct a valid date in this month:
      date.setYMD(year, date.month(), 1);
      date.setYMD(year, date.month(), qMin(day, date.daysInMonth()));
      // ----- set this month
      setDate(date);
    } else {
      KNotifyClient::beep();
    }
  delete popup;
}

void
CalendarPanel::setEnabled(bool enable)
{
  QWidget *widgets[]= {
    yearForward, yearBackward, monthForward, monthBackward,
    selectMonth, selectYear,
    line, table, d->selectWeek };
  const int Size=sizeof(widgets)/sizeof(widgets[0]);
  int count;
  // -----
  for(count=0; count<Size; ++count)
    {
      widgets[count]->setEnabled(enable);
    }
    table->setEnabled(enable);
}

void
CalendarPanel::lineEnterPressed()
{
  QDate temp;
  // -----
  if(val->date(line->text(), temp)==QValidator::Acceptable)
    {
        //kDebug() << "CalendarPanel::lineEnterPressed: valid date entered." << endl;
        emit(dateEntered(temp));
        setDate(temp);
    } else {
      KNotifyClient::beep();
      //kDebug() << "CalendarPanel::lineEnterPressed: invalid date entered." << endl;
    }
}

QSize
CalendarPanel::sizeHint() const
{
  QSize tableSize=table->sizeHint();
  QWidget *buttons[]={
    yearBackward,
    monthBackward,
    selectMonth,
    selectYear,
    monthForward,
    yearForward,
    d->closeButton
  };
  const int NoOfButtons=sizeof(buttons)/sizeof(buttons[0]);
  QSize sizes[NoOfButtons];
  int cx=0, cy=0, count;
  // ----- store the size hints:
    for(count=0; count<NoOfButtons; ++count) {
        if ( buttons[count] )
            sizes[count]=buttons[count]->sizeHint();
        else
            sizes[count] = QSize(0,0);

        if(buttons[count]==selectMonth) {
            // what's this doing differently from sizeHint?
            //QSize metricBound = style()->sizeFromContents(QStyle::CT_ToolButton, selectMonth, maxMonthRect);
            //cx+=qMax(metricBound.width(), maxMonthRect.width()+2*QApplication::style()->pixelMetric(QStyle::PM_ButtonMargin));
            cx += qMax( selectMonth->sizeHint().width(), maxMonthRect.width()+2*QApplication::style()->pixelMetric(QStyle::PM_ButtonMargin) );
    	} else {
	        cx+=sizes[count].width();
	    }
        cy=qMax(sizes[count].height(), cy);
    }
    // ----- calculate width hint:
    cx=qMax(cx, tableSize.width()); // line edit ignored
    // ----- calculate height hint:
    cy+=tableSize.height()+line->sizeHint().height();
    return QSize(cx, cy);
}

void
CalendarPanel::setFontSize(int s)
{
  QWidget *buttons[]= {
    // yearBackward,
    // monthBackward,
    selectMonth,
    selectYear,
    // monthForward,
    // yearForward
  };
  const int NoOfButtons=sizeof(buttons)/sizeof(buttons[0]);
  int count;
  QFont font;
  QRect r;
  // -----
  fontsize=s;
  for(count=0; count<NoOfButtons; ++count)
    {
      font=buttons[count]->font();
      font.setPointSize(s);
      buttons[count]->setFont(font);
    }
  QFontMetrics metrics(selectMonth->fontMetrics());
  for(int i=1; i <= 12; ++i)
    { // maxMonthRect is used by sizeHint()
      r=metrics.boundingRect(KGlobal::locale()->calendar()->monthName(i, false));
      maxMonthRect.setWidth(qMax(r.width(), maxMonthRect.width()));
      maxMonthRect.setHeight(qMax(r.height(),  maxMonthRect.height()));
    }
  table->setFontSize(s);
}

void
CalendarPanel::setCloseButton( bool enable )
{
    if ( enable == (d->closeButton != 0L) )
        return;

    if ( enable ) {
        d->closeButton = new QToolButton( this );
        d->closeButton->setToolTip( i18n("Close"));
        d->closeButton->setPixmap( SmallIcon("remove") );
        connect( d->closeButton, SIGNAL( clicked() ),
                 topLevelWidget(), SLOT( close() ) );
    }
    else {
        delete d->closeButton;
        d->closeButton = 0L;
    }

    updateGeometry();
}

bool CalendarPanel::hasCloseButton() const
{
    return (d->closeButton != 0L);
}

int CalendarPanel::weekOfYear(QDate date)
{
    // Calculate ISO 8601 week number (taken from glibc/Gnumeric)
    int year, week, wday, jan1wday, nextjan1wday;
    QDate jan1date, nextjan1date;

    year=date.year();
    wday=date.dayOfWeek();

    jan1date=QDate(year,1,1);
    jan1wday=jan1date.dayOfWeek();

    week = (date.dayOfYear()-1 + jan1wday-1)/7 + ((jan1wday-1) == 0 ? 1 : 0);

    /* Does date belong to last week of previous year? */
    if ((week == 0) && (jan1wday > 4 /*THURSDAY*/)) {
        QDate tmpdate=QDate(year-1,12,31);
        return weekOfYear(tmpdate);
    }

    if ((jan1wday <= 4 /*THURSDAY*/) && (jan1wday > 1 /*MONDAY*/))
        week++;

    if (week == 53) {
        nextjan1date=QDate(year+1, 1, 1);
        nextjan1wday = nextjan1date.dayOfWeek();
        if (nextjan1wday <= 4 /*THURSDAY*/)
            week = 1;
    }

    return week;
}

void CalendarPanel::slotWeekdaySelected(int day) {
    //kDebug()<<k_funcinfo<<endl;
    emit weekdaySelected(day);
}

void CalendarPanel::slotWeekSelected(int week, int year) {
    //kDebug()<<k_funcinfo<<endl;
    emit weekSelected(week, year);
}

void CalendarPanel::setCalendar(Calendar *cal) {
    //kDebug()<<k_funcinfo<<endl;
    table->clear();
    if (cal) {
        table->setMarkedWeekdays(cal->weekdaysMap());
        Q3PtrListIterator<CalendarDay> it = cal->days();
        //kDebug()<<k_funcinfo<<"Days="<<it.count()<<endl;
        for (; it.current(); ++it) {
            if (it.current()->state() != Map::None) {
                table->addMarkedDate(it.current()->date(), it.current()->state());
            //kDebug()<<k_funcinfo<<"Added day: "<<it.current()->date().toString()<<"="<<it.current()->state()<<endl;
            }
        }
        setEnabled(true);
        table->setFocus();
    }
}

DateMap  CalendarPanel::selectedDates() {
    return table->selectedDates();
}

IntMap  CalendarPanel::selectedWeekdays() {
    return table->selectedWeekdays();
}

DateMap  CalendarPanel::markedDates() {
    return table->markedDates();
}

IntMap  CalendarPanel::markedWeekdays() {
    return table->markedWeekdays();
}

void CalendarPanel::clear() {
    table->clear();
    setEnabled(false);
}

void CalendarPanel::markSelected(int state) {
    table->markSelected(state);
 }

void CalendarPanel::slotSelectionCleared() {
    emit selectionCleared();
 }

void CalendarPanel::virtual_hook( int /*id*/, void* /*data*/ )
{ /*BASE::virtual_hook( id, data );*/ }

}  //KPlato namespace

#include "kptcalendarpanel.moc"
