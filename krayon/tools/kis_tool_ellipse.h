/*
 *  kis_tool_ellipse.h - part of Krayon
 *
 *  Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
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

#ifndef __ellipsetool_h__
#define __ellipsetool_h__

#include <qpoint.h>

#include "kis_tool.h"

class KisDoc;
class KisView;
class KisCanvas;

class EllipseTool : public KisTool
{
public:

    EllipseTool( KisDoc* _doc, KisView* _view, KisCanvas* _canvas );
    ~EllipseTool();

    virtual QString toolName() { return QString( "LineTool" ); }

    virtual void mousePress( QMouseEvent* event );
    virtual void mouseMove( QMouseEvent* event );
    virtual void mouseRelease( QMouseEvent* event );

    virtual void optionsDialog();
    
protected:

    void drawEllipse( const QPoint&, const QPoint& );

protected:

    int     lineThickness;
    int     lineOpacity;

    bool    fillSolid;
    bool    usePattern;
    bool    useGradient;
    
    QPoint  m_dragStart;
    QPoint  m_dragEnd;

    bool    m_dragging;
    
    KisCanvas   *pCanvas;
    KisDoc      *m_pDoc;
};

#endif //__linetool_h__
