/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTROKEFILLPREVIEW_H__
#define __VSTROKEFILLPREVIEW_H__

#include <qframe.h>

class VStrokeFillPreview : public QFrame
{
	Q_OBJECT

public:
	VStrokeFillPreview( QWidget* parent = 0L, const char* name = 0L );

	virtual QSize sizeHint() const
		{ return QSize( 50, 50 ); }
	virtual QSize minimumSizeHint() const
		{ return QSize( 20, 20 ); }
	virtual QSizePolicy sizePolicy() const
		{ return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ); }

protected:
	virtual void paintEvent( QPaintEvent* event );

private:
};

#endif

