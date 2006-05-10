/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef _VGRADIENTWIDGET_H_
#define _VGRADIENTWIDGET_H_

#include <QWidget>
#include <koffice_export.h>
class VGradient;
class QPainter;
class VColor;

class KARBONBASE_EXPORT VGradientWidget : public QWidget
{
	Q_OBJECT

public:
	VGradientWidget( VGradient& gradient, QWidget* parent = 0L, const char* name = 0L );
	~VGradientWidget();

	virtual void paintEvent( QPaintEvent* );

signals:
	void changed();

protected:
	/** mouse events... For color stops manipulation */
	void mousePressEvent( QMouseEvent* );
	void mouseReleaseEvent( QMouseEvent* );
	void mouseDoubleClickEvent( QMouseEvent* );
	void mouseMoveEvent( QMouseEvent* );

	void paintColorStop( QPainter& p, int x, VColor& color );
	void paintMidPoint( QPainter& p, int x );

	/** The gradient to modify. */
	VGradient*             m_gradient;
	/** The point to modify. */
	unsigned int currentPoint;
	
	QRect m_pntArea;
}; // VGradientWidget

#endif /* _VGRADIENTWIDGET_H_ */
