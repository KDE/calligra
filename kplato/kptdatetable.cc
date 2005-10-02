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

#include "kptdatetable.h"
#include "kptmap.h"

#include <kapplication.h>
#include <knotifyclient.h>
#include <kcalendarsystem.h>

#include <qdatetime.h>
#include <qstring.h>
#include <qpen.h>
#include <qpainter.h>
#include <qdialog.h>
#include <assert.h>
#include <qlayout.h>
#include <kglobalsettings.h>

#include <kdebug.h>

namespace KPlato
{

KPTDateValidator::KPTDateValidator(QWidget* parent, const char* name)
    : QValidator(parent, name)
{
}

QValidator::State
KPTDateValidator::validate(QString& text, int&) const
{
  QDate temp;
  // ----- everything is tested in date():
  return date(text, temp);
}

QValidator::State
KPTDateValidator::date(const QString& text, QDate& d) const
{
  QDate tmp = KGlobal::locale()->readDate(text);
  if (!tmp.isNull())
    {
      d = tmp;
      return Acceptable;
    } else
      return Valid;
}

void
KPTDateValidator::fixup( QString& ) const
{

}


KPTDateTable::KPTDateTable(QWidget *parent, QDate date_, const char* name, WFlags f)
    : QGridView(parent, name, f),
      m_enabled(true)
{
    //kdDebug()<<k_funcinfo<<endl;
    m_dateStartCol = 1;
    m_selectedDates.clear();
    m_selectedWeekdays.clear();
    m_selectedWeeks.clear();

    QPair<int, int> p(0,0);
    m_weeks.fill(p, 7);

    setFontSize(10);
    if(!date_.isValid()) {
        kdError() <<k_funcinfo<<"Given date is invalid, using current date." << endl;
        date_=QDate::currentDate();
    }
    setFocusPolicy( QWidget::StrongFocus );
    setNumCols(7+m_dateStartCol); // 7 days a week + maybe 1 for weeknumbers
    setNumRows(7); // 6 weeks max + headline

    setHScrollBarMode(AlwaysOff);
    setVScrollBarMode(AlwaysOff);
    viewport()->setEraseColor(KGlobalSettings::baseColor());
    setDate(date_); // this initializes firstday, numdays, numDaysPrevMonth

    colorBackgroundHoliday = QColor(0, 245, 255, QColor::Hsv);
    //colorBackgroundHoliday = colorBackgroundHoliday.light();
    colorBackgroundWorkday = QColor(208, 230, 240, QColor::Hsv);;
    //colorBackgroundWorkday = colorBackgroundWorkday.light();
    colorTextHoliday = black;
    colorTextWorkday = black;
    colorLine = black;
    backgroundSelectColor = KGlobalSettings::highlightColor();
    penSelectColor=KGlobalSettings::baseColor();

}

void KPTDateTable::paintWeekday(QPainter *painter, int col) {
    QRect rect;
    int w=cellWidth();
    int h=cellHeight();

    QFont font = KGlobalSettings::generalFont();
    font.setBold(true);
    if (!m_enabled)
        font.setItalic(true);
    painter->setFont(font);

    int day = weekday(col);

    //kdDebug()<<k_funcinfo<<" col="<<col<<" day="<<day<<" name="<<daystr<<endl;

    painter->setBrush(KGlobalSettings::baseColor());
    painter->setPen(KGlobalSettings::baseColor());
    painter->drawRect(0, 0, w, h);
    painter->setPen(KGlobalSettings::textColor());

    if (m_markedWeekdays.state(day) == KPTMap::Working) {
        painter->setPen(colorBackgroundWorkday);
        painter->setBrush(colorBackgroundWorkday);
        painter->drawRect(0, 0, w, h);
        painter->setPen(colorTextWorkday);
    } else if (m_markedWeekdays.state(day) == KPTMap::NonWorking) {
        painter->setPen(colorBackgroundHoliday);
        painter->setBrush(colorBackgroundHoliday);
        painter->drawRect(0, 0, w, h);
        painter->setPen(colorTextHoliday);
    }
    if (m_selectedWeekdays.contains(day)) {
        painter->setPen(backgroundSelectColor);
        painter->setBrush(backgroundSelectColor);
        painter->drawRect(2, 2, w-4, h-4);
        painter->setPen(penSelectColor);
    }
    painter->drawText(0, 0, w, h-1, AlignCenter, KGlobal::locale()->calendar()->weekDayName(day, true), -1, &rect);
    painter->setPen(colorLine);
    painter->moveTo(0, h-1);
    painter->lineTo(w-1, h-1);

    if(rect.width()>maxCell.width()) maxCell.setWidth(rect.width());
    if(rect.height()>maxCell.height()) maxCell.setHeight(rect.height());

    //kdDebug()<<k_funcinfo<<"headline: row,col=("<<row<<","<<col<<")"<<" day="<<daystr<<endl;
}

void KPTDateTable::paintWeekNumber(QPainter *painter, int row) {
    QRect rect;
    int w=cellWidth();
    int h=cellHeight();

    QFont font=KGlobalSettings::generalFont();
    font.setBold(true);
    if (!m_enabled)
        font.setItalic(true);
    painter->setFont(font);

    painter->setBrush(KGlobalSettings::baseColor());
    painter->setPen(KGlobalSettings::baseColor());
    painter->drawRect(0, 0, w, h);
    painter->setPen(KGlobalSettings::textColor());
    if (m_markedWeeks.state(m_weeks[row]) == KPTMap::NonWorking) {
        painter->setPen(colorBackgroundHoliday);
        painter->setBrush(colorBackgroundHoliday);
        painter->drawRect(0, 0, w, h);
        painter->setPen(colorTextHoliday);
    } else if (m_markedWeeks.state(m_weeks[row]) == KPTMap::Working) {
        painter->setPen(colorBackgroundWorkday);
        painter->setBrush(colorBackgroundWorkday);
        painter->drawRect(0, 0, w, h);
        painter->setPen(colorTextWorkday);
    }
    if (m_selectedWeeks.contains(m_weeks[row])) {
        painter->setPen(backgroundSelectColor);
        painter->setBrush(backgroundSelectColor);
        painter->drawRect(2, 2, w-4, h-4);
        painter->setPen(penSelectColor);
    }
    painter->drawText(0, 0, w, h-1, AlignCenter, QString("%1").arg(m_weeks[row].first), -1, &rect);
    painter->setPen(colorLine);
    painter->moveTo(w-1, 0);
    painter->lineTo(w-1, h-1);
    //kdDebug()<<k_funcinfo<<"weeknumbers: row,col=("<<row<<","<<col<<") week="<<m_weeks[row].first<<endl;

    if(rect.width()>maxCell.width()) maxCell.setWidth(rect.width());
    if(rect.height()>maxCell.height()) maxCell.setHeight(rect.height());
}

void KPTDateTable::paintDay(QPainter *painter, int row, int col) {
    //kdDebug()<<k_funcinfo<<"row,col=("<<row<<","<<col<<")"<<" num col="<<numCols()<<endl;
    QRect rect;
    int w=cellWidth();
    int h=cellHeight();

    QFont font=KGlobalSettings::generalFont();
    font.setPointSize(fontsize);
    if (!m_enabled)
        font.setItalic(true);
    painter->setFont(font);

    QDate d = getDate(position(row, col));

    painter->setBrush(KGlobalSettings::baseColor());
    painter->setPen(KGlobalSettings::baseColor());
    painter->drawRect(0, 0, w, h);

    // First paint the dates background
    if (m_markedDates.state(d) == KPTMap::NonWorking) {
        //kdDebug()<<k_funcinfo<<"Marked date: "<<d<<"  row,col=("<<row<<","<<col<<")=NonWorking"<<endl;
        painter->setPen(colorBackgroundHoliday);
        painter->setBrush(colorBackgroundHoliday);
        painter->drawRect(0, 0, w, h);
    } else if (m_markedDates.state(d) == KPTMap::Working) {
        //kdDebug()<<k_funcinfo<<"Marked date: "<<d<<"  row,col=("<<row<<","<<col<<")=Working"<<endl;
        painter->setPen(colorBackgroundWorkday);
        painter->setBrush(colorBackgroundWorkday);
        painter->drawRect(0, 0, w, h);
    }
    if(m_selectedDates.contains(d)) {
        //kdDebug()<<k_funcinfo<<"Selected: "<<d<<" row,col=("<<row<<","<<col<<")"<<endl;
        painter->setPen(backgroundSelectColor);
        painter->setBrush(backgroundSelectColor);
        painter->drawRect(2, 2, w-4, h-4);
    }
    // If weeks or weekdays are selected/marked we draw lines around the date
    QPen pen = painter->pen();
    if (m_markedWeeks.state(m_weeks[row]) == KPTMap::NonWorking) {
        //kdDebug()<<k_funcinfo<<"Marked week: row,dayCol=("<<row<<","<<dayCol<<")=NonWorking"<<endl;
        painter->setPen(colorBackgroundHoliday);
        painter->moveTo(0, 0);
        painter->lineTo(w-1, 0);
        painter->moveTo(0, h-1);
        painter->lineTo(w-1, h-1);
        pen.setStyle(DotLine);
    } else if (m_markedWeeks.state(m_weeks[row]) == KPTMap::Working) {
        //kdDebug()<<k_funcinfo<<"Marked week: row,dayCol=("<<row<<","<<dayCol<<")=Working"<<endl;
        painter->setPen(colorBackgroundWorkday);
        painter->moveTo(0, 0);
        painter->lineTo(w-1, 0);
        painter->moveTo(0, h-1);
        painter->lineTo(w-1, h-1);
        pen.setStyle(DotLine);
    }
    if (m_markedWeekdays.state(weekday(col)) == KPTMap::NonWorking) {
        //kdDebug()<<k_funcinfo<<"Marked week: row,dayCol=("<<row<<","<<dayCol<<")=NonWorking"<<endl;
        pen.setColor(colorBackgroundHoliday);
        painter->setPen(pen);
        painter->moveTo(0, 0);
        painter->lineTo(0, h-1);
        painter->moveTo(w-1, 0);
        painter->lineTo(w-1, h-1);
    } else if (m_markedWeekdays.state(weekday(col)) == KPTMap::Working) {
        //kdDebug()<<k_funcinfo<<"Marked weekday: row,dayCol=("<<row<<","<<dayCol<<")=Working"<<endl;
        pen.setColor(colorBackgroundWorkday);
        painter->setPen(pen);
        painter->moveTo(0, 0);
        painter->lineTo(0, h-1);
        painter->moveTo(w-1, 0);
        painter->lineTo(w-1, h-1);
    }
    // then paint square if current date
    if (d  == QDate::currentDate()) {
        painter->setPen(colorLine);
        painter->drawRect(1, 1, w-2, h-2);
    }

    // and now the day number
    d.month() == date.month() ? painter->setPen(KGlobalSettings::textColor()) : painter->setPen(gray);
    painter->drawText(0, 0, w, h, AlignCenter, QString().setNum(d.day()), -1, &rect);

    if(rect.width()>maxCell.width()) maxCell.setWidth(rect.width());
    if(rect.height()>maxCell.height()) maxCell.setHeight(rect.height());
}

void KPTDateTable::paintCell(QPainter *painter, int row, int col) {
    //kdDebug()<<k_funcinfo<<"row,col=("<<row<<","<<col<<")"<<"enabled="<<m_enabled<<endl;
    if (row == 0 && col == 0) {
        painter->save();
        int w=cellWidth();
        int h=cellHeight();
        painter->setPen(colorLine);
        painter->setBrush(KGlobalSettings::baseColor());
        painter->moveTo(w-1, 0);
        painter->lineTo(w-1, h-1);
        painter->lineTo(0, h-1);
        painter->restore();
        return;
    }
    painter->save();
    if(row==0) { // we are drawing the weekdays
        paintWeekday(painter, col);
    } else if (col == 0) { // draw week numbers
        paintWeekNumber(painter, row);
    } else { // draw the day
        paintDay(painter, row, col);
    }
    painter->restore();
}

//FIXME
void KPTDateTable::keyPressEvent( QKeyEvent *e ) {
    if (!m_enabled)
        return;
    if ( e->key() == Qt::Key_Prior ) {
        setDate(date.addMonths(-1));
        return;
    }
    if ( e->key() == Qt::Key_Next ) {
        setDate(date.addMonths(1));
        return;
    }

    if ( e->key() == Qt::Key_Up ) {
        if ( date.day() > 7 ) {
            setDate(date.addDays(-7));
            return;
        }
    }
    if ( e->key() == Qt::Key_Down ) {
        if ( date.day() <= date.daysInMonth()-7 ) {
            setDate(date.addDays(7));
            return;
        }
    }
    if ( e->key() == Qt::Key_Left ) {
        if ( date.day() > 1 ) {
            setDate(date.addDays(-1));
            return;
        }
    }
    if ( e->key() == Qt::Key_Right ) {
        if ( date.day() < date.daysInMonth() ) {
            setDate(date.addDays(1));
            return;
        }
    }

    if ( e->key() == Qt::Key_Minus ) {
        setDate(date.addDays(-1));
        return;
    }
    if ( e->key() == Qt::Key_Plus ) {
        setDate(date.addDays(1));
        return;
    }
    if ( e->key() == Qt::Key_N ) {
        setDate(QDate::currentDate());
        return;
    }
    if ( e->key() == Qt::Key_Control ) {
        return;
    }
    if ( e->key() == Qt::Key_Shift ) {
        return;
    }

    KNotifyClient::beep();
}

void KPTDateTable::viewportResizeEvent(QResizeEvent * e) {
  QGridView::viewportResizeEvent(e);

  setCellWidth(viewport()->width()/numCols());
  setCellHeight(viewport()->height()/numRows());
}

void KPTDateTable::setFontSize(int size) {
  int count;
  QFontMetrics metrics(fontMetrics());
  QRect rect;
  // ----- store rectangles:
  fontsize=size;
  // ----- find largest day name:
  maxCell.setWidth(0);
  maxCell.setHeight(0);
  for(count=0; count<7; ++count)
    {
      rect=metrics.boundingRect(KGlobal::locale()->calendar()->weekDayName(count+1, true));
      maxCell.setWidth(QMAX(maxCell.width(), rect.width()));
      maxCell.setHeight(QMAX(maxCell.height(), rect.height()));
    }
  // ----- compare with a real wide number and add some space:
  rect=metrics.boundingRect(QString::fromLatin1("88"));
  maxCell.setWidth(QMAX(maxCell.width()+2, rect.width()));
  maxCell.setHeight(QMAX(maxCell.height()+4, rect.height()));
}

//FIXME
void KPTDateTable::wheelEvent ( QWheelEvent * e ) {
    setDate(date.addMonths( -(int)(e->delta()/120)) );
    e->accept();
}


void KPTDateTable::contentsMousePressEvent(QMouseEvent *e) {
    if (!m_enabled)
        return;
    //kdDebug()<<k_funcinfo<<endl;
    if(e->type()!=QEvent::MouseButtonPress) {
        return;
    }
    QPoint mouseCoord = e->pos();
    int row=rowAt(mouseCoord.y());
    int col=columnAt(mouseCoord.x());
    if (row == 0 && col == 0) { // user clicked on (unused) upper left square
        updateSelectedCells();
        m_selectedWeeks.clear();
        m_selectedWeekdays.clear();
        m_selectedDates.clear();
        repaintContents(false);
        emit selectionCleared();
        return;
    }
    if (col == 0) { // user clicked on week numbers
        updateSelectedCells();
        m_selectedWeekdays.clear();
        m_selectedDates.clear();
        if (e->state() & ShiftButton) {
            //TODO
        } else if (e->state() & ControlButton) {
            // toggle select this week
            m_selectedWeeks.toggle(m_weeks[row]);
            //kdDebug()<<k_funcinfo<<"toggle row: "<<row<<" state="<<m_selectedWeeks.state(m_weeks[row])<<endl;
        } else {
            // toggle select this, clear all others
            m_selectedWeeks.toggleClear(m_weeks[row]);
            //kdDebug()<<k_funcinfo<<"toggleClear row: "<<row<<" state="<<m_selectedWeeks.state(m_weeks[row])<<endl;
        }
        updateSelectedCells();
        repaintContents(false);
        if (m_enabled)
            emit weekSelected(m_weeks[row].first, m_weeks[row].second);
        return;
    }
    if (row==0 && col>=0) { // the user clicked on weekdays
        updateSelectedCells();
        m_selectedDates.clear();
        m_selectedWeeks.clear();;
        int day = weekday(col);
        if (e->state() & ShiftButton) {
            //TODO
        } else if (e->state() & ControlButton) {
            // toggle select this date
            m_selectedWeekdays.toggle(day);
        } else {
            // toggle select this, clear all others
            m_selectedWeekdays.toggleClear(day);
        }
        updateSelectedCells();
        repaintContents(false);
        if (m_enabled) {
            //kdDebug()<<k_funcinfo<<"emit weekdaySelected("<<day<<")"<<endl;
            emit weekdaySelected(day); // day= 1..7
        }
        return;
    }

    if (contentsMousePressEvent_internal(e)) {
        // Date hit,
        m_selectedWeekdays.clear();
        m_selectedWeeks.clear();
        if (e->state() & ShiftButton) {
            //TODO
        } else if (e->state() & ControlButton) {
            // toggle select this date
            m_selectedDates.toggle(date);
            //kdDebug()<<k_funcinfo<<"toggle date: "<<date.toString()<<" state="<<m_selectedDates.state(date)<<endl;
        } else {
            // toggle select this, clear all others
            m_selectedDates.toggleClear(date);
            //kdDebug()<<k_funcinfo<<"toggleClear date: "<<date.toString()<<" state="<<m_selectedDates.state(date)<<endl;
        }
    }
    repaintContents(false);
}

bool KPTDateTable::contentsMousePressEvent_internal(QMouseEvent *e) {
    QPoint mouseCoord = e->pos();
    int row=rowAt(mouseCoord.y());
    int col=columnAt(mouseCoord.x());
    if(row<1 || col<0) { // the user clicked on the frame of the table
        return false;
    }
    //kdDebug()<<k_funcinfo<<"pos["<<row<<","<<col<<"]="<<position(row,col)<<" firstday="<<firstday<<endl;
    selectDate(getDate(position(row, col)));
    return true;
}

bool KPTDateTable::selectDate(const QDate& date_) {
    //kdDebug()<<k_funcinfo<<"date="<<date_.toString()<<endl;
    bool changed=false;
    QDate temp;
    // -----
    if(!date_.isValid()) {
        return false;
    }
    if(date!=date_) {
        date=date_;
        changed=true;
    }
    m_selectedDates.clear();

    temp.setYMD(date.year(), date.month(), 1);
    firstday=temp.dayOfWeek();
    if(firstday==1) firstday=8; // Reserve row 1 for previous month
    numdays=date.daysInMonth();
    if(date.month()==1) { // set to december of previous year
        temp.setYMD(date.year()-1, 12, 1);
        setWeekNumbers(QDate(date.year()-1, 12, 31));
    } else { // set to previous month
        temp.setYMD(date.year(), date.month()-1, 1);
        QDate d(date.year(), date.month()-1,1);
        setWeekNumbers(d.addDays(d.daysInMonth()-1));
    }
    numDaysPrevMonth=temp.daysInMonth();
    if(changed) {
        repaintContents(false);
    }
    if (m_enabled)
        emit(dateChanged(date));
    return true;
}

bool KPTDateTable::setDate(const QDate& date_, bool repaint) {
    //kdDebug()<<k_funcinfo<<"date="<<date_.toString()<<endl;
    bool changed=false;
    QDate temp;
    // -----
    if(!date_.isValid()) {
        //kdDebug() << "KPTDateTable::setDate: refusing to set invalid date." << endl;
        return false;
        }
    if(date!=date_) {
        date=date_;
        changed=true;
    }
    m_selectedDates.clear();

    temp.setYMD(date.year(), date.month(), 1);
    firstday=temp.dayOfWeek();
    if(firstday==1) firstday=8;
    //kdDebug()<<k_funcinfo<<"date="<<temp.toString()<<"day="<<temp.dayOfWeek()<<" firstday="<<firstday<<endl;
    numdays=date.daysInMonth();
    if(date.month()==1) { // set to december of previous year
        temp.setYMD(date.year()-1, 12, 1);
        setWeekNumbers(QDate(date.year()-1, 12, 31));
    } else { // set to previous month
        temp.setYMD(date.year(), date.month()-1, 1);
        QDate d(date.year(), date.month()-1,1);
        setWeekNumbers(d.addDays(d.daysInMonth()-1));
    }
    if (m_selectedWeeks.isEmpty() && m_selectedWeekdays.isEmpty() &&
        !m_selectedDates.isEmpty() && !m_selectedDates.contains(date))
    {
        //kdDebug()<<k_funcinfo<<"date inserted"<<endl;
        m_selectedDates.insert(date);
    }
    numDaysPrevMonth=temp.daysInMonth();
    if(changed && repaint) {
        repaintContents(false);
    }
    if (m_enabled)
      emit(dateChanged(date));
    return true;
}

const QDate& KPTDateTable::getDate() const {
  return date;
}

void KPTDateTable::focusInEvent( QFocusEvent *e ) {
    QGridView::focusInEvent( e );
}

void KPTDateTable::focusOutEvent( QFocusEvent *e ) {
    QGridView::focusOutEvent( e );
}

QSize KPTDateTable::sizeHint() const {
    if(maxCell.height()>0 && maxCell.width()>0) {
      return QSize(maxCell.width()*numCols()+2*frameWidth(),
             (maxCell.height()+2)*numRows()+2*frameWidth());
    } else {
      //kdDebug() << "KPTDateTable::sizeHint: obscure failure - " << endl;
      return QSize(-1, -1);
    }
}

void KPTDateTable::setWeekNumbers(QDate date) {
    if (!date.isValid()) {
        kdError()<<k_funcinfo<<"Invalid date"<<endl;
    }
    QDate d(date);
    for (int i = 1; i < 7; ++i) {
        m_weeks[i].first = d.weekNumber(&(m_weeks[i].second));
        //kdDebug()<<k_funcinfo<<"date="<<d.toString()<<" week=("<<m_weeks[i].first<<","<<m_weeks[i].second<<")"<<endl;
        d = d.addDays(7);
    }
}

void KPTDateTable::updateCells() {
    //kdDebug()<<k_funcinfo<<endl;
    for (int row=0; row < numRows(); ++row) {
        for (int col=0; col < numCols(); ++col) {
            updateCell(row, col);
        }
    }
}

void KPTDateTable::updateSelectedCells() {
    //kdDebug()<<k_funcinfo<<endl;
    QDate dt(date.year(), date.month(), 1);
    dt = dt.addDays(-firstday);
    for (int pos=0; pos < 42; ++pos) {
        if (m_selectedDates.contains(dt.addDays(pos)) ||
            m_selectedWeekdays.contains(pos%7+1) ||
            m_selectedWeeks.contains(m_weeks[pos/7+1]))
        {
            updateCell(pos/7+1, pos%7+1);
            //kdDebug()<<k_funcinfo<<" update cell ("<<pos/7+1<<","<<pos%7+1<<") date="<<dt.addDays(pos).toString()<<endl;
        }
    }
}

void KPTDateTable::updateMarkedCells() {
    QDate dt(date.year(), date.month(), 1);
    dt = dt.addDays(-firstday);
    for (int pos=0; pos < 42; ++pos) {
        if (m_markedDates.contains(dt.addDays(pos)) ||
            m_markedWeekdays.contains(pos%7+1) ||
            m_markedWeeks.contains(m_weeks[pos/7+1]))
        {
            updateCell(pos/7+1, pos%7+1);
            //kdDebug()<<k_funcinfo<<" update cell ("<<pos/7+1<<","<<pos%7+1<<") date="<<dt.addDays(pos).toString()<<endl;
        }
    }
}

void KPTDateTable::setMarkedWeeks(KPTWeekMap weeks) {
    updateMarkedCells();
    m_markedWeeks.clear();
    KPTWeekMap::iterator it;
    for (it = weeks.begin(); it != weeks.end(); ++it) {
        //kdDebug()<<k_funcinfo<<"insert: "<<it.key()<<"="<<it.data()<<endl;
        m_markedWeeks.insert(it, it.data());
    }
    updateMarkedCells();
    repaintContents(false);
}

void KPTDateTable::addMarkedWeek(int week, int year, int state) {
    m_markedWeeks.insert(week, year, state);
    updateMarkedCells();
    repaintContents(false);
}

void KPTDateTable::setMarkedWeekdays(const KPTIntMap days) {
    updateMarkedCells();
    m_markedWeekdays.clear();
    m_markedWeekdays = days;
    updateMarkedCells();
    repaintContents(false);
}

bool KPTDateTable::weekdayMarked(int day) {
    return m_markedWeekdays.contains(day);
}

bool KPTDateTable::dateMarked(QDate date) {
    return m_markedDates[date.toString()];
}

bool KPTDateTable::weekMarked(QPair<int, int> week) {
    return m_markedWeeks.contains(week);
}

void KPTDateTable::clear() {
    clearSelection();
    m_markedDates.clear();
    m_markedWeekdays.clear();
    m_markedWeeks.clear();
    repaintContents(false);
}

void KPTDateTable::clearSelection() {
    m_selectedDates.clear();
    m_selectedWeekdays.clear();
    m_selectedWeeks.clear();;
    repaintContents(false);
}

