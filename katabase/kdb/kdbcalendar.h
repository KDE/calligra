/***************************************************************************
                          kdbCalendar.h  -  description
                             -------------------
    begin                : Fri Jun 25 1999
    copyright            : (C) 1999 by Ørn E. Hansen
    email                : hanseno@mail.bip.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __CALENDAR_H
#define __CALENDAR_H

#include <qwidget.h>
#include <qdatetime.h>
#include <langinfo.h>

#include <kdbdatafield.h>
#include <kdbpickdate.h>

/** The Calendar provides an easy method, to edit date fields in Qt.
	*
	* This widget, provides means to edit and modify date fields.  With
	* graphical methods.  Where a calendar of the month, within the
	* year is presented and the user can select the day.  Or change the
	* month by browsing.  Or alternatively, edit the text to the date.
	*
	* The widget uses local names for months and days, using the
	* information in the langinfo() database. This is an addition to
	* the original widget, added by Orn Hansen <hanseno@mail.bip.net>
	*
	* Rewritten from an original by Mark Jackson <mdj@phoenix.bmdesign.com>
	* and made to comply with ComboBox features, Qt-2.0, locales and QDate.
	*@author Örn E. Hansen <hanseno@mail.bip.net>
 */
class kdbCalendar : public QWidget {
  Q_OBJECT
public:
  kdbCalendar(const QString&,kdbDataField*,QWidget *p=0,const char *n=0);
  ~kdbCalendar(void);

  /** Returns the date, as text.  In a format, specified by the user. */
	const QString& text();
	/** Set the date, according to the text.  The text must be in
		* accordance with the format, given on output.  For the date to
		* be recognized.
	*/
	void setText(const QString&);
	/** Bind this widget, to a datafield. */
	void bindField(kdbDataField *);
	
	void popUp();
	void popDown();
	
protected:
	bool event(QEvent *);
	bool eventFilter(QObject *, QEvent *);
  void paintEvent(QPaintEvent*);
  void resizeEvent(QResizeEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void focusOutEvent(QFocusEvent*);

signals:
	void dateChanged(const QDate&);
	
protected slots:
	void datePicked(const QDate&);
	
private:
	/** If this is a widget, connected to a datafield, then all changes
		* made to the widget text, will be assigned to the datafield.  And
		* the data from the field, will be read when the widget gets an
		* event to show it's contents.
	*/
	kdbDataField *_field;
  kdbPickDate  *_date;
  QRect         _arrow;
  QRect         _contents;
	QString       _str;
	QString       _dateFormat;
	bool          _pushed;
	bool          _arrowDown;
	bool          _mouseWasInsidePopup;
	bool          _discardPress;	
};

#endif





























