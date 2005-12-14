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

/* vcolorslider.h */
#ifndef VCOLORSLIDER_H
#define VCOLORSLIDER_H

#include <qwidget.h>

class QLabel;
class KIntSpinBox;
class KGradientSelector;

/**
 * This is the color slider widget that is used to select color or color components.
 * It combines a label, a gradient selector and a spinbox.
 */
class VColorSlider : public QWidget
{
	Q_OBJECT
public:
	/**
	 * Constructs a new color slider.
	 *
	 * @param parent the parent widget
	 * @param name the slider's name
	 */
	VColorSlider( QWidget* parent = 0L, const char* name = 0L );

	/**
	 * Constructs a new color slider.
	 *
	 * @param label the label text
	 * @param col1 the left color
	 * @param col2 the right color
	 * @param min the minimum value
	 * @param max the maximum value
	 * @param value the actual value
	 * @param parent the parent widget
	 * @param name the slider's name
	 */
	VColorSlider( const QString& label, const QColor& col1, const QColor& col2,
		int min, int max, int value, QWidget* parent = 0L, const char* name = 0L );
	
	/** Destroys the color slider */
	~VColorSlider();
	
	/** 
	 * Reflects if the slider is still being dragged while the color changes 
	 *
	 * @return true if slider is still dragged, else false
	 */
	bool isDragging() { return m_isDragging; }
public slots:
	
	/**
	 * Sets the description of the slider 
	 *
	 * @param label the new label text
	 */
	virtual void setLabel( const QString& label );
	
	/**
	* Sets the colors for the slider.
	*
	* @param color1 the new left color
	* @param color2 the new right color
	*/
	virtual void setColors( const QColor& color1, const QColor& color2 );
	
	/**
	 * Sets the value of the spinbox (and the value of the vcolorslider).
	 *
	 * @param value the new value
	 */
	virtual void setValue( int value );

	/**
	 * Sets the minimum value of the spinbox and slider.
	 *
	 * @param value the new minimum value
	 */
	virtual void setMinValue( int value );
	
	/**
	 * Sets the maximum value of the spinbox and slider.
	 *
	 * @param value the new maximum value
	 */
	virtual void setMaxValue( int value );
	
	/**
	 * Retrieves the actual value of the spinbox and slider.
	 *
	 * @return the actual value
	 */
	int value();

private:
	void init();
 	bool eventFilter( QObject *obj, QEvent *ev );
	QLabel* m_label;
	KIntSpinBox* m_spinBox;
	KGradientSelector* m_gradientSelect;
	bool m_isDragging;
	int m_minValue;
	int m_maxValue;

signals:
	/** 
	* Is emitted whenever the slider or spinbox value has changed.
	* Use @ref isDragging to know if the slider is still being dragged.
	*
	* @param value the actual value
	*/
	void valueChanged( int value );

private slots:
	void updateFrom_spinBox( int );
	void updateFrom_gradientSelect( int );
};

#endif

