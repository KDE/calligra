/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002 - 2005, The Karbon Developers

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

#ifndef __VCOLORDOCKER_H__
#define __VCOLORDOCKER_H__

#include <QMouseEvent>
#include <KoColor.h>

class QWidget;
class KarbonView;
class KoUniColorChooser;

class VColorDocker : public QWidget
{
	Q_OBJECT

public:
	 VColorDocker( KarbonPart* part, KarbonView* parent = 0L, const char* name = 0L );
	 virtual ~VColorDocker();

	 virtual bool isStrokeDocker() { return m_isStrokeDocker; };
	 KoColor color() { return m_color; }

public slots:
	virtual void setFillDocker();
	virtual void setStrokeDocker();
	virtual void update();

private:
	virtual void mouseReleaseEvent( QMouseEvent *e );

	KoUniColorChooser *m_colorChooser;

signals:
	void colorChanged( const KoColor &c );
	//void fgColorChanged( const QColor &c );
	//void bgColorChanged( const QColor &c );

private slots:
	void updateColor( const KoColor &c );
	void updateFgColor(const KoColor &c);
	void updateBgColor(const KoColor &c);

protected:
	bool m_isStrokeDocker; //Are we setting stroke color ( true ) or fill color ( false )
	KoColor m_color;
	KoColor m_oldColor;

private:
	KarbonPart *m_part;
	KarbonView *m_view;
	VStrokeCmd *m_strokeCmd;
};

#endif

