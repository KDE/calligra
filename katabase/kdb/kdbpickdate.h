/***************************************************************************
                          kdbpickdate.h  -  description                              
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


#ifndef KDBPICKDATE_H
#define KDBPICKDATE_H

#include <qwidget.h>
#include <qdatetime.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qpointarray.h>
#include <qfont.h>
#include <qfontmetrics.h>

/**
  *@author Ørn E. Hansen
  */

class kdbPickDate : public QWidget  {
	Q_OBJECT
private:
	QDate  _date;
	QFont  _small;
	QFont  _headr;
	QRect  _contents;
	QRect  _l, _r;
	QPoint _pos;
	bool   _lpushed, _rpushed;
	
	void markPos(QPoint, bool);
	void drawCell(QPainter&, uint, uint, bool);
	void drawArrows(Qt::ArrowType, bool);
	
public: 
	kdbPickDate(QWidget *p_par=0, const char *p_nam=0, WFlags p_f=0);
	kdbPickDate(const QDate&);
	~kdbPickDate();
	
	void setDate(const QDate&);
	void setDate(uint, uint, uint);
	
	void setText(const QString&, const QString&);
	
	QSize       sizeHint() const;
	QSizePolicy sizePolicy() const;
	QDate       getDate() const;
	
	QString     format();
	QString     format(const QString&);
	
protected:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void resizeEvent(QResizeEvent *);
	void focusInEvent(QFocusEvent *);
	void focusOutEvent(QFocusEvent *);
	
signals:
	void dateSet(const QDate&);
	
};

#endif













