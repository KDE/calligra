/***************************************************************************
*   Copyright (C) 2005 by Raphael Langerhorst <raphael-langerhorst@gmx.at>*
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

#ifndef KSPREAD_CALENDAR_TOOL_WIDGET
#define KSPREAD_CALENDAR_TOOL_WIDGET

#include <ui_CalendarToolWidget.h>


class KDatePicker;

namespace KSpread
{

/**
* The CalendarToolWidget class implements a dialog window that allows to
* define how details about the calendar that should be inserted.
*/
class CalendarToolWidget : public QDialog, public Ui::CalendarToolWidget
{
    Q_OBJECT

private:

    /**
     * Stores a pointer to the currently active date picker frame.
     * This is useful if we want to delete it manually.
     * @see datePickerDeleted
     */
    KDatePicker* m_datePicker;

public:

    /**
     * Constructor, initializes functionality.
     * The dates in the dialog initialize to a complete calendar for the
     * current month.
     */
    CalendarToolWidget(QWidget* parent = 0);

    /**
     * Virtual destructor.
     */
    virtual ~CalendarToolWidget();

protected:

    /**
     * Creates a KDatePicker frame that is not connected
     * to any date widget. Normally showStartDatePicker
     * of showEndDatePicker use this method to build
     * a generally usable date picker widget and connect
     * it to the appropriate KDateWidget of the dialog.
     *
     * DatePickerDeleted is already connected!
     */
    bool buildDatePickerFrame();

protected slots:

    /**
     * Sets the currently active KDatePicker frame to 0.
     * It's expected that the date picker frame is deleted.
     * @see m_datePicker
     */
    void datePickerDeleted();

    /**
     * Emits the insertCalendar() signal with the chosen start and end date.
     */
    void emitInsertCalendar();

public slots:

    /**
     * Shows a KDatePicker widget to select the start date.
     * This slot is activated by the select date button.
     */
    void showStartDatePicker();

    /**
     * Shows a KDatePicker widget to select the end date.
     * This slot is activated by the select date button.
     */
    void showEndDatePicker();

    /**
     * Slot for setting the start date in the KDateWidget.
     * Normally this slot is activated by a KDatePicker widget.
     */
    void setStartDate(const QDate&);

    /**
     * Slot for setting the end date in the KDateWidget.
     * Normally this slot is activated by a KDatePicker widget.
     */
    void setEndDate(const QDate&);

public:

    // METHODS for getting data //

    /**
     * @returns the selected start date.
     */
    QDate startDate() const;

    /**
     * @returns the selected end date.
     */
    QDate endDate() const;

signals:

    /**
     * This signal is emitted when the dialog is
     * accepted - that is, when the Insert button
     * is clicked.
     * This signal is meant to tell the
     * insert calendar plugin to actually insert
     * the calendar with the selected dates.
     */
    void insertCalendar(const QDate &from, const QDate &to);
};

}

#endif // KSPREAD_CALENDAR_TOOL_WIDGET
