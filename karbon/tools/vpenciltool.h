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

#ifndef __VPENCILTOOL_H__
#define __VPENCILTOOL_H__


#include <q3ptrlist.h>
#include <QString>
//Added by qt3to4:
#include <QLabel>

#include <klocale.h>
#include <kdialogbase.h>

#include "vtool.h"


class QLabel;
class QWidget;
class VPath;
class KarbonView;
class QCheckBox;
class Q3WidgetStack;
class QComboBox;
class KDoubleNumInput;
class QCursor;
class QPointF;

class VPencilOptionsWidget : public KDialogBase
{
	Q_OBJECT
	public:
		VPencilOptionsWidget( KarbonView*view, QWidget* parent = 0L, const char* name = 0L );

		int currentMode();
		bool optimize();
		float combineAngle();
		float fittingError();
		
	public slots:
		void selectMode();

	private:
		KarbonView			*m_view;
		QCheckBox			*m_optimizeRaw;
		QCheckBox			*m_optimizeCurve;
		KDoubleNumInput			*m_combineAngle;
		KDoubleNumInput			*m_fittingError;
		Q3WidgetStack			*m_widgetStack;
		QComboBox			*m_combo;
		int				m_mode;
};


/**
 * The pencil tool.
 *
 * This tool has three modes of operation
 * 	* Raw -  exactly what is drawn by the user is what is captured
 * 	* Straightend - captured data is straightended 
 * 	* Smooth - captured data is converted to a bezier curve
 * 	
 */

class VPencilTool : public VTool
{
	public:
		VPencilTool( KarbonView *view );
		~VPencilTool(); 

		virtual void activate();
		virtual void deactivate();

		virtual void setup(KActionCollection *collection);
		virtual QString uiname() { return i18n( "Pencil Tool" ); }
		virtual QString contextHelp();
		virtual enumToolType toolType() { return TOOL_FREEHAND; }

		virtual bool showDialog() const;
		
		enum {
			RAW,
			STRAIGHT,
			CURVE
		};

	protected:
		/**
		 * Helper method: draws the polyline.
		 */
		void draw();

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
		Q3PtrList<QPointF> m_Points;

		/**
		 * The start of the last drawn vector.
		 */
		QPointF m_lastVectorStart;

		/**
		 * The end of the last drawn vector.
		 */
		QPointF m_lastVectorEnd;

		/**
		 * Indicates if the Line is to close.
		 */
		bool m_close;

		/**
		 * The mode of the pencile
		 */
		
		short m_mode;

		bool m_optimize;

		float m_combineAngle;

		VPencilOptionsWidget *m_optionWidget;

	private:
		QCursor* m_cursor;
};

#endif
