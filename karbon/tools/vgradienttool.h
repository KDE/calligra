/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGRADIENTTOOL_H__
#define __VGRADIENTTOOL_H__

#include "vtool.h"


class VGradientDlg;


class VGradientTool : public VTool
{
public:
	VGradientTool( KarbonView* view );
	virtual ~VGradientTool();

	virtual void activate();

	virtual bool eventFilter( QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject();

	virtual void showDialog() const;
protected:
	virtual void mousePressed( QMouseEvent * );

private:
	VGradientDlg* m_dialog;
};

#endif

