/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTATEBUTTON_H__
#define __VSTATEBUTTON_H__

#include <qpushbutton.h>
#include <qpixmap.h>
#include <qptrlist.h>

class VStateButton : public QPushButton
{
public:
	VStateButton( QWidget* parent = 0L, const char* name = 0L );
	~VStateButton();

	void addState( QPixmap *state ) { m_pixmaps.append( state ); }
	void setState( unsigned int index );
	unsigned int getState() const { return m_index; }

private:
	void mouseReleaseEvent( QMouseEvent * );

	QPtrList<QPixmap> m_pixmaps;
	unsigned int m_index;
};

#endif

