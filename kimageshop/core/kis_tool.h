/*
 *  kis_tool.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
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

#ifndef __kis_tool_h__
#define __kis_tool_h__

#include <qobject.h>
#include <qcursor.h>

class KisDoc;
class KisView;

class KisTool : public QObject
{
  Q_OBJECT

 public:
    KisTool(KisDoc *doc, KisView *view = 0L);
    ~KisTool();

    QString toolName();
    QCursor cursor();
    void setCursor( const QCursor& );
    
    virtual void optionsDialog();
    virtual void clearOld(){}
    
 public slots:

    virtual void mousePress(QMouseEvent*); 
    virtual void mouseMove(QMouseEvent*);
    virtual void mouseRelease(QMouseEvent*);
    
 protected:

    inline int zoomed(int n);
    inline int zoomedX(int n);  
    inline int zoomedY(int n);
    inline QPoint zoomed(QPoint & point);
  
    KisDoc  *m_pDoc;
    KisView *m_pView;
    QCursor  m_Cursor;
};

#endif

