/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
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
		{ return QSize( 50, 50 ); }
	virtual QSize minimumSizeHint() const
		{ return QSize( 20, 20 ); }
	virtual QSizePolicy sizePolicy() const
		{ return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ); }

	void update( const VStroke &, const VFill & );

	virtual bool eventFilter( QObject* object, QEvent* event );

protected:
	virtual void paintEvent( QPaintEvent* event );

private:
	VKoPainter* m_painter;
	QPixmap m_pixmap;
	KarbonPart *m_part;
};

#endif

