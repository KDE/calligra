/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLSINUS_H__
#define __VCTOOLSINUS_H__

#include "vtool.h"

class KarbonPart;
class VCDlgSinus;

// A singleton state to create a sinus

class VCToolSinus : public VTool
{
public:
	virtual ~VCToolSinus();
	static VCToolSinus* instance( KarbonPart* part );

	virtual VCommand* createCmdFromDialog( const QPoint& point );
	virtual VCommand* createCmdFromDragging( const QPoint& tl, const QPoint& br );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& tl, const QPoint& br );

protected:
	VCToolSinus( KarbonPart* part );

private:
	static VCToolSinus* s_instance;

	VCDlgSinus* m_dialog;
};

#endif

