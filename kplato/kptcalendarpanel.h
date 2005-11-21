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

#ifndef KPTCALENDARPANEL_H
#define KPTCALENDARPANEL_H

#include "kptmap.h"

#include <qdatetime.h>
#include <qframe.h>

class QLineEdit;
class QToolButton;

namespace KPlato
{

class DateValidator;
class DateTable;
class Calendar;

/**
 * Provides a widget for calendar input.
 *
 **/
class CalendarPanel: public QFrame
{
    Q_OBJECT
    Q_PROPERTY( QDate date READ date WRITE setDate)
    Q_PROPERTY( bool closeButton READ hasCloseButton WRITE setCloseButton )
    
public:
    /** The usual constructor.  The given date will be displayed
    * initially.
    **/
    CalendarPanel(QWidget *parent=0, QDate=QDate::currentDate(), const char *name=0, WFlags f=0);
    
    /**
    * Standard qt widget constructor. The initial date will be the
    * current date.
    */
    CalendarPanel( QWidget *parent, const char *name );
    
    /**
    * The destructor.
    **/
    virtual ~CalendarPanel();
    
    /** The size hint for date pickers. The size hint recommends the
    *   minimum size of the widget so that all elements may be placed
    *  without clipping. This sometimes looks ugly, so when using the
    *  size hint, try adding 28 to each of the reported numbers of
    *  pixels.
    **/
    QSize sizeHint() const;
    
    QSize minimumSizeHint() const { return sizeHint(); }
    /**
    * Sets the date.
    *
    *  @returns @p false and does not change anything
    *      if the date given is invalid.
    **/
    bool setDate(const QDate&);
    
    /**
    * Returns the selected date.
    * @deprecated
    **/
    const QDate& getDate() const;
    
    /**
    * @returns the selected date.
    */
    const QDate &date() const;
    
    /**
    * Enables or disables the widget.
    **/
    void setEnabled(bool);
    
    /**
    * Sets the font size of the widgets elements.
    **/
    void setFontSize(int);
    /**
    * Returns the font size of the widget elements.
    */
    int fontSize() const
        { return fontsize; }
    
    /**
    * By calling this method with @p enable = true, CalendarPanel will show
    * a little close-button in the upper button-row. Clicking the
    * close-button will cause the CalendarPanel's topLevelWidget()'s close()
    * method being called. This is mostly useful for toplevel datepickers
    * without a window manager decoration.
    * @see #hasCloseButton
    */
    void setCloseButton( bool enable );
    
    /**
    * @returns true if a CalendarPanel shows a close-button.
    * @see #setCloseButton
    */
    bool hasCloseButton() const;
    
    void setCalendar(Calendar *cal);
    
    DateMap selectedDates();
    IntMap selectedWeekdays();
    WeekMap selectedWeeks();
    
    DateMap markedDates();
    IntMap markedWeekdays();
    WeekMap markedWeeks();
    
    void clear();
    
    void markSelected(int state);
    
protected:
    /// to catch move keyEvents when QLineEdit has keyFocus
    virtual bool eventFilter(QObject *o, QEvent *e );
    /// the resize event
    virtual void resizeEvent(QResizeEvent*);
    /// the year forward button
    QToolButton *yearForward;
    /// the year backward button
    QToolButton *yearBackward;
    /// the month forward button
    QToolButton *monthForward;
    /// the month backward button
    QToolButton *monthBackward;
    /// the button for selecting the month directly
    QToolButton *selectMonth;
    /// the button for selecting the year directly
    QToolButton *selectYear;
    /// the line edit to enter the date directly
    QLineEdit *line;
    /// the validator for the line edit:
    DateValidator *val;
    /// the date table
    DateTable *table;
    // the widest month string in pixels:
    QSize maxMonthRect;

protected slots:
    void dateChangedSlot(QDate);
    void tableClickedSlot();
    void monthForwardClicked();
    void monthBackwardClicked();
    void yearForwardClicked();
    void yearBackwardClicked();
    void selectWeekClicked();
    void selectMonthClicked();
    void selectYearClicked();
    void lineEnterPressed();
    
    void slotWeekdaySelected(int day);
    void slotWeekSelected(int week, int year);
    void slotSelectionCleared();
  
signals:
    /** This signal is emitted each time the selected date is changed.
     *  Usually, this does not mean that the date has been entered,
     *  since the date also changes, for example, when another month is
     *  selected.
     *  @see dateSelected
     */
    void dateChanged(QDate);
    /** This signal is emitted each time a day has been selected by
     *  clicking on the table (hitting a day in the current month).
     */
    void dateSelected(QDate);
    /** This signal is emitted when enter is pressed and a VALID date
     *  has been entered before into the line edit. Connect to both
     *  dateEntered() and dateSelected() to receive all events where the
     *  user really enters a date.
     */
    void dateEntered(QDate);
    /** This signal is emitted when the day has been selected by
     *  clicking on it in the table.
     */
    void tableClicked();
    
    void weekSelected(int week, int year);
    void weekdaySelected(int day);
    /**
     * All selections have been cleared
     */
    void selectionCleared();
  
private:
    /// the font size for the widget
    int fontsize;
    
    bool m_selectedDays[7];
    
protected:
    virtual void virtual_hook( int id, void* data );
private:
    void init( const QDate &dt );
    class CalendarPanelPrivate;
    CalendarPanelPrivate *d;
    // calculate ISO 8601 week number
    int weekOfYear(QDate);
};

}  //KPlato namespace

#endif //  CALENDARPANEL_H
