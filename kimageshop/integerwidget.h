/*
 *  integerwidget.h - part of KImageShop
 *
 *  A convenience widget for setting integer values
 *  Consists of a SpinBox and a slider
 *
 *  Copyright (c) 1999 Carsten Pfeiffer <pfeiffer@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef INTEGERWIDGET_H
#define INTEGERWIDGET_H

#include <qslider.h>
#include <qwidget.h>

class QHBoxLayout;
class QSpinBox;

class IntegerWidget : public QWidget
{
  Q_OBJECT

public:
  IntegerWidget( int min, int max, QWidget *parent, const char *name );
  ~IntegerWidget();

  int 		value();
  void 		setRange( int min, int max );
  void 		setEditFocus();

  void 		setTickmarks( QSlider::TickSetting );
  void 		setTickInterval ( int );
  int 		tickInterval() const;

  
public slots:
  void 		setValue( int value );

  
protected:
  void 		initGUI();

  QSlider 	*slider;
  QSpinBox 	*spinBox;


private:
  QHBoxLayout 	*layout;


protected slots:
  void 		setSliderValue( int );


signals:
  void 		valueChanged( int );

};



#endif // INTEGERWIDGET_H
