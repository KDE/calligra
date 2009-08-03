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

#include "kdatepicker.h"
#include "kdatetable.h"
#include "kdatetable_p.h"

#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QFont>
#include <QtGui/QLayout>
#include <QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QToolButton>
#include <QtGui/QDoubleValidator>
#include <QtGui/QWidget>
#include <QPushButton>

#include <kcalendarsystem.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <knotification.h>

#include "kdatepicker.moc"

namespace KPlato
{

// Week numbers are defined by ISO 8601
// See http://www.merlyn.demon.co.uk/weekinfo.htm for details

class KDatePicker::KDatePickerPrivate
{
public:
    KDatePickerPrivate() : closeButton(0L), selectWeek(0L), todayButton(0), navigationLayout(0) {}

    void fillWeeksCombo(const QDate &date);

    QToolButton *closeButton;
    QComboBox *selectWeek;
    QPushButton *todayButton;
    QBoxLayout *navigationLayout;

    /// the year forward button
    QPushButton *yearForward;
    /// the year backward button
    QPushButton *yearBackward;
    /// the month forward button
    QPushButton *monthForward;
    /// the month backward button
    QPushButton *monthBackward;
    /// the button for selecting the month directly
    QPushButton *selectMonth;
    /// the button for selecting the year directly
    QPushButton *selectYear;
    /// the line edit to enter the date directly
    QLineEdit *line;
    /// the validator for the line edit:
    KDateValidator *val;
    /// the date table
    KDateTable *table;
    /// the widest month string in pixels:
    QSize maxMonthRect;

    /// the font size for the widget
    int fontsize;
};

void KDatePicker::fillWeeksCombo(const QDate &date)
{
  // every year can have a different number of weeks
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();

  // it could be that we had 53,1..52 and now 1..53 which is the same number but different
  // so always fill with new values

  d->selectWeek->clear();

  // We show all week numbers for all weeks between first day of year to last day of year
  // This of course can be a list like 53,1,2..52

  QDate day;
  int year = calendar->year(date);
  calendar->setYMD(day, year, 1, 1);
  int lastMonth = calendar->monthsInYear(day);
  QDate lastDay, firstDayOfLastMonth;
  calendar->setYMD(firstDayOfLastMonth, year, lastMonth, 1);
  calendar->setYMD(lastDay, year, lastMonth, calendar->daysInMonth(firstDayOfLastMonth));

  for (; day <= lastDay ; day = calendar->addDays(day, 7 /*calendar->daysOfWeek()*/) )
  {
    QString week = i18n("Week %1", calendar->weekNumber(day, &year));
    if ( year != calendar->year(day) ) week += '*';  // show that this is a week from a different year
    d->selectWeek->addItem(week);

    // make sure that the week of the lastDay is always inserted: in Chinese calendar
    // system, this is not always the case
    if(day < lastDay && day.daysTo(lastDay) < 7 && calendar->weekNumber(day) != calendar->weekNumber(lastDay))
      day = lastDay.addDays(-7);
  }
}

KDatePicker::KDatePicker(QWidget* parent)
  : QFrame(parent),d(new KDatePickerPrivate())
{
  init( QDate::currentDate() );
}

KDatePicker::KDatePicker(const QDate& dt, QWidget* parent)
  : QFrame(parent),d(new KDatePickerPrivate())
{
  init( dt );
}

void KDatePicker::init( const QDate &dt )
{
  setFocusPolicy( Qt::StrongFocus );

  QBoxLayout * topLayout = new QVBoxLayout(this);
  topLayout->setSpacing(0);
  topLayout->setMargin(0);

  d->navigationLayout = new QHBoxLayout();
  d->navigationLayout->setSpacing(0);
  d->navigationLayout->setMargin(0);
  topLayout->addLayout(d->navigationLayout);
  d->navigationLayout->addStretch();
  
  d->yearBackward = new QPushButton(this);
  //d->yearBackward->setAutoRaise(true);
  d->yearBackward->setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Fixed );
  d->yearBackward->setMaximumSize( QSize( 30, 30 ) );
  d->navigationLayout->addWidget(d->yearBackward);
  d->yearBackward->installEventFilter( this );
  
