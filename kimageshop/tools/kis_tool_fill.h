/*
 *  kis_tool_fill.h - part of Krayon
 *
 *  Copyright (c) 2000 John Califf
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

struct fillinfo
{
   int left;
   int right;
   int top;
   int bottom;
   unsigned char or, og, ob, r, g, b;
};

struct fillpixelinfo
{
   int y, xl, xr, dy;
};

class Fill : public KisTool
{
  public:
    Fill(KisDoc *doc, KisView *view);
    ~Fill();
  
    QString toolName() { return QString("Fill Tool"); }
    bool flood(int startX, int startY);
    
  public slots:
    virtual void mousePress(QMouseEvent*); 
  
  protected:

    // from gpaint
    int is_old_pixel_value(struct fillinfo *info, int x, int y);  
    void set_new_pixel_value(struct fillinfo *info, int x, int y);      
    void flood_fill(struct fillinfo *info, int x, int y);
    void seed_flood_fill( int x, int y, QRect & frect );    
                
    // new colors (desired)
    int nRed;
    int nGreen;
    int nBlue;

    // source colors (existing)
    int sRed;
    int sGreen;
    int sBlue;

    KisLayer *fLayer;
};

#endif //__filltool_h__

