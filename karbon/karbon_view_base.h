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

#ifndef __KARBON_VIEW_BASE__
#define __KARBON_VIEW_BASE__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <koView.h>
#include <koPoint.h>
#include <koffice_export.h>

class KarbonPartBase;
class VPainterFactory;
class VCanvas;
class VTool;
class KoRect;

class KARBONBASE_EXPORT KarbonViewBase : public KoView
{
public:
	KarbonViewBase( KarbonPartBase* part, QWidget* parent = 0L, const char* name = 0L );
	virtual ~KarbonViewBase();

	virtual VCanvas* canvasWidget() const { return 0L; }
	virtual VPainterFactory* painterFactory() const { return 0L; }

	KarbonPartBase* part() const { return m_part; }

	// manage tools
	virtual void registerTool( VTool * ) {}

	virtual bool mouseEvent( QMouseEvent* , const KoPoint & ) = 0;
	virtual bool keyEvent( QEvent* ) = 0;
	virtual void dropEvent( QDropEvent *e ) = 0;

	virtual void setViewportRect( const KoRect &rect ) = 0;
	virtual void setZoomAt( double zoom, const KoPoint & = KoPoint() ) = 0;

protected:
	KarbonPartBase* m_part;
};

#endif