  d->monthBackward = new QPushButton(this);
  //d->monthBackward ->setAutoRaise(true);
  d->monthBackward->setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Fixed );
  d->monthBackward->setMaximumSize( QSize( 30, 30 ) );
  d->navigationLayout->addWidget(d->monthBackward);
  d->navigationLayout->addSpacing(KDialog::spacingHint());
  d->monthBackward->installEventFilter( this );

  d->selectMonth = new QPushButton(this);
  //d->selectMonth ->setAutoRaise(true);
  d->selectMonth->setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Fixed );
  d->selectMonth->setMaximumSize( QSize( 30, 30 ) );
  d->navigationLayout->addWidget(d->selectMonth);
  d->selectMonth->installEventFilter( this );

  d->selectYear = new QPushButton(this);
  d->selectYear->setCheckable(true);
  //d->selectYear->setAutoRaise(true);
  d->selectYear->setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Fixed );
  d->selectYear->setMaximumSize( QSize( 30, 30 ) );
  d->navigationLayout->addWidget(d->selectYear);
  d->navigationLayout->addSpacing(KDialog::spacingHint());
  d->selectYear->installEventFilter( this );

  d->monthForward = new QPushButton(this);
  //d->monthForward ->setAutoRaise(true);
  d->monthForward->setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Fixed );
  d->monthForward->setMaximumSize( QSize( 30, 30 ) );
  d->navigationLayout->addWidget(d->monthForward);
  
  d->yearForward = new QPushButton(this);
  //d->yearForward ->setFlat(true);
  d->yearForward->setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::Fixed );
  d->yearForward->setMaximumSize( QSize( 30, 30 ) );
  d->navigationLayout->addWidget(d->yearForward);
  d->yearForward->installEventFilter( this );

  d->navigationLayout->addStretch();

  d->line = new KLineEdit(this);
  d->val = new KDateValidator(this);
  
  Frame *f = new Frame( this );
  topLayout->addWidget( f );
  QVBoxLayout *l = new QVBoxLayout( f );
  
  d->table = new KDateTable(f);
  connect( d->table, SIGNAL( focusChanged( QFocusEvent* ) ), f, SLOT( updateFocus( QFocusEvent* ) ) );
  l->addWidget( d->table );
  f->setFocusProxy( d->table );
  d->table->installEventFilter( this );
  
  d->fontsize = KGlobalSettings::generalFont().pointSize();
  if (d->fontsize == -1)
     d->fontsize = QFontInfo(KGlobalSettings::generalFont()).pointSize();

  d->fontsize++; // Make a little bigger

  d->selectWeek = new QComboBox(this);  // read only week selection
  d->selectWeek->installEventFilter( this );
  
  d->todayButton = new QPushButton(this);
  d->todayButton->setIcon(KIcon("go-jump-today"));
  d->todayButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  d->todayButton->setMaximumSize( QSize( 25, 25 ) );
  d->todayButton->installEventFilter( this );

  d->yearForward->setToolTip(i18n("Next year"));
  d->yearBackward->setToolTip(i18n("Previous year"));
  d->monthForward->setToolTip(i18n("Next month"));
  d->monthBackward->setToolTip(i18n("Previous month"));
  d->selectWeek->setToolTip(i18n("Select a week"));
  d->selectMonth->setToolTip(i18n("Select a month"));
  d->selectYear->setToolTip(i18n("Select a year"));
  d->todayButton->setToolTip(i18n("Select the current day"));

  // -----
  setFontSize(d->fontsize);
  d->line->setValidator(d->val);
  d->line->installEventFilter( this );
  if ( QApplication::isRightToLeft() )
  {
      d->yearForward->setIcon(KIcon(QLatin1String("arrow-left-double")));
      d->yearBackward->setIcon(KIcon(QLatin1String("arrow-right-double")));
      d->monthForward->setIcon(KIcon(QLatin1String("arrow-left")));
      d->monthBackward->setIcon(KIcon(QLatin1String("arrow-right")));
  }
  else
  {
      d->yearForward->setIcon(KIcon(QLatin1String("arrow-right-double")));
      d->yearBackward->setIcon(KIcon(QLatin1String("arrow-left-double")));
      d->monthForward->setIcon(KIcon(QLatin1String("arrow-right")));
      d->monthBackward->setIcon(KIcon(QLatin1String("arrow-left")));
  }

  connect(d->table, SIGNAL(dateChanged(const QDate&)), SLOT(dateChangedSlot(const QDate&)));
  connect(d->table, SIGNAL(tableClicked()), SLOT(tableClickedSlot()));
  connect(d->monthForward, SIGNAL(clicked()), SLOT(monthForwardClicked()));
  connect(d->monthBackward, SIGNAL(clicked()), SLOT(monthBackwardClicked()));
  connect(d->yearForward, SIGNAL(clicked()), SLOT(yearForwardClicked()));
  connect(d->yearBackward, SIGNAL(clicked()), SLOT(yearBackwardClicked()));
  connect(d->selectWeek, SIGNAL(activated(int)), SLOT(weekSelected(int)));
  connect(d->todayButton, SIGNAL(clicked()), SLOT(todayButtonClicked()));
  connect(d->selectMonth, SIGNAL(clicked()), SLOT(selectMonthClicked()));
  connect(d->selectYear, SIGNAL(toggled(bool)), SLOT(selectYearClicked()));
  connect(d->line, SIGNAL(returnPressed()), SLOT(lineEnterPressed()));
  d->table->setFocus();


  QBoxLayout * bottomLayout = new QHBoxLayout();
  bottomLayout->setMargin(0);
  bottomLayout->setSpacing(0);
  topLayout->addLayout(bottomLayout);

  bottomLayout->addWidget(d->todayButton);
  bottomLayout->addWidget(d->line);
  bottomLayout->addWidget(d->selectWeek);

  d->table->setDate(dt);
  dateChangedSlot(dt);  // needed because table emits changed only when newDate != oldDate
}

