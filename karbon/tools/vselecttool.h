/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef __VSELECTTOOL_H__
#define __VSELECTTOOL_H__

#include <kdialog.h>
#include <klocale.h>

#include "vtool.h"

class QPointF;
class KarbonView;

class VSelectOptionsWidget : public KDialog
{
Q_OBJECT

public:
	VSelectOptionsWidget( KarbonPart* part );

public slots:
	void modeChange( int mode );

private:
	KarbonPart*     m_part;
}; // VSelectOptionsWidget

class VSelectTool : public VTool
{
Q_OBJECT

public:
	VSelectTool( KarbonView *view );
	virtual ~VSelectTool();

	virtual void setup(KActionCollection *collection);
	virtual bool showDialog() const;
	virtual QString uiname() { return i18n( "Select Tool" ); }
	virtual enumToolType toolType() { return TOOL_SELECT; }
	virtual QString statusText();
	virtual uint priority() { return 0; }
	virtual QString contextHelp();

	virtual void refreshUnit();

	virtual void activate();

protected:
	virtual void draw();

	virtual void setCursor() const;

	virtual void mouseButtonPress();
	virtual void rightMouseButtonPress();
	virtual void mouseButtonRelease();
	virtual void rightMouseButtonRelease();
	virtual void mouseDrag();
	virtual void mouseDragRelease();
	virtual void mouseDragCtrlPressed();
	virtual void mouseDragCtrlReleased();
	virtual void mouseDragShiftPressed();
	virtual void mouseDragShiftReleased();
	virtual void arrowKeyReleased( Qt::Key );
	virtual bool keyReleased( Qt::Key );

	virtual void cancel();

protected slots:
	void updateStatusBar() const;

private:
	enum { normal, moving, scaling, rotating } m_state;
	bool m_lock;
	// controls if objects are added to or removed from the selection
	bool m_add;
	double m_s1;
	double m_s2;
	double m_distx;
	double m_disty;
	QPointF m_sp;
	QPointF m_current;

	VHandleNode m_activeNode;

	void recalc();

	// A list of temporary objects:
	VObjectList m_objects;
	// The options widget.
	VSelectOptionsWidget	*m_optionsWidget;
};

#endif

