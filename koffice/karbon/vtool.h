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

#ifndef __VTOOL_H__
#define __VTOOL_H__

#include <qobject.h>
#include <kaction.h>
#include <klocale.h>
#include <KoPoint.h>
#include <koffice_export.h>
class KRadioAction;
class KarbonPart;
class KarbonView;
class QEvent;
class QWidget;
class VPainter;
class VToolController;

enum enumToolType
{
	TOOL_SELECT = 0,       // 
	TOOL_FREEHAND = 1,     //
	TOOL_SHAPE = 2,        // Geometric shapes like ellipses and lines
	TOOL_MANIPULATION = 3, //
	TOOL_MISC = 4          //
};

/**
 * The base class for all karbon tools.
 *
 * Each tool has an icon, name and category. Basic mouse event and key handling is
 * implemented here.
 */
class KARBONBASE_EXPORT VTool : public QObject
{
	Q_OBJECT

public:
	/**
	 * Constructs a new tool connected to the specified karbon view.
	 * 
	 * @param view the karbon view the tool is connected to
	 * @param name unused
	 */
	VTool( KarbonView *view, const char* name );
	// Make VTool "abstract":
	
	/**
	 * Destroys the tool and unregisters it from the connected part.
	 */
	virtual ~VTool();

	/**
	 * Registers the specified tool at the connected part.
	 *
	 * Tools should register themselves by calling this function.
	 *
	 * @param tool the tool to register
	 */
	virtual void registerTool( VTool *tool );

	/**
	 * Deactivates the tool.
	 */
	virtual void deactivate() {}

	/**
	 * Shows the tools option dialog.
	 */
	virtual bool showDialog() const { return false; }

	/**
	 * The name of the tool.
	 */
	virtual QString uiname() { return i18n( "Unnamed Tool" ); }

	/**
	 * The context help of the tool.
	 */
	virtual QString contextHelp() { return i18n( "This tool has no description." ); }

	/**
	 * The tool type.
	 */
	virtual enumToolType toolType() { return TOOL_MISC; }

	/**
	 * The tool status text.
	 */
	virtual QString statusText() { return ""; }

	/**
	 * Position in toolbox. Zero means no preferred position, other
	 * values indicate the true position.
	 */
	virtual uint priority() { return 0; }

	/**
	 * The tool icon name.
	 */
	QString icon() { return m_action->icon(); }

	/**
	 * This function processes every important mouse event.
	 * It then calls suiting functions like mouseMoved() so deriving tools
	 * don't need to directly deal with events themselves.
	 */
	bool mouseEvent( QMouseEvent* event, const KoPoint& );

	/**
	 * This function processes every important key event.
	 * It then calls suiting functions like mouseButtonPress() so deriving tools
	 * don't need to directly deal with events themselves.
	 */
	bool keyEvent( QEvent* event );

	/**
	 * This function is called when the documents unit setting were changed.
	 */
	virtual void refreshUnit() {}

	/**
	 * Called after tool creation. A tool is supposed to hook its associated action 
	 * into the specified action collection here.
	 */
	virtual void setup(KActionCollection *) {}

	/**
	 * Returns the tool's associated action object.
	 *
	 * @return the associated action
	 */
	KRadioAction *action() const { return m_action; }

	/**
	 * Draws the actual tool state.
	 *
	 * @param painter the painter on which to paint
	 */
	virtual void draw( VPainter* painter ) { Q_UNUSED( painter ); }

public slots:
	/**
	 * Called during the tool activation. A tool is supposed to set a mouse cursor and/or
	 * the statusbar properly here.
	 */
	virtual void activate();

protected:
	/**
	 * Returns the current dagging state.
	 *
	 * @return true if the mouse is currently dragged, else false
	 */
	bool isDragging() const { return m_isDragging; }

	virtual void draw( /*VPainter* painter*/ ) {}

