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

#ifndef __VPOLYLINETOOL_H__
#define __VPOLYLINETOOL_H__


#include <qptrlist.h>
#include <qstring.h>

#include "koPoint.h"

#include "vtool.h"


class QLabel;
class QWidget;
class VPath;


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
		VPolylineTool( KarbonPart *part );
		~VPolylineTool(); 

		virtual void activate();
		virtual void deactivate();

		virtual QString name() { return i18n( "Polyline Tool" ); }
		virtual QString contextHelp();
		virtual QString icon() { return "14_polyline"; }

	protected:
		/**
		 * Helper method: draws the polyline.
		 */
		void draw();

		/**
		 * Helper method: draws a bezier vector.
		 */
		void drawBezierVector( KoPoint& start, KoPoint& end );

		virtual void mouseMove();
		virtual void mouseButtonPress();
		virtual void mouseButtonRelease();
		virtual void mouseButtonDblClick();
		virtual void mouseDrag();
		virtual void mouseDragRelease();
		virtual void mouseDragShiftPressed();
		virtual void mouseDragCtrlPressed();
		virtual void mouseDragShiftReleased();
		virtual void mouseDragCtrlReleased();

		virtual void cancel();
		virtual void cancelStep();
		virtual void accept();

		/**
		 * The list of this polyline points.
		 */
		QPtrList<KoPoint> m_bezierPoints;

		/**
		 * The start of the last drawn vector.
		 */
		KoPoint m_lastVectorStart;

		/**
		 * The end of the last drawn vector.
		 */
		KoPoint m_lastVectorEnd;

		/**
		 * Indicates if the polyline is to close.
		 */
		bool m_close;
};

#endif

