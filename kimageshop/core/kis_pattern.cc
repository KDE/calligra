/*
 *  kis_pattern.cc - part of Krayon
 *
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
 *                2001 John Califf
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
#include <kdebug.h>

#include "kis_pattern.h"

#define THUMB_SIZE 30


KisPattern::KisPattern(QString file)
  : KisKrayon()
{
    m_valid    = false;
    m_spacing  = 3;
    loadViaQImage(file);
}

KisPattern::KisPattern(int formula)
  : KisKrayon()
{
    m_valid = false;
    validThumb = false;
    m_spacing  = 3;    
    loadViaFormula(formula);
}

KisPattern::~KisPattern()
{
    delete m_pImage;
    delete m_pPixmap;
}

void KisPattern::loadViaQImage(QString file)
{
    // load via QImage
    m_pImage = new QImage(file);
    m_pImage->setAlphaBuffer(true);
    
    if (m_pImage->isNull())
    {
        m_valid = false;
        qDebug("Failed to load pattern: %s", file.latin1());
    }

    *m_pImage = m_pImage->convertDepth(32);
    
    // create pixmap for preview dialog
    m_pPixmap = new QPixmap;
    QImage img = *m_pImage;
    m_pPixmap->convertFromImage(img, QPixmap::AutoColor);

    // scale a pixmap for iconview cell to size of cell
    if(img.width() > THUMB_SIZE || img.height() > THUMB_SIZE)
    {
        QPixmap filePixmap;
        filePixmap.load(file);
        QImage fileImage = filePixmap.convertToImage();
        
        m_pThumbPixmap = new QPixmap;
    
        int xsize = THUMB_SIZE;
        int ysize = THUMB_SIZE;
        int picW  = fileImage.width();
        int picH  = fileImage.height(); 
        
        if(picW > picH)
        {
            float yFactor = (float)((float)(float)picH/(float)picW);
            ysize = (int)(yFactor * (float)THUMB_SIZE);
            kdDebug() << "ysize is " << ysize << endl;
            if(ysize > 30) ysize = 30;
        }
        else if(picW < picH)
        {
            float xFactor = (float)((float)picW/(float)picH);
            xsize = (int)(xFactor * (float)THUMB_SIZE);
            kdDebug() << "xsize is " << xsize << endl;            
            if(xsize > 30) xsize = 30;            
        }
        
        QImage thumbImg = fileImage.smoothScale(xsize, ysize);
        
        if(!thumbImg.isNull())
        {
            m_pThumbPixmap->convertFromImage(thumbImg);
            if(!m_pThumbPixmap->isNull())
            {
                validThumb = true;
            }    
        }
    } 
       
    m_w = m_pImage->width();
    m_h = m_pImage->height();
 
    m_valid = true;
    qDebug("Loading pattern: %s",file.latin1());
}

/*
    load pattern from a formula or algorithm - these will
    algorithms and/or predefined Qt patterns 
*/

/*
    Formulas for patterns should come from plugins which
    could be written in almost any language to generate
    patterns in a given area or region
*/
void KisPattern::loadViaFormula(int formula)
{
    // load via QImage
    m_pImage = new QImage(64, 64, 32);

    if (m_pImage->isNull())
    {
        m_valid = false;
        qDebug("Failed to load pattern: %d", formula);
    }

    *m_pImage = m_pImage->convertDepth(32);

    // create pixmap for preview dialog
    m_pPixmap = new QPixmap;
    m_pPixmap->convertFromImage(*m_pImage, QPixmap::AutoColor);

    switch(formula)
    {
        default:
            m_pPixmap->fill(Qt::white);        
            break;    
    }

    m_w = m_pImage->width();
    m_h = m_pImage->height();
 
    m_valid = true;
    qDebug("Loading pattern: %d", formula);
}


