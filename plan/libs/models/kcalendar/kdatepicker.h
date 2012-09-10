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
#ifndef KPTDATEPICKER_H
#define KPTDATEPICKER_H

#include "../kplatomodels_export.h"

#include "kdatetable.h"

#include <QDateTime>
#include <QFrame>

class QLineEdit;
class QToolButton;

namespace KPlato
{

class KDateValidator;
class KDateTable;

/**
 * @short A date selection widget.
 *
 * Provides a widget for calendar date input.
 *
 *     Different from the
 *     previous versions, it now emits two types of signals, either
 * dateSelected() or dateEntered() (see documentation for both
 *     signals).
 *
 *     A line edit has been added in the newer versions to allow the user
 *     to select a date directly by entering numbers like 19990101
 *     or 990101.
 *
 * \image html kdatepicker.png "KDE Date Widget"
 *
 *     @author Tim Gilman, Mirko Boehm
 *
 **/
class KPLATOMODELS_EXPORT KDatePicker: public QFrame
{
  Q_OBJECT
  Q_PROPERTY( QDate date READ date WRITE setDate USER true )
  Q_PROPERTY( bool closeButton READ hasCloseButton WRITE setCloseButton )
  Q_PROPERTY( int fontSize READ fontSize WRITE setFontSize )

public:
  /**
   * The constructor. The current date will be displayed initially.
   **/
  explicit KDatePicker(QWidget* parent = 0);

  /**
   * The constructor. The given date will be displayed initially.
   **/
  explicit KDatePicker(const QDate& dt, QWidget* parent = 0);

  /**
   * The destructor.
   **/
  virtual ~KDatePicker();

  /** The size hint for date pickers. The size hint recommends the
   *   minimum size of the widget so that all elements may be placed
   *  without clipping. This sometimes looks ugly, so when using the
   *  size hint, try adding 28 to each of the reported numbers of
   *  pixels.
   **/
  QSize sizeHint() const;

  /**
   * Sets the date.
   *
   *  @returns @p false and does not change anything
   *      if the date given is invalid.
   **/
  bool setDate(const QDate&);

  /**
   * @returns the selected date.
   */
  const QDate &date() const;

  /**
   * Enables or disables the widget.
   **/
  void setEnabled(bool);

  /**
   * @returns the KDateTable widget child of this KDatePicker
   * widget.
   */
  KDateTable *dateTable() const;

  /**
   * Sets the font size of the widgets elements.
   **/
  void setFontSize(int);

  /**
   * Returns the font size of the widget elements.
   */
  int fontSize() const;

  /**
   * By calling this method with @p enable = true, KDatePicker will show
   * a little close-button in the upper button-row. Clicking the
   * close-button will cause the KDatePicker's topLevelWidget()'s close()
   * method being called. This is mostly useful for toplevel datepickers
   * without a window manager decoration.
   * @see hasCloseButton
   */
  void setCloseButton( bool enable );

  /**
   * @returns true if a KDatePicker shows a close-button.
   * @see setCloseButton
   */
  bool hasCloseButton() const;

protected:
  /// to catch move keyEvents when QLineEdit has keyFocus
  virtual bool eventFilter(QObject *o, QEvent *e );
  /// the resize event
  virtual void resizeEvent(QResizeEvent*);
  virtual void paintEvent(QPaintEvent *e);
  void drawFrame(QPainter *p);
  
protected Q_SLOTS:
  void dateChangedSlot(const QDate&);
  void tableClickedSlot();
  void monthForwardClicked();
  void monthBackwardClicked();
  void yearForwardClicked();
  void yearBackwardClicked();
  void selectMonthClicked();
  void selectYearClicked();
  void lineEnterPressed();
  void todayButtonClicked();
  void weekSelected(int);

Q_SIGNALS:
  /** This signal is emitted each time the selected date is changed.
   *  Usually, this does not mean that the date has been entered,
   *  since the date also changes, for example, when another month is
   *  selected.
   *  @see dateSelected
   */
  void dateChanged( const QDate&);
  /** This signal is emitted each time a day has been selected by
   *  clicking on the table (hitting a day in the current month). It
   *  has the same meaning as dateSelected() in older versions of
   *  KDatePicker.
   */
  void dateSelected( const QDate&);
  /** This signal is emitted when enter is pressed and a VALID date
   *  has been entered before into the line edit. Connect to both
   *  dateEntered() and dateSelected() to receive all events where the
   *  user really enters a date.
   */
  void dateEntered( const QDate &);
  /** This signal is emitted when the day has been selected by
   *  clicking on it in the table.
   */
  void tableClicked();

private:
  void init( const QDate &dt );
  void fillWeeksCombo(const QDate &date);
  class KDatePickerPrivate;
  KDatePickerPrivate *const d;
};

} //namespace KPlato

#endif //  KDATEPICKER_H
