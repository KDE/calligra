 /***************************************************************************
 *   Copyright (C) 2005 by Raphael Langerhorst                             *
 *   raphael-langerhorst@gmx.at                                            *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/

#include <kspread_insertcalendardialog.h>

#include <kdatepicker.h>
#include <kdatewidget.h>
#include <kdebug.h>

#include <qpushbutton.h>

namespace KSpread
{

InsertCalendarDialog::InsertCalendarDialog(QWidget* parent, const char* name)
: InsertCalendarDialogBase(parent,name)
{
  this->m_datePicker = 0;

  //we start with a default calendar for the current month;

  QDate first_day_in_month = QDate::currentDate();
  first_day_in_month.setYMD(first_day_in_month.year(),first_day_in_month.month(),1);

  QDate last_day_in_month(first_day_in_month.year(),first_day_in_month.month(),first_day_in_month.daysInMonth());

  this->m_startDateWidget->setDate(first_day_in_month);
  this->m_endDateWidget->setDate(last_day_in_month);

  connect(this->m_selectStartDateButton,SIGNAL(clicked()),this,SLOT(showStartDatePicker()));
  connect(this->m_selectEndDateButton,SIGNAL(clicked()),this,SLOT(showEndDatePicker()));

  connect(this->m_insertButton,SIGNAL(clicked()),this,SLOT(accept()));
  connect(this->m_cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}

InsertCalendarDialog::~InsertCalendarDialog()
{
}

bool InsertCalendarDialog::buildDatePickerFrame()
{
  if (m_datePicker)
  {
    delete m_datePicker; //destroyed signal is connected to datePickerDeleted()
  }

  m_datePicker = new KDatePicker();

  Q_ASSERT(m_datePicker);

  if (!m_datePicker)
    return false;

  connect(m_datePicker,SIGNAL(destroyed()),this,SLOT(datePickerDeleted()));

  m_datePicker->setCloseButton(true);
  m_datePicker->move(this->x()+this->width(),this->y());
  m_datePicker->show();

  return true;
}

void InsertCalendarDialog::datePickerDeleted()
{
  kDebug() << "date picker deleted" << endl;
  m_datePicker = 0;
}

void InsertCalendarDialog::accept()
{
  if (m_datePicker)
    m_datePicker->deleteLater();
  kDebug() << "insert calendar dialog accepted (insert button clicked)" << endl;
  done(QDialog::Accepted);
  emit insertCalendar(startDate(),endDate());
}

void InsertCalendarDialog::reject()
{
  if (m_datePicker)
    m_datePicker->deleteLater();
  kDebug() << "insert calendar dialog rejected (cancel button clicked)" << endl;
  done(QDialog::Rejected);
}

void InsertCalendarDialog::showStartDatePicker()
{
  if (buildDatePickerFrame())
  {
    connect(m_datePicker,SIGNAL(dateSelected(QDate)),this,SLOT(setStartDate(QDate)));
    connect(m_datePicker,SIGNAL(dateEntered(QDate)),this,SLOT(setStartDate(QDate)));
    m_datePicker->setDate(startDate());
  }
}

void InsertCalendarDialog::showEndDatePicker()
{
  if (buildDatePickerFrame())
  {
    connect(m_datePicker,SIGNAL(dateSelected(QDate)),this,SLOT(setEndDate(QDate)));
    connect(m_datePicker,SIGNAL(dateEntered(QDate)),this,SLOT(setEndDate(QDate)));
    m_datePicker->setDate(endDate());
  }
}

void InsertCalendarDialog::setStartDate(QDate date)
{
  this->m_startDateWidget->setDate(date);
}

void InsertCalendarDialog::setEndDate(QDate date)
{
  this->m_endDateWidget->setDate(date);
}

QDate InsertCalendarDialog::startDate() const
{
  return this->m_startDateWidget->date();
}

QDate InsertCalendarDialog::endDate() const
{
  return this->m_endDateWidget->date();
}

}

#include "kspread_insertcalendardialog.moc"
