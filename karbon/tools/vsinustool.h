/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSINUSTOOL_H__
#define __VSINUSTOOL_H__

#include "vshapetool.h"

class KarbonPart;
class VSinusDlg;

// A singleton state to create a sinus

class VSinusTool : public VShapeTool
{
public:
	virtual ~VSinusTool();
	static VSinusTool* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

	virtual void showDialog() const;

protected:
	VSinusTool( KarbonPart* part );

private:
	static VSinusTool* s_instance;

	VSinusDlg* m_dialog;
};

#endif

