/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPAINTERDACTORY_H__
#define __VPAINTERDACTORY_H__

// painter factory

class VPainter;
class QPaintDevice;

class VPainterFactory
{
public:
	VPainterFactory();
	~VPainterFactory();

	VPainter *painter();
	void setPainter( QPaintDevice *target, int w = 0, int h = 0 );

	VPainter *editpainter();
	void setEditPainter( QPaintDevice *target, int w = 0, int h = 0 );

	void setWireframePainter( QPaintDevice *target, int w = 0, int h = 0 );

private:
	VPainter *m_painter;
	VPainter *m_editpainter;
};

#endif
