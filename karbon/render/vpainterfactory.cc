/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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

// painter factory

#include "vpainterfactory.h"
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
	m_painter = new VQPainter( target, w, h );
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

