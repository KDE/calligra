/*
 *  kis_tool_airbrush.h - part of KImageShop
 *
 *  Copyright (c) 2004 Boudewijn Rempt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#if !defined KIS_TOOL_AIRBRUSH_H
#define KIS_TOOL_AIRBRUSH_H

#include "kis_tool_paint.h"

class QPoint;
class QTimer;

class KisPainter;

class KisToolAirBrush : public KisToolPaint {
	Q_OBJECT
	typedef KisToolPaint super;
    
 public:
	KisToolAirBrush();
	virtual ~KisToolAirBrush();
  
	virtual void setup(KActionCollection *collection);

	virtual void update(KisCanvasSubject *subject);

	virtual void mousePress(QMouseEvent *e); 
	virtual void mouseMove(QMouseEvent *e);
	virtual void mouseRelease(QMouseEvent *e);
	virtual void tabletEvent(QTabletEvent *e);
	
	virtual KDialog * optionsDialog(QWidget * parent);

 protected slots:
	void timeoutPaint();  

 private:

	virtual void paintLine(const QPoint & pos1,
			       const QPoint & pos2,
			       const Q_INT32 pressure,
			       const Q_INT32 xtilt,
			       const Q_INT32 ytitl);
	virtual void initPaint(const QPoint & pos);
	virtual void endPaint();

	enumBrushMode m_mode;
	KisPainter * m_painter;

	QTimer * m_timer;
    
	QPoint m_dragStart;
	float m_dragDist;

	QPoint m_currentPos;
	Q_INT32 m_pressure;
	Q_INT32 m_xTilt;
	Q_INT32 m_yTilt;

	KisCanvasSubject *m_subject;
	KisImageSP m_currentImage;
};

#endif // KIS_TOOL_AIRBRUSH_H
