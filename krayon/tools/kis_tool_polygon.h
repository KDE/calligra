/*
 *  polygontool.h - part of Krayon
 *
 *  Copyright (c) 2001 Toshitaka Fujioka <fujioka@kde.org>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __polygontool_h__
#define __polygontool_h__

#include <qpoint.h>

#include "kis_tool.h"

class KisDoc;
class KisView;
class KisCanvas;

class PolyGonTool : public KisTool {
public:
	PolyGonTool( KisDoc* _doc, KisView* _view, KisCanvas* _canvas );
	virtual ~PolyGonTool();

	virtual void toolSelect();
	virtual void optionsDialog();
	virtual void setupAction(QObject *collection);

public slots:
	virtual void mousePress(QMouseEvent *event);
	virtual void mouseMove(QMouseEvent *event);
	virtual void mouseRelease(QMouseEvent *event);
    
protected:

    void drawPolygon( const QPoint&, const QPoint& );

private:

    int         lineThickness;
    int         lineOpacity;
    int         cornersValue;
    int         sharpnessValue;

    bool        usePattern;
    bool        useGradient;
    bool        useRegions;
    bool        checkPolygon;
    bool        checkConcavePolygon;
        
    QPoint      m_dragStart;
    QPoint      m_dragEnd;
    QPoint      mStart;
    QPoint      mFinish;
    
    bool        m_dragging;
    bool        m_done;
    
    KisCanvas   *pCanvas;

    QPointArray drawPoints;
};

#endif //__polygontool_h__
