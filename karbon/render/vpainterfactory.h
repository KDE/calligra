/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPAINTERDACTORY_H__
#define __VPAINTERDACTORY_H__

// painter factory

class VPainter;
class QWidget;

class VPainterFactory
{
public:
	VPainterFactory();
	~VPainterFactory();

	static VPainter *painter();
	void setPainter( QWidget *target, int w = 0, int h = 0 );

	static VPainter *editpainter();
	void setEditPainter( QWidget *target, int w = 0, int h = 0 );

	void setWireframePainter( QWidget *target, int w = 0, int h = 0 );

private:
	static VPainter *m_painter;
	static VPainter *m_editpainter;
};

#endif
