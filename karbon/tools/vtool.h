/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOL_H__
#define __VTOOL_H__

#include <koPoint.h>

class KarbonView;
class QEvent;
class VPainter;


class VTool
{
public:
	VTool( KarbonView* view );

	/**
	 * Activate the tool. The tools is supposed to set a mouse cursor and/or
	 * the statusbar properly here.
	 */
	virtual void activate() {}

	/**
	 * Deactivate the tool.
	 */
	virtual void deactivate() {}

	/**
	 * This function processes every important mouse or keyboard event.
	 * It then calls suiting functions like mouseMoved() so deriving tools
	 * don't need to directly deal with events themselves.
	 */
	bool eventFilter( QEvent* event );

protected:
	virtual void draw(/* VPainter* painter*/ ) {}

	virtual void setCursor( const KoPoint& /*current*/ ) const {}

	virtual void mouseButtonPress( const KoPoint& /*current*/ ) {}
	virtual void mouseButtonRelease( const KoPoint& /*current*/ ) {}
	virtual void mouseMove( const KoPoint& /*current*/ ) {}
	virtual void mouseDrag( const KoPoint& /*current*/ ) {}
	virtual void mouseDragRelease( const KoPoint& /*current*/ ) {}
	virtual void mouseDragShiftPressed( const KoPoint& /*current*/ ) {}
	virtual void mouseDragCtrlPressed( const KoPoint& /*current*/ ) {}
	virtual void mouseDragShiftReleased( const KoPoint& /*current*/ ) {}
	virtual void mouseDragCtrlReleased( const KoPoint& /*current*/ ) {}
	virtual void mouseDragAltPressed( const KoPoint& /*current*/ ) {}

	/**
	 * Cancels all tool operations. This event is invoked when ESC is pressed.
	 */
	virtual void cancel() {}

	// Make VTool "abstract":
	virtual ~VTool() {}

	KarbonView* view() const { return m_view; }

	/**
	 * Most tools need to know the first mouse coordinate.
	 */
	const KoPoint& first( bool raw = false ) const { return raw ? m_firstPointRaw : m_firstPoint; }
	const KoPoint& last( bool raw = false ) const { return raw ? m_lastPointRaw : m_lastPoint; }

private:
	/**
	 * The view the tool acts upon.
	 */
	KarbonView* m_view;

	/**
	 * First input mouse coordinate.
	 */
	KoPoint m_firstPoint;
	KoPoint m_firstPointRaw;

	/**
	 * Last input mouse coordinate.
	 */
	KoPoint m_lastPoint;
	KoPoint m_lastPointRaw;

	/**
	 * A tool state.
	 */
	bool m_mouseButtonIsDown;

	/**
	 * A tool state.
	 */
	bool m_isDragging;
};

#endif

