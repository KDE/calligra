/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOL_H__
#define __VTOOL_H__


class KarbonView;
class QEvent;
class VCommand;


class VTool
{
public:
	VTool( KarbonView* view );

	virtual void activate() {}
	virtual void deactivate() {}

	virtual bool eventFilter( QEvent* event );

	KarbonView* view() const { return m_view; }

protected:
	// make vtool "abstract":
	virtual ~VTool() {}

private:
	// that's our view:
	KarbonView* m_view;
};

#endif

