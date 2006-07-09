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

#ifndef __VTRANSFORMDOCKER_H__
#define __VTRANSFORMDOCKER_H__

class KarbonPart;
class KarbonView;
class KoUnitDoubleSpinBox;

class VTransformDocker : public QWidget
{
	Q_OBJECT

public:
	VTransformDocker( KarbonPart* part, KarbonView* parent = 0L, const char* name = 0L );

public slots:
	void update();
	virtual void setUnit( KoUnit::Unit unit );

private slots:
	void translate();
	void scale();
	void enableSignals( bool enable );
	void shearX();
	void shearY();
	void rotate();

private:
	KarbonPart *m_part;
	KarbonView *m_view;
	KoUnitDoubleSpinBox *m_x;
	KoUnitDoubleSpinBox *m_y;
	KoUnitDoubleSpinBox *m_width;
	KoUnitDoubleSpinBox *m_height;
	KDoubleSpinBox *m_rotate;
	KoUnitDoubleSpinBox *m_shearX;
	KoUnitDoubleSpinBox *m_shearY;
};

#endif

