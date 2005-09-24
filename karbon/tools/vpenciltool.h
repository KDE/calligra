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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

*/

#ifndef __VPENCILTOOL_H__
#define __VPENCILTOOL_H__


#include <qptrlist.h>
#include <qstring.h>

#include <klocale.h>
#include <kdialogbase.h>
#include "koPoint.h"

#include "vtool.h"


class QLabel;
class QWidget;
class VPath;
class KarbonPart;
class QCheckBox;
class QWidgetStack;
class QComboBox;
class KDoubleNumInput;
		
class VPencilOptionsWidget : public KDialogBase
{
	Q_OBJECT
	public:
		VPencilOptionsWidget( KarbonPart*part, QWidget* parent = 0L, const char* name = 0L );

		int currentMode();
		bool optimize();
		float combineAngle();
		float fittingError();
		
	public slots:
		void selectMode();

	private:
		KarbonPart			*m_part;
		QCheckBox			*m_optimizeRaw;
		QCheckBox			*m_optimizeCurve;
		KDoubleNumInput			*m_combineAngle;
		KDoubleNumInput			*m_fittingError;
		QWidgetStack			*m_widgetStack;
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
		VPencilTool( KarbonPart *part );
		~VPencilTool(); 

		virtual void activate();
		virtual void deactivate();

		virtual QString name() { return i18n( "Pencil Tool" ); }
		virtual QString contextHelp();
		virtual QString icon() { return "14_pencil"; }
	
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
		QPtrList<KoPoint> m_Points;

		/**
		 * The start of the last drawn vector.
		 */
		KoPoint m_lastVectorStart;

		/**
		 * The end of the last drawn vector.
		 */
		KoPoint m_lastVectorEnd;

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
};

#endif