 void KPTDateTable::setEnabled(bool yes) {
    if (m_enabled == yes)
        return;
    m_enabled=yes;
    updateCells();
}

void KPTDateTable::markSelected(int state) {
    if (!m_selectedDates.isEmpty()) {
        KPTDateMap::iterator it;
        for(it = m_selectedDates.begin(); it != m_selectedDates.end(); ++it) {
            m_markedDates.insert(it.key(), state);
            //kdDebug()<<k_funcinfo<<"marked date: "<<it.key()<<"="<<state<<endl;
        }
    } else if (!m_selectedWeekdays.isEmpty()) {
        KPTIntMap::iterator it;
        for(it = m_selectedWeekdays.begin(); it != m_selectedWeekdays.end(); ++it) {
            m_markedWeekdays.insert(it.key(), state);
            //kdDebug()<<k_funcinfo<<"marked weekday: "<<it.key()<<"="<<state<<endl;
        }
    } else if (!m_selectedWeeks.isEmpty()) {
        KPTWeekMap::iterator it;
        for(it = m_selectedWeeks.begin(); it != m_selectedWeeks.end(); ++it) {
            m_markedWeeks.insert(KPTWeekMap::week(it.key()), state);
            //kdDebug()<<k_funcinfo<<"marked week: "<<it.key()<<"="<<state<<endl;
        }
    }
    updateSelectedCells();
    repaintContents(false);
}

KPTDateInternalWeekSelector::KPTDateInternalWeekSelector
(int fontsize, QWidget* parent, const char* name)
  : QLineEdit(parent, name),
    val(new QIntValidator(this)),
    result(0)
{
  QFont font;
  // -----
  font=KGlobalSettings::generalFont();
  font.setPointSize(fontsize);
  setFont(font);
  setFrameStyle(QFrame::NoFrame);
  val->setRange(1, 53);
  setValidator(val);
  connect(this, SIGNAL(returnPressed()), SLOT(weekEnteredSlot()));
}

void
KPTDateInternalWeekSelector::weekEnteredSlot()
{
  bool ok;
  int week;
  // ----- check if this is a valid week:
  week=text().toInt(&ok);
  if(!ok)
    {
      KNotifyClient::beep();
      return;
    }
  result=week;
  emit(closeMe(1));
}

int
KPTDateInternalWeekSelector::getWeek() const
{
  return result;
}

void
KPTDateInternalWeekSelector::setWeek(int week)
{
  QString temp;
  // -----
  temp.setNum(week);
  setText(temp);
}

KPTDateInternalMonthPicker::KPTDateInternalMonthPicker
(int fontsize, QWidget* parent, const char* name)
  : QGridView(parent, name),
    result(0) // invalid
{
  QRect rect;
  QFont font;
  // -----
  activeCol = -1;
  activeRow = -1;
  font=KGlobalSettings::generalFont();
  font.setPointSize(fontsize);
  setFont(font);
  setHScrollBarMode(AlwaysOff);
  setVScrollBarMode(AlwaysOff);
  setFrameStyle(QFrame::NoFrame);
  setNumRows(4);
  setNumCols(3);
  // enable to find drawing failures:
  // setTableFlags(Tbl_clipCellPainting);
  viewport()->setEraseColor(KGlobalSettings::baseColor()); // for consistency with the datepicker
  // ----- find the preferred size
  //       (this is slow, possibly, but unfortunatly it is needed here):
  QFontMetrics metrics(font);
  for(int i=1; i <= 12; ++i)
    {
      rect=metrics.boundingRect(KGlobal::locale()->calendar()->monthName(i, false));
      if(max.width()<rect.width()) max.setWidth(rect.width());
      if(max.height()<rect.height()) max.setHeight(rect.height());
    }

}

QSize
KPTDateInternalMonthPicker::sizeHint() const
{
  return QSize((max.width()+6)*numCols()+2*frameWidth(),
         (max.height()+6)*numRows()+2*frameWidth());
}

int
KPTDateInternalMonthPicker::getResult() const
{
  return result;
}

void
KPTDateInternalMonthPicker::setupPainter(QPainter *p)
{
  p->setPen(KGlobalSettings::textColor());
}

void
KPTDateInternalMonthPicker::viewportResizeEvent(QResizeEvent*)
{
  setCellWidth(width()/3);
  setCellHeight(height()/4);
}

void
KPTDateInternalMonthPicker::paintCell(QPainter* painter, int row, int col)
{
  int index;
  QString text;
  // ----- find the number of the cell:
  index=3*row+col+1;
  text=KGlobal::locale()->calendar()->monthName(index, false);
  painter->drawText(0, 0, cellWidth(), cellHeight(), AlignCenter, text);
  if ( activeCol == col && activeRow == row )
      painter->drawRect( 0, 0, cellWidth(), cellHeight() );
}

void
KPTDateInternalMonthPicker::contentsMousePressEvent(QMouseEvent *e)
{
  if(!isEnabled() || e->button() != LeftButton)
    {
      KNotifyClient::beep();
      return;
    }
  // -----
  int row, col;
  QPoint mouseCoord;
  // -----
  mouseCoord = e->pos();
  row=rowAt(mouseCoord.y());
  col=columnAt(mouseCoord.x());

  if(row<0 || col<0)
    { // the user clicked on the frame of the table
      activeCol = -1;
      activeRow = -1;
    } else {
      activeCol = col;
      activeRow = row;
      updateCell( row, col /*, false */ );
  }
}

void
KPTDateInternalMonthPicker::contentsMouseMoveEvent(QMouseEvent *e)
{
  if (e->state() & LeftButton)
    {
      int row, col;
      QPoint mouseCoord;
      // -----
      mouseCoord = e->pos();
      row=rowAt(mouseCoord.y());
      col=columnAt(mouseCoord.x());
      int tmpRow = -1, tmpCol = -1;
      if(row<0 || col<0)
        { // the user clicked on the frame of the table
          if ( activeCol > -1 )
            {
              tmpRow = activeRow;
              tmpCol = activeCol;
            }
          activeCol = -1;
          activeRow = -1;
        } else {
          bool differentCell = (activeRow != row || activeCol != col);
          if ( activeCol > -1 && differentCell)
            {
              tmpRow = activeRow;
              tmpCol = activeCol;
            }
          if ( differentCell)
            {
              activeRow = row;
              activeCol = col;
              updateCell( row, col /*, false */ ); // mark the new active cell
            }
        }
      if ( tmpRow > -1 ) // repaint the former active cell
          updateCell( tmpRow, tmpCol /*, true */ );
    }
}

void
KPTDateInternalMonthPicker::contentsMouseReleaseEvent(QMouseEvent *e)
{
  if(!isEnabled())
    {
      return;
    }
  // -----
  int row, col, pos;
  QPoint mouseCoord;
  // -----
  mouseCoord = e->pos();
  row=rowAt(mouseCoord.y());
  col=columnAt(mouseCoord.x());
  if(row<0 || col<0)
    { // the user clicked on the frame of the table
      emit(closeMe(0));
    }
  pos=3*row+col+1;
  result=pos;
  emit(closeMe(1));
}



KPTDateInternalYearSelector::KPTDateInternalYearSelector
(int fontsize, QWidget* parent, const char* name)
  : QLineEdit(parent, name),
    val(new QIntValidator(this)),
    result(0)
{
  QFont font;
  // -----
  font=KGlobalSettings::generalFont();
  font.setPointSize(fontsize);
  setFont(font);
  setFrameStyle(QFrame::NoFrame);
  // we have to respect the limits of QDate here, I fear:
  val->setRange(0, 8000);
  setValidator(val);
  connect(this, SIGNAL(returnPressed()), SLOT(yearEnteredSlot()));
}

void
KPTDateInternalYearSelector::yearEnteredSlot()
{
  bool ok;
  int year;
  QDate date;
  // ----- check if this is a valid year:
  year=text().toInt(&ok);
  if(!ok)
    {
      KNotifyClient::beep();
      return;
    }
  date.setYMD(year, 1, 1);
  if(!date.isValid())
    {
      KNotifyClient::beep();
      return;
    }
  result=year;
  emit(closeMe(1));
}

int
KPTDateInternalYearSelector::getYear() const
{
  return result;
}

void
KPTDateInternalYearSelector::setYear(int year)
{
  QString temp;
  // -----
  temp.setNum(year);
  setText(temp);
}

KPTPopupFrame::KPTPopupFrame(QWidget* parent, const char*  name)
  : QFrame(parent, name, WType_Popup),
    result(0), // rejected
    main(0)
{
  setFrameStyle(QFrame::Box|QFrame::Raised);
  setMidLineWidth(2);
}

void
KPTPopupFrame::keyPressEvent(QKeyEvent* e)
{
  if(e->key()==Key_Escape)
    {
      result=0; // rejected
      qApp->exit_loop();
    }
}

void
KPTPopupFrame::close(int r)
{
  result=r;
  qApp->exit_loop();
}

void
KPTPopupFrame::setMainWidget(QWidget* m)
{
  main=m;
  if(main!=0)
    {
      resize(main->width()+2*frameWidth(), main->height()+2*frameWidth());
    }
}

void
KPTPopupFrame::resizeEvent(QResizeEvent*)
{
  if(main!=0)
    {
      main->setGeometry(frameWidth(), frameWidth(),
          width()-2*frameWidth(), height()-2*frameWidth());
    }
}

void
KPTPopupFrame::popup(const QPoint &pos)
{
  // Make sure the whole popup is visible.
  QRect d = QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(pos));
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
KPTPopupFrame::exec(QPoint pos)
{
  popup(pos);
  repaint();
  qApp->enter_loop();
  hide();
  return result;
}

int
KPTPopupFrame::exec(int x, int y)
{
  return exec(QPoint(x, y));
}

void KPTPopupFrame::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void KPTDateTable::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

}  //KPlato namespace

#include "kptdatetable.moc"
