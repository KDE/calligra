/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOL_H__
#define __VTOOL_H__


class KarbonPart;
class KarbonView;
class QEvent;
class VCommand;


class VTool
{
public:
	VTool( KarbonPart* part );

	virtual void activate() {}
	virtual void deactivate() {}

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	KarbonPart* part() const { return m_part; }

protected:
// TODO: remove friendship
	friend KarbonView;
	// make vtool "abstract":
	virtual ~VTool() {}

	// that's our part:
	KarbonPart* m_part;
};

#endif

