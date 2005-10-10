/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers

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

/* This is the color slider widget that is used to select color or color components */

/* vcolorslider.h */
#ifndef VCOLORSLIDER_H
#define VCOLORSLIDER_H

#include <qwidget.h>

class QLabel;
class KIntSpinBox;
class KGradientSelector;

class VColorSlider : public QWidget
{
	Q_OBJECT
public:
	VColorSlider( QWidget* parent = 0L, const char* name = 0L );
	VColorSlider( const QString&, const QColor&, const QColor&,
		int, int, int, QWidget* parent = 0L, const char* name = 0L );
	~VColorSlider();
	
	/** reflects if the slider is still being dragged while the color changes */
	bool isDragging() { return m_isDragging; }
public slots:
	
	/** Sets the description of the slider */
	virtual void setLabel( const QString& );
	
	/** 
	* Sets the colors for the slider. 
	* @param Color1 is left
	* @param Color2 is right 
	*/
	virtual void setColors( const QColor&, const QColor& );
	
	/** Sets the value of the spinbox (and the value of the vcolorslider) */
	virtual void setValue( int );
	/** Sets the minimum value of the spinbox and slider */
	virtual void setMinValue( int );
	/** Sets the maximum value of the spinbox and slider */
	virtual void setMaxValue( int );
	/** Retrieves the actual value of the spinbox and slider */
	int value();

private:
	void init();
 	bool eventFilter( QObject *obj, QEvent *ev );
	QLabel* m_label;
	KIntSpinBox* m_spinBox;
	KGradientSelector* m_gradientSelect;
	bool m_isDragging;

signals:
	/** 
	* is emitted whenever the slider or spinbox value has changed
	* use @ref isDragging to know if the slider is still being dragged
	*/
	void valueChanged( int value );

private slots:
	void updateFrom_spinBox( int );
	void updateFrom_gradientSelect( int );
};

#endif

