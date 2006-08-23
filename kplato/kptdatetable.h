/* This file is part of the KDE project
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2004-2006 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTDATETABEL_H
#define KPTDATETABEL_H

#include "kptmap.h"

#include <kglobal.h>
#include <klocale.h>

#include <qgridview.h>
#include <qmemarray.h>
#include <qdict.h>
#include <qpair.h>

#include <qvalidator.h>
#include <qlineedit.h>
#include <qdatetime.h>

namespace KPlato
{

/** Week selection widget.
* @internal
* @version $Id$
* @author Stephan Binner
*/
class DateInternalWeekSelector : public QLineEdit
{
  Q_OBJECT
protected:
  QIntValidator *val;
  int result;
public slots:
  void weekEnteredSlot();
signals:
  void closeMe(int);
public:
  DateInternalWeekSelector(int fontsize,
			    QWidget* parent=0,
			    const char* name=0);
  int getWeek() const;
  void setWeek(int week);

private:
  class DateInternalWeekPrivate;
  DateInternalWeekPrivate *d;
};

/**
* A table containing month names. It is used to pick a month directly.
* @internal
* @version $Id$
* @author Tim Gilman, Mirko Boehm
*/
class DateInternalMonthPicker : public QGridView
{
  Q_OBJECT
protected:
  /**
   * Store the month that has been clicked [1..12].
   */
  int result;
  /**
   * the cell under mouse cursor when LBM is pressed
   */
  short int activeCol;
  short int activeRow;
  /**
   * Contains the largest rectangle needed by the month names.
   */
  QRect max;
signals:
  /**
   * This is send from the mouse click event handler.
   */
  void closeMe(int);
public:
  /**
   * The constructor.
   */
  DateInternalMonthPicker(int fontsize, QWidget* parent, const char* name=0);
  /**
   * The size hint.
   */
  QSize sizeHint() const;
  /**
   * The minimum size hint.
   */
  QSize minimumSizeHint() const { return sizeHint(); }
  /**
   * Return the result. 0 means no selection (reject()), 1..12 are the
   * months.
   */
  int getResult() const;
protected:
  /**
   * Set up the painter.
   */
  void setupPainter(QPainter *p);
  /**
   * The resize event.
   */
  virtual void viewportResizeEvent(QResizeEvent*);
  /**
   * Paint a cell. This simply draws the month names in it.
   */
  virtual void paintCell(QPainter* painter, int row, int col);
  /**
   * Catch mouse click and move events to paint a rectangle around the item.
   */
  virtual void contentsMousePressEvent(QMouseEvent *e);
  virtual void contentsMouseMoveEvent(QMouseEvent *e);
  /**
   * Emit monthSelected(int) when a cell has been released.
   */
  virtual void contentsMouseReleaseEvent(QMouseEvent *e);

private:
  class DateInternalMonthPrivate;
  DateInternalMonthPrivate *d;
};

/** Year selection widget.
* @internal
* @version $Id$
* @author Tim Gilman, Mirko Boehm
*/
class DateInternalYearSelector : public QLineEdit
{
  Q_OBJECT
protected:
  QIntValidator *val;
  int result;
public slots:
  void yearEnteredSlot();
signals:
  void closeMe(int);
public:
  DateInternalYearSelector(int fontsize,
			    QWidget* parent=0,
			    const char* name=0);
  int getYear() const;
  void setYear(int year);

private:
  class DateInternalYearPrivate;
  DateInternalYearPrivate *d;
};

/**
 * Frame with popup menu behaviour.
 * @author Tim Gilman, Mirko Boehm
 * @version $Id$
 */
class PopupFrame : public QFrame
{
  Q_OBJECT
protected:
  /**
   * The result. It is returned from exec() when the popup window closes.
   */
  int result;
  /**
   * Catch key press events.
   */
  virtual void keyPressEvent(QKeyEvent* e);
  /**
   * The only subwidget that uses the whole dialog window.
   */
  QWidget *main;
public slots:
  /**
   * Close the popup window. This is called from the main widget, usually.
   * @p r is the result returned from exec().
   */
  void close(int r);
public:
  /**
   * The contructor. Creates a dialog without buttons.
   */
  PopupFrame(QWidget* parent=0, const char*  name=0);
  /**
   * Set the main widget. You cannot set the main widget from the constructor,
   * since it must be a child of the frame itselfes.
   * Be careful: the size is set to the main widgets size. It is up to you to
   * set the main widgets correct size before setting it as the main
   * widget.
   */
  void setMainWidget(QWidget* m);
  /**
   * The resize event. Simply resizes the main widget to the whole
   * widgets client size.
   */
  virtual void resizeEvent(QResizeEvent*);
  /**
   * Open the popup window at position pos.
   */
  void popup(const QPoint &pos);
  /**
   * Execute the popup window.
   */
  int exec(QPoint p);
  /**
   * Dito.
   */
  int exec(int x, int y);

private:

