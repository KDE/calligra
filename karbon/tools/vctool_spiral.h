/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLSPIRAL_H__
#define __VCTOOLSPIRAL_H__

#include "vtool.h"

class KarbonPart;
class VCDlgSpiral;

// A singleton state to create a spiral.

class VCToolSpiral : public VTool
{
public:
	virtual ~VCToolSpiral();
	static VCToolSpiral* instance( KarbonPart* part );

	virtual VCommand* createCmd( const QPoint& p, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

protected:
	VCToolSpiral( KarbonPart* part );

private:
	static VCToolSpiral* s_instance;

	VCDlgSpiral* m_dialog;
};

#endif

