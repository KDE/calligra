/*
 *  kis_pattern.h - part of KImageShop
 *
 *  Copyright (c) 2000 Matthias Elter  <elter@kde.org>
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

#ifndef __kis_pattern_h__
#define __kis_pattern_h__

#include <qsize.h>

#include "iconitem.h"

class QPoint;
class QPixmap;
class QImage;

class KisPattern : public IconItem
{
    public:
        KisPattern(QString file);
        KisPattern(int formula);
        virtual ~KisPattern();

        bool      isValid()	const { return m_valid; }
        QPixmap&  pixmap()  const;
        QImage*   image()   const;
        QSize     size()    const { return QSize(m_w, m_h); } 
        int       width()   const { return m_w; }
        int       height()  const { return m_h; }

    private:
        void      loadViaQImage(QString file);
        void      loadViaFormula(int formula);
        bool      m_valid;
        int       m_w, m_h;

        QImage   *m_pImage;
        QPixmap  *m_pPixmap;
};

#endif

