/*
 *  colorpicker.h - part of KImageShop
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

#ifndef __filltool_h__
#define __filltool_h__

#include <qpoint.h>

#include "kis_color.h"
#include "kis_view.h"
#include "kis_tool.h"
#include "kis_layer.h"

class Fill : public KisTool
{
  public:
    Fill(KisDoc *doc, KisView *view);
    ~Fill();
  
    QString toolName() { return QString("Fill"); }
    bool flood(int startx, int starty);

  public slots:
    virtual void mousePress(QMouseEvent*); 
  
  protected:
    int checkTouching(KisLayer *lay, int x, int y, int r, int g, int b);
    void drawFlood(KisLayer *lay, QRect & layerRect, int x, int y);
    
    // new colors (desired)
    int nRed;
    int nGreen;
    int nBlue;

    // source colors (existing)
    int sRed;
    int sGreen;
    int sBlue;
    
};
#endif //__filltool_h__

