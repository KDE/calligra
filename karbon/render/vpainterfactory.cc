/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

// painter factory

#include "vpainterfactory.h"
#include "vkopainter.h"
#include "vqpainter.h"

VPainterFactory::VPainterFactory()
{
	m_painter = 0L;
	m_editpainter = 0L;
}

VPainterFactory::~VPainterFactory()
{
	delete m_painter;
	delete m_editpainter;
}

VPainter *
VPainterFactory::painter()
{
	return m_painter;
}

void
VPainterFactory::setPainter( QPaintDevice *target, int w, int h )
{
	if( m_painter )
		delete m_painter;
	m_painter = new VKoPainter( target, w, h );
}

VPainter *
VPainterFactory::editpainter()
{
	return m_editpainter;
}

void
VPainterFactory::setEditPainter( QPaintDevice *target, int w, int h )
{
	if( m_editpainter )
		delete m_editpainter;
	m_editpainter = new VQPainter( target, w, h );
}

void
VPainterFactory::setWireframePainter( QPaintDevice *target, int w, int h )
{
	if( m_painter )
		delete m_painter;
	m_painter = new VQPainter( target, w, h );
}