  virtual bool close(bool alsoDelete) { return QFrame::close(alsoDelete); }
protected:
  virtual void virtual_hook( int id, void* data );
private:
  class PopupFramePrivate;
  PopupFramePrivate *d;
};

/**
* Validates user-entered dates.
*/
class DateValidator : public QValidator
{
public:
    DateValidator(QWidget* parent=0, const char* name=0);
    virtual State validate(QString&, int&) const;
    virtual void fixup ( QString & input ) const;
    State date(const QString&, QDate&) const;
};


class DateTable : public QGridView
{
    Q_OBJECT
public:
    /**
     * The constructor.
     */
    DateTable(QWidget *parent=0, QDate date=QDate::currentDate(),
                 const char* name="DateTable", WFlags f=0);

    /**
     * Returns a recommended size for the widget.
     * To save some time, the size of the largest used cell content is
     * calculated in each paintCell() call, since all calculations have
     * to be done there anyway. The size is stored in maxCell. The
     * sizeHint() simply returns a multiple of maxCell.
     */
    virtual QSize sizeHint() const;
    /**
     * Set the font size of the date table.
     */
    void setFontSize(int size);
    /**
     * Select and display this date.
     */
    bool setDate(const QDate&, bool repaint=true);
    const QDate& getDate() const;
    bool selectDate(const QDate& date_);

    void addMarkedDate(QDate date, int state) { m_markedDates.insert(date, state); }
    bool dateMarked(QDate date);

    void addMarkedWeekday(int day, int state);
    void setMarkedWeekday(int day, int state) { m_markedWeekdays.insert(day, state); }
    void setMarkedWeekdays(const IntMap days);
    bool weekdayMarked(int day);

    DateMap selectedDates() const { return m_selectedDates; }
    IntMap selectedWeekdays() const { return m_selectedWeekdays; }

    DateMap markedDates() const { return m_markedDates; }
    IntMap markedWeekdays() const { return m_markedWeekdays; }

    void clear();
    void clearSelection();

    void setEnabled(bool yes);
    bool isEnabled() const { return m_enabled; }

    void markSelected(int state);

protected:
    /**
     * Paint a cell.
     */
    virtual void paintCell(QPainter*, int, int);
    /**
     * Handle the resize events.
     */
    virtual void viewportResizeEvent(QResizeEvent *);
    /**
     * React on mouse clicks that select a date.
     */
    virtual void contentsMousePressEvent(QMouseEvent *);
    virtual void wheelEvent( QWheelEvent * e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );

    bool contentsMousePressEvent_internal(QMouseEvent *);

    int weekOfYear(QDate date) const;
    void setWeekNumbers(QDate);

    bool weekSelected(int row);
    bool weekSelected();
    bool weekdaySelected();
    bool isWeekdaySelected(int day);
    bool dateSelected(QDate date);
    bool dateSelected();
    void updateSelectedCells();
    void updateMarkedCells();
    void updateCells();

    QDate getDate(int pos) const;
    
    /**
     *  pos can be 1..42
     * row starts at 1, col depends on wether weeks are presented (in col 0)
     */
    int position(int row, int col) { return ((7 * (row - 1)) + col - m_dateStartCol + 1); }

    int weekday(int col) const;
    int column(int weekday) const;
    
    void paintWeekday(QPainter *painter, int col);
    void paintWeekNumber(QPainter *painter, int row);
    void paintDay(QPainter *painter, int row, int col);

    /**
     * The font size of the displayed text.
     */
    int fontsize;
    /**
     * The currently selected date.
     */
    QDate date;
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
    QRect maxCell;

signals:
    /**
     * The selected date changed.
     */
    void dateChanged(QDate);
    /**
     * A date has been selected by clicking on the table.
     */
    void tableClicked();

    void weekdaySelected(int);
    void weekSelected(int, int);
    /**
     * All selections have been cleared
     */
    void selectionCleared();

private:

    QMemArray< QPair<int, int> > m_weeks;

    int m_currentRow; // row of selected date

    // User has selected these, results in "select coloring" the dates in datetable

    DateMap m_selectedDates;
    IntMap m_selectedWeekdays;

    // These results in marking the dates, weekdays and weeks respectivly
    DateMap m_markedDates;
    IntMap m_markedWeekdays;

    int m_dateStartCol;
    bool m_enabled;

    QColor colorBackgroundHoliday;
    QColor colorBackgroundWorkday;
    QColor colorTextHoliday;
    QColor colorTextWorkday;
    QColor colorLine;
    QColor backgroundSelectColor;
    QColor penSelectColor;

protected:
  virtual void virtual_hook( int id, void* data );
private:
    class DateTablePrivate;
    DateTablePrivate *d;
};

}  //KPlato namespace

#endif // DATETABEL_H