	/**
	 * This function is called on each mouse event the tool receives.
	 */
	virtual void setCursor() const {}

	/**
	 * Left mouse button press.
	 */
	virtual void mouseButtonPress() {}

	/**
	 * Right mouse button press.
	 */
	virtual void rightMouseButtonPress() {}

	/**
	 * Left mouse button release. The mouse wasn't moved.
	 */
	virtual void mouseButtonRelease() {}

	/**
	 * Right mouse button release. The mouse wasn't moved.
	 */
	virtual void rightMouseButtonRelease() {}

	/**
	 * Mouse button double click.
	 */
	virtual void mouseButtonDblClick() {}

	/**
	 * Mouse move. No mouse button is pressed.
	 */
	virtual void mouseMove() {}

	/**
	 * Mouse drag.
	 */
	virtual void mouseDrag() {}

	/**
	 * Mouse button release. The mouse was moved before.
	 */
	virtual void mouseDragRelease() {}

	/**
	 * Mouse drag with "Shift" key pressed at the same time.
	 */
	virtual void mouseDragShiftPressed() {}

	/**
	 * Mouse drag with "Ctrl" key pressed at the same time.
	 */
	virtual void mouseDragCtrlPressed() {}

	/**
	 * "Shift" key released while mouse drag.
	 */
	virtual void mouseDragShiftReleased() {}

	/**
	 * "Ctrl" key released while mouse drag.
	 */
	virtual void mouseDragCtrlReleased() {}

	/**
	 * "Arrow" key released up, down, left, right
	 */
	virtual void arrowKeyReleased( Qt::Key ) {}

	/**
	 * Specified key released.
	 */
	virtual bool keyReleased( Qt::Key ) { return false; }

	/**
	 * Cancels all tool operations. This event is invoked when ESC is pressed.
	 */
	virtual void cancel() {}

	/**
	 * Cancels the last tool step (if any). This event is invoked when Backspace is pressed.
	 */
	virtual void cancelStep() {}

	/**
	 * Terminates the current tool drawing (if any). This event is invoked when Enter/Return is pressed.
	 */
	virtual void accept() {}

	/**
	 * Returns the connected karbon part.
	 */
//	KarbonPart* part() const { return m_part; }

	/**
	 * Returns the connected karbon view.
	 */
	KarbonView* view() const;

	/**
	 * Most tools need to know the first mouse coordinate.
	 */
	const KoPoint& first() const { return m_firstPoint; }

	/**
	 * The last mouse coordinate.
	 */
	const KoPoint& last() const { return m_lastPoint; }

	/**
	 * The status of the shift key.
	 *
	 * @return true if key is pressed, else false
	 */
	bool shiftPressed() const { return m_shiftPressed; }

	/**
	 * The status of the ctrl key.
	 *
	 * @return true if key is pressed, else false
	 */
	bool ctrlPressed() const { return m_ctrlPressed; }

	/**
	 * The status of the alt key.
	 *
	 * @return true if key is pressed, else false
	 */
	bool altPressed() const { return m_altPressed; }

	/** The tool's action object. */
	KRadioAction *m_action;

	/** Helper function. Returns the parent view's toolcontroller. */
	VToolController *toolController() const;

private:
	/**
	 */
	KarbonView *m_view;

	/**
	 * First input mouse coordinate.
	 */
	KoPoint m_firstPoint;

	/**
	 * Last input mouse coordinate.
	 */
	KoPoint m_lastPoint;

	/**
	 * A tool state.
	 */
	bool m_mouseButtonIsDown;

	/**
	 * A tool state.
	 */
	bool m_isDragging;

	/**
	 * Indicates if shift is pressed.
	 */
	bool m_shiftPressed;

	/**
	 * Indicates if ctrl is pressed.
	 */
	bool m_ctrlPressed;

	/**
	 * Indicates if Alt is pressed.
	 */
	bool m_altPressed;
};

#endif

