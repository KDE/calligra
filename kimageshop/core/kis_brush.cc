/*
 *  kis_brush.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <elter@kde.org>
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

#include <qpoint.h>
#include <qsize.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qfileinfo.h>

#include <kimageeffect.h>
#include <ksimpleconfig.h>

#include "kis_brush.h"

KisBrush::KisBrush(QString file, bool monochrome, bool special)
  : IconItem()
{
    // set defaults
    m_valid    = false;
    m_spacing  = 3;

    // load the brush image data
    loadViaQImage(file, monochrome);

    // default hotspot
    if(!special)
        m_hotSpot = QPoint( width()/2, height()/2 );
    else
        m_hotSpot = QPoint( 0, 0 );        

    // search and load the brushinfo file
    if(!special)
    {
        QFileInfo fi(file);
        file = fi.dirPath() + "/" + fi.baseName() + ".brushinfo";
        fi.setFile(file);
        if (fi.exists() && fi.isFile())
            readBrushInfo(file);
    }            
}


KisBrush::~KisBrush()
{
    delete [] m_pData;
    delete m_pPixmap;
}


void KisBrush::readBrushInfo(QString file)
{
    KSimpleConfig config(file, true);

    config.setGroup("General");
    int spacing = config.readNumEntry("Spacing", m_spacing);
    int hotspotX = config.readNumEntry("hotspotX", m_hotSpot.x());
    int hotspotY = config.readNumEntry("hotspotY", m_hotSpot.y());

    if (spacing > 0)
        m_spacing = spacing;

    m_hotSpot = QPoint(hotspotX, hotspotY);
}


/*
    why grayscale brushes?  don't make sense
*/

void KisBrush::loadViaQImage(QString file, bool monochrome)
{
    // load via QImage
    QImage img(file);

    if (img.isNull())
    {
        m_valid = false;
        qDebug("Failed to load brush: %s", file.latin1());
    }

    img = img.convertDepth(32);
    
    if(monochrome)
        img = KImageEffect::toGray(img, true); // jwc no grayscale!!!

    // create pixmap for preview dialog
    m_pPixmap = new QPixmap;
    m_pPixmap->convertFromImage(img, QPixmap::AutoColor);

    m_w = img.width();
    m_h = img.height();

    m_pData = new uchar[m_h * m_w];

    uint *p;

    for (int h = 0; h < m_h; h++)
    {
        p = (QRgb*)img.scanLine(h);
        
        for (int w = 0; w < m_w; w++)
	    {
	        // no need to use qGray here, we have converted the image 
            // to grayscale already 
            if(monochrome)
	            m_pData[m_w * h + w] = 255 - qRed(*(p+w)); 
            else    
	            m_pData[m_w * h + w] = *(p+w); // jwc            
	    }      
    }
 
    m_valid = true;
    
    // qDebug("Loading brush: %s",file.latin1());
}


QPixmap& KisBrush::pixmap() const 
{
    return *m_pPixmap;
}


void KisBrush::setHotSpot(QPoint pt)
{
    int x = pt.x();
    int y = pt.y();

    if (x < 0) x = 0;
    else if (x >= m_w) x = m_w-1;

    if (y < 0) y = 0;
    else if (y >= m_h) y = m_h-1;
  
    m_hotSpot = QPoint(x,y);
}


uchar KisBrush::value(int x, int y) const
{
    return m_pData[m_w * y + x];
}


uchar* KisBrush::scanline(int i) const
{
    if (i < 0) i = 0;
    if (i >= m_h) i = m_h-1;
        
    return (m_pData + m_w * i);
}

uchar* KisBrush::bits() const
{
    return m_pData;
}

void KisBrush::dump() const
{
    qDebug("KisBrush data:\n");

    for (int h = 0; h < m_h; h++) 
    {
        for (int w = 0; w < m_w; w++) 
        {
	        qDebug("%d", m_pData[m_w * h + w]);
	    }
    }
}