KDatePicker::~KDatePicker()
{
  delete d;
}

bool
KDatePicker::eventFilter(QObject *o, QEvent *e )
{
/*   if ( e->type() == QEvent::KeyPress ) {
      QKeyEvent *k = (QKeyEvent *)e;

      if ( (k->key() == Qt::Key_PageUp) ||
           (k->key() == Qt::Key_PageDown)  ||
           (k->key() == Qt::Key_Up)    ||
           (k->key() == Qt::Key_Down) )
       {
          QApplication::sendEvent( d->table, e );
          d->table->setFocus();
          return true; // eat event
       }
   }*/
   if ( o != this && ( e->type() == QEvent::FocusIn || e->type() == QEvent::FocusOut ) ) {
       update();
   }
   return QFrame::eventFilter( o, e );
}

void
KDatePicker::resizeEvent(QResizeEvent* e)
{
  QWidget::resizeEvent(e);
}

void
KDatePicker::paintEvent(QPaintEvent *e)
{
    //kDebug()<<e;
    QPainter paint(this);
    drawFrame(&paint);
}

void
KDatePicker::drawFrame(QPainter *p)
{
    QPoint p1, p2;
    QStyleOptionFrame opt;
    opt.init(this);
    QList<QWidget*> lst = findChildren<QWidget*>();
    foreach ( QWidget *w, lst ) {
        if ( w->hasFocus() ) {
            opt.state |= QStyle::State_HasFocus;
            break;
        }
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

void
KDatePicker::dateChangedSlot(const QDate &date)
{
    kDebug()<< "KDatePicker::dateChangedSlot: date changed (" << date.year() << "/" << date.month() << "/" << date.day() << ").";

    const KCalendarSystem * calendar = KGlobal::locale()->calendar();

    d->line->setText(KGlobal::locale()->formatDate(date, KLocale::ShortDate));
    d->selectMonth->setText(calendar->monthName(date, KCalendarSystem::ShortName));
    fillWeeksCombo(date);

    // calculate the item num in the week combo box; normalize selected day so as if 1.1. is the first day of the week
    QDate firstDay;
    calendar->setYMD(firstDay, calendar->year(date), 1, 1);
    d->selectWeek->setCurrentIndex((calendar->dayOfYear(date) + calendar->dayOfWeek(firstDay) - 2) / 7/*calendar->daysInWeek()*/);
    d->selectYear->setText(calendar->yearString(date, KCalendarSystem::ShortFormat));

    emit(dateChanged(date));
}

void
KDatePicker::tableClickedSlot()
{
  kDebug()<< "KDatePicker::tableClickedSlot: table clicked.";
  emit(dateSelected(d->table->date()));
  emit(tableClicked());
}

const QDate &
KDatePicker::date() const
{
    return d->table->date();
}

bool
KDatePicker::setDate(const QDate& date)
{
    if(date.isValid())
    {
        d->table->setDate(date);  // this also emits dateChanged() which then calls our dateChangedSlot()
        return true;
    }
    else
    {
        kDebug()<<"KDatePicker::setDate: refusing to set invalid date.";
        return false;
    }
}

void
KDatePicker::monthForwardClicked()
{
    QDate temp;
    temp = KGlobal::locale()->calendar()->addMonths( d->table->date(), 1 );

    setDate( temp );
}

void
KDatePicker::monthBackwardClicked()
{
    QDate temp;
    temp = KGlobal::locale()->calendar()->addMonths( d->table->date(), -1 );

    setDate( temp );
}

void
KDatePicker::yearForwardClicked()
{
    QDate temp;
    temp = KGlobal::locale()->calendar()->addYears( d->table->date(), 1 );

    setDate( temp );
}

void
KDatePicker::yearBackwardClicked()
{
    QDate temp;
    temp = KGlobal::locale()->calendar()->addYears( d->table->date(), -1 );

    setDate( temp );
}

void
KDatePicker::weekSelected(int week)
{
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();

  QDate date = d->table->date();
  int year = calendar->year(date);

  calendar->setYMD(date, year, 1, 1);  // first day of selected year

  // calculate the first day in the selected week (day 1 is first day of week)
  date = calendar->addDays(date, week * 7/*calendar->daysOfWeek()*/ -calendar->dayOfWeek(date) + 1);

  setDate(date);
}

void
KDatePicker::selectMonthClicked()
{
  // every year can have different month names (in some calendar systems)
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();
  QDate date = d->table->date();
  const int months = calendar->monthsInYear(date);

  QMenu popup(d->selectMonth);

  for (int i = 1; i <= months; i++)
    popup.addAction(calendar->monthName(i, calendar->year(date)))->setData(i);

  //QMenuItem *item = popup.findItem (calendar->month(date) - 1);
  QAction *item=popup.actions()[calendar->month(date)-1];
  if (item) // if this happens the above should already given an assertion
    popup.setActiveAction(item);

  if ( (item = popup.exec(d->selectMonth->mapToGlobal(QPoint(0, 0)), item)) == 0 ) return;  // canceled

  int day = calendar->day(date);
  // ----- construct a valid date in this month:
  calendar->setYMD(date, calendar->year(date), item->data().toInt(), 1);
  date = date.addDays(qMin(day, calendar->daysInMonth(date)) - 1);
  // ----- set this month
  setDate(date);
}

void
KDatePicker::selectYearClicked()
{
  const KCalendarSystem * calendar = KGlobal::locale()->calendar();

  if (!d->selectYear->isChecked ())
  {
    return;
  }

  int year;
  KPopupFrame* popup = new KPopupFrame(this);
  KDateInternalYearSelector* picker = new KDateInternalYearSelector(popup);
  // -----
  picker->resize(picker->sizeHint());
  picker->setYear( d->table->date().year() );
  picker->selectAll();
  popup->setMainWidget(picker);
  connect(picker, SIGNAL(closeMe(int)), popup, SLOT(close(int)));
  picker->setFocus();
  if(popup->exec(d->selectYear->mapToGlobal(QPoint(0, d->selectMonth->height()))))
    {
      QDate date;
      int day;
      // -----
      year=picker->getYear();
      date=d->table->date();
      day=calendar->day(date);
      // ----- construct a valid date in this month:
      //date.setYMD(year, date.month(), 1);
      //date.setYMD(year, date.month(), qMin(day, date.daysInMonth()));
      calendar->setYMD(date, year, calendar->month(date),
                       qMin(day, calendar->daysInMonth(date)));
      // ----- set this month
      setDate(date);
    } else {
      //KNotification::beep();
    }
  d->selectYear->setChecked( false );
  delete popup;
}

// ####### KDE4: setEnabled isn't virtual anymore, so this isn't polymorphic.
// Better reimplement changeEvent() instead.
void
KDatePicker::setEnabled(bool enable)
{
  QWidget *widgets[]= {
    d->yearForward, d->yearBackward, d->monthForward, d->monthBackward,
    d->selectMonth, d->selectYear,
    d->line, d->table, d->selectWeek, d->todayButton };
  const int Size=sizeof(widgets)/sizeof(widgets[0]);
  int count;
  // -----
  for(count=0; count<Size; ++count)
    {
      widgets[count]->setEnabled(enable);
    }
}

KDateTable *KDatePicker::dateTable() const
{
    return d->table;
}

void
KDatePicker::lineEnterPressed()
{
  QDate temp;
  // -----
  if(d->val->date(d->line->text(), temp)==QValidator::Acceptable)
    {
        kDebug()<< "KDatePicker::lineEnterPressed: valid date entered.";
        emit(dateEntered(temp));
        setDate(temp);
    } else {
      KNotification::beep();
      kDebug()<< "KDatePicker::lineEnterPressed: invalid date entered.";
    }
}

void
KDatePicker::todayButtonClicked()
{
  setDate(QDate::currentDate());
}

QSize
KDatePicker::sizeHint() const
{
  return QWidget::sizeHint();
}

void
KDatePicker::setFontSize(int s)
{
  QWidget *buttons[]= {
    // yearBackward,
    // monthBackward,
    d->selectMonth,
    d->selectYear,
    // monthForward,
    // yearForward
  };
  const int NoOfButtons=sizeof(buttons)/sizeof(buttons[0]);
  int count;
  QFont font;
  QRect r;
  // -----
  d->fontsize=s;
  for(count=0; count<NoOfButtons; ++count)
    {
      font=buttons[count]->font();
      font.setPointSize(s);
      buttons[count]->setFont(font);
    }
  QFontMetrics metrics(d->selectMonth->fontMetrics());

  for (int i = 1; ; ++i)
    {
      QString str = KGlobal::locale()->calendar()->monthName(i,
         KGlobal::locale()->calendar()->year(d->table->date()), KCalendarSystem::ShortName);
      if (str.isNull()) break;
      r=metrics.boundingRect(str);
      d->maxMonthRect.setWidth(qMax(r.width(), d->maxMonthRect.width()));
      d->maxMonthRect.setHeight(qMax(r.height(),  d->maxMonthRect.height()));
    }

  QStyleOptionToolButton opt;

  // stolen from KToolBarButton
  opt.init(this);
  opt.font      = d->selectMonth->font();
  opt.icon      = d->selectMonth->icon();
  opt.text      = d->selectMonth->text();
  opt.features  = d->selectMonth->menu() ? QStyleOptionToolButton::Menu : QStyleOptionToolButton::None; //### FIXME: delay?
  opt.subControls       = QStyle::SC_All;
  opt.activeSubControls = 0; //### FIXME: !!

  QSize metricBound = style()->sizeFromContents(QStyle::CT_ToolButton,
                                               &opt,
                                               d->maxMonthRect, d->selectMonth);
  d->selectMonth->setMinimumSize(metricBound);

  d->table->setFontSize(s);
}

int KDatePicker::fontSize() const
{
    return d->fontsize;
}


void
KDatePicker::setCloseButton( bool enable )
{
    if ( enable == (d->closeButton != 0L) )
        return;

    if ( enable ) {
        d->closeButton = new QToolButton( this );
        d->closeButton->setAutoRaise(true);
        d->navigationLayout->addSpacing(KDialog::spacingHint());
        d->navigationLayout->addWidget(d->closeButton);
        d->closeButton->setToolTip(i18n("Close"));
        d->closeButton->setIcon( SmallIcon("list-remove") );
        connect( d->closeButton, SIGNAL( clicked() ),
                 topLevelWidget(), SLOT( close() ) );
    }
    else {
        delete d->closeButton;
        d->closeButton = 0L;
    }

    updateGeometry();
}

bool KDatePicker::hasCloseButton() const
{
    return (d->closeButton);
}

} //namespace KPlato
