/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef KPTTIMESCALE_H
#define KPTTIMESCALE_H

#include "kptduration.h"

#include <qptrlist.h>
#include <qlabel.h>
#include <qcanvas.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qdatetime.h>

class KPTNode;

class QString;
class QWidget;
class QHBox;
class QVBox;

class KPTTimeScale : public QCanvasView
{
    Q_OBJECT

public:
    KPTTimeScale( QWidget *parent, const KPTDuration &startTime,  const KPTDuration &endTime, int height );
    KPTTimeScale( QWidget *parent, KPTDuration *startTime,  KPTDuration *duration, int height );
    ~KPTTimeScale();
    
    enum Format { Day, Week, Month };
    
    int totalWidth() { return m_totalWidth; }

    int getX(const KPTDuration* time);
    int getWidth(const KPTDuration* dur);
    
    void draw(const KPTDuration &startTime,  const KPTDuration &endTime, int height );
    void draw();
    
public slots:
    void slotSetContents(int x, int /*y*/);
    
protected:
    int calcTotalWidth();
    
private:
    void init();
    void addWeekDays(int x, int size);
    void clear();
    
    QCanvas *m_canvas;

    int m_defaultFontSize;
    QVBox *m_timescale;
    int m_totalWidth;
    int m_height;
    QHBox *m_topBox, *m_bottomBox;
    QPtrList<QLabel> m_topLabels;
    QPtrList<QLabel> m_bottomLabels;
    
    QDateTime m_startTime;
    QDateTime m_endTime;
    Format m_unit;
    int m_pointsPrUnit;

};

#endif
