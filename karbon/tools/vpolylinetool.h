/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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

#ifndef _VPOLYLINETOOL_H_
#define _VPOLYLINETOOL_H_

#include "vtool.h"

class VComposite;

/**
 * The polyline tool.
 *
 * When the tool is activated, you draw your polyline until the tool is 
 * deactivated.
 * When the mouse button is pressed, you add a Bezier node and sets the vector by 
 * dragging it.
 * While dragging, when you press on CTRL, you edit the other vector, and when 
 * you press on SHIFT the current segment, the segment becames a line.
 */
class VPolylineTool : public VTool
{
	public:
		VPolylineTool( KarbonView* view );
		~VPolylineTool(); 
		
		virtual void        activate();
		virtual void        deactivate();
		
	protected:
			/** Helper method: draws the polyline. */
		void                draw();
			/** Helper method: draws a bezier vector. */
		void                drawBezierVector( KoPoint& start, KoPoint& end );
		
		virtual void        mouseMove();
		virtual void        mouseButtonPress();
		virtual void        mouseButtonRelease();
		virtual void        mouseButtonDlbClick();
		virtual void        mouseDrag();
		virtual void        mouseDragRelease();
		virtual void        mouseDragShiftPressed();
		virtual void        mouseDragCtrlPressed();
		virtual void        mouseDragShiftReleased();
		virtual void        mouseDragCtrlReleased();

		virtual void        cancel();

			/** The list of this polyline points. */
		QPtrList<KoPoint>   bezierPoints;
			/** The start of the last drawn vector. */
		KoPoint             lastVectorStart;
			/** The end of the last drawn vector. */
		KoPoint             lastVectorEnd;
			/** Indicates if shift is pressed. */
		bool                shiftPressed;
			/** Indicates if ctrl is pressed. */
		bool                ctrlPressed;
}; // VPolylineTool

#endif /* _VPOLYLINETOOL_H_ */
