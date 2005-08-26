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

#ifndef __VZOOMTOOL_H__
#define __VZOOMTOOL_H__

#include "koPoint.h"
#include <qstring.h>

#include <tools/vtool.h>
#include <core/vkarbonplugin.h>

class QCursor;

class KarbonViewBase;

class VZoomTool : public VTool, public VKarbonPlugin
{
public:
	VZoomTool( KarbonViewBase *view, const char *, const QStringList & );
	~VZoomTool(); 

	virtual void activate();
	virtual void deactivate();

	virtual QString name() { return i18n( "Zoom Tool" ); }
	virtual QString contextHelp();
	virtual QString icon() { return "14_zoom"; }
	virtual QString statusText();

protected:
	void draw();

	virtual void mouseButtonPress();
	virtual void mouseButtonRelease();
	virtual void mouseDrag();
	virtual void mouseDragRelease();

	virtual bool keyReleased( Qt::Key key );

	virtual void rightMouseButtonRelease();

	void recalc();

	KoPoint m_current;

private:
	QCursor* m_minusCursor;
	QCursor* m_plusCursor;

};

#endif

