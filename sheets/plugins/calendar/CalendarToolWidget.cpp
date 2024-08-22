/***************************************************************************
 *   SPDX-FileCopyrightText: 2005 Raphael Langerhorst <raphael-langerhorst@gmx.at>*
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

#include "CalendarToolWidget.h"

// #include "SheetsDebug.h"

// #include <KoIcon.h>

#include <QDateEdit>
// #include <QPushButton>
// #include <QDate>

namespace Calligra
{
namespace Sheets
{

CalendarToolWidget::CalendarToolWidget(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    // we start with a default calendar for the current month;

    QDate first_day_in_month = QDate::currentDate();
    first_day_in_month.setDate(first_day_in_month.year(), first_day_in_month.month(), 1);

    QDate last_day_in_month(first_day_in_month.year(), first_day_in_month.month(), first_day_in_month.daysInMonth());

    this->m_startDateWidget->setDate(first_day_in_month);
    this->m_endDateWidget->setDate(last_day_in_month);

    connect(this->m_insertButton, &QAbstractButton::clicked, this, &CalendarToolWidget::emitInsertCalendar);
}

CalendarToolWidget::~CalendarToolWidget() = default;

void CalendarToolWidget::emitInsertCalendar()
{
    Q_EMIT insertCalendar(startDate(), endDate());
}

void CalendarToolWidget::setStartDate(const QDate &date)
{
    this->m_startDateWidget->setDate(date);
}

void CalendarToolWidget::setEndDate(const QDate &date)
{
    this->m_endDateWidget->setDate(date);
}

QDate CalendarToolWidget::startDate() const
{
    return this->m_startDateWidget->date();
}

QDate CalendarToolWidget::endDate() const
{
    return this->m_endDateWidget->date();
}

} // namespace Sheets
} // namespace Calligra
