
/*
 *  kis_tool_brush.h - part of KImageShop
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

#ifndef __pastetool_h__
#define __pastetool_h__

#include <qpoint.h>

#include "kis_selection.h"
#include "kis_tool.h"

class KisSelection;

class PasteTool : public KisTool
{
 public:
    PasteTool(KisDoc *doc, KisView *view, const KisSelection *_selection);
    ~PasteTool();
  
    QString toolName() { return QString("PasteTool"); }
    void setOpacity(int opacity);
    bool pasteMonochrome(QPoint pos);
    bool pasteColor(QPoint pos);

 public slots:
 
    virtual void mousePress(QMouseEvent*); 
    virtual void mouseMove(QMouseEvent*);
    virtual void mouseRelease(QMouseEvent*);

 protected:
 
    /* contains selection rectangle definition, status, etc. 
    This will also contain effects and raster operations to
    be performed on selection and/or area pasted to */
    const KisSelection	*m_pSelection; 
    KisView     *m_pView;
    QPoint 	m_dragStart;
    bool        m_dragging;
    float       m_dragdist;
};

#endif //__brushtool_h__
