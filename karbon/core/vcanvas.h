/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCANVAS_H__
#define __VCANVAS_H__

#include <qscrollview.h>

class KarbonPart;
class KarbonView;
class KoRect;

// The canvas is a QScrollView.

class VCanvas : public QScrollView
{
	Q_OBJECT
public:
	VCanvas( KarbonView* view, KarbonPart* part );

	void repaintAll( bool drawVObjects = true );
	void setYMirroring( bool edit );
	void ViewportToContents( int vx, int vy, int &x, int &y ) const;
	QPoint ViewportToContents( const QPoint & ) const;

	QPixmap *pixmap() { return m_pixmap; }

protected:
	virtual void focusInEvent( QFocusEvent * );
	virtual void viewportPaintEvent( QPaintEvent* );
	virtual void drawContents( QPainter* painter, int clipx, int clipy,
		int clipw, int cliph  );
	void drawDocument( QPainter* painter, const QRect& rect, bool drawVObjects = true );

	virtual void resizeEvent( QResizeEvent* event );


private slots:
	void slotContentsMoving( int , int );

private:
	QPixmap *m_pixmap;
	KarbonPart* m_part;
	KarbonView* m_view;

	bool m_bScrolling;
};

#endif
