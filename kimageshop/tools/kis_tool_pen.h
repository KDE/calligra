/*
 *  kis_tool_pen.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter
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

#ifndef __pentool_h__
#define __pentool_h__

#include <qpoint.h>
#include "kis_view.h"
#include "kis_canvas.h"
#include "kis_tool.h"

class KisBrush;
class KisFrameBuffer;

class PenTool : public KisTool
{
 public:

    PenTool(KisDoc *doc, KisView *view, KisCanvas *canvas, KisBrush *_brush);
    ~PenTool();
  
    QString toolName() { return QString("BrushTool"); }

    void setBrush(KisBrush *_brush);
    bool paint(QPoint pos);

 public slots:

    virtual void mousePress(QMouseEvent*); 
    virtual void mouseMove(QMouseEvent*);
    virtual void mouseRelease(QMouseEvent*);

  virtual void optionsDialog();

 protected:

    QPoint 	        m_dragStart;
    bool   	        m_dragging;
    float           m_dragdist;
    
    KisBrush        *m_pBrush;
    KisView         *m_pView;
    KisCanvas       *m_pCanvas;
    KisFrameBuffer  *fb;
    
    QPen pen;
    int  penW;
    QPixmap *buffer;
    
    // tool options
    
    bool usePattern;
    bool useGradient;
    int lineThickness;
    int lineOpacity;
    
    int penColorThreshold;
    int penOpacity;
    bool penPattern;
};

#endif //__pentool_h__

