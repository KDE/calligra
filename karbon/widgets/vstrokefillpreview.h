/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VSTROKEFILLPREVIEW_H__
#define __VSTROKEFILLPREVIEW_H__

#include <qframe.h>
#include <qpixmap.h>

class VKoPainter;
class VFill;
class VStroke;
class KarbonPart;

class VStrokeFillPreview : public QFrame
{
	Q_OBJECT

public:
	VStrokeFillPreview( KarbonPart *part, QWidget* parent = 0L, const char* name = 0L );
	~VStrokeFillPreview();

	virtual QSize sizeHint() const
		{ return QSize( 50, 60 ); }
	virtual QSize minimumSizeHint() const
		{ return QSize( 20, 20 ); }
	virtual QSizePolicy sizePolicy() const
		{ return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ); }

	void update( const VStroke &, const VFill & );

	virtual bool eventFilter( QObject* object, QEvent* event );

signals:
	void strokeChanged( const VStroke & );
	void fillChanged( const VFill& );
	void fillSelected();
	void strokeSelected();

protected:
	virtual void paintEvent( QPaintEvent* event );

private:
	VKoPainter* m_painter;
	QPixmap m_pixmap;
	KarbonPart *m_part;
};

#endif

