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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VSELECTTOOL_H__
#define __VSELECTTOOL_H__

#include <kdialogbase.h>
#include <klocale.h>

#include "vtool.h"

class KarbonView;

class VSelectOptionsWidget : public KDialogBase
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
public:
	VSelectTool( KarbonPart *part, const char* name );
	virtual ~VSelectTool();

	virtual void activate();

	virtual bool showDialog() const;
	virtual QString name() { return i18n( "Select Tool" ); }
	virtual QString icon() { return "14_select"; }
	virtual QString category() { return "manipulation"; }
	virtual QString statusText();
	virtual uint priority() { return 1; }
	virtual QString contextHelp();

	virtual void refreshUnit();

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

	virtual void cancel();

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
	KoPoint m_sp;
	KoPoint m_current;

	VHandleNode m_activeNode;

	void recalc();

	// A list of temporary objects:
	VObjectList m_objects;
	// The options widget.
	VSelectOptionsWidget	*m_optionsWidget;
};

#endif

