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
   
#include "kpttimescale.h"
#include "kptduration.h"
#include "kptnode.h"

#include <qhbox.h>
#include <qfont.h>
#include <qlabel.h>
#include <qframe.h>
#include <qobjectlist.h>

#include <kdebug.h>

#include <koRect.h> // DEBUGRECT

KPTTimeScale::KPTTimeScale( QWidget *parent, const KPTDuration &startTime,  const KPTDuration &endTime, int height )
    : QCanvasView ( parent, "Timescale view"),
    m_height(height),
    m_unit(KPTTimeScale::Day)
{
    m_startTime = startTime.dateTime();
    m_endTime = endTime.dateTime();
    
    kdDebug()<<k_funcinfo<<" Start: "<<m_startTime.toString()<<" end: "<<m_endTime.toString()<<endl;
    init();
}
KPTTimeScale::KPTTimeScale( QWidget *parent, KPTDuration *startTime,  KPTDuration *duration, int height )
    : QCanvasView ( parent ),
    m_height(height),
    m_unit(KPTTimeScale::Day)
{ 
    m_startTime = startTime->dateTime();
    m_endTime = duration->dateTime();
    m_endTime.setDate(m_startTime.date());
    init();   
}

KPTTimeScale::~KPTTimeScale()
{}

void KPTTimeScale::init()
{
    m_canvas = new QCanvas( this, "Timescale canvas" );
    setCanvas( m_canvas );
   
    m_startTime = m_startTime.addDays( 1 - m_startTime.date().dayOfWeek()); // start of week
    m_endTime = m_startTime.addDays( 7 - m_endTime.date().dayOfWeek()); // end of week
    kdDebug()<<k_funcinfo<<"Start="<<m_startTime.toString()<<endl;
    kdDebug()<<k_funcinfo<<"End="<<m_endTime.toString()<<endl;
    
    m_topBox = 0;
    m_bottomBox = 0;
    m_pointsPrUnit = 36;
        
    setMaximumHeight( m_height );
    setMinimumHeight( m_height );
    setHScrollBarMode(QScrollView::AlwaysOff);
    setVScrollBarMode(QScrollView::AlwaysOff);
    setBackgroundMode ( PaletteButton  );
    m_totalWidth = calcTotalWidth();

}

void KPTTimeScale::slotSetContents(int x, int /*y*/)
{
    kdDebug()<<k_funcinfo<<" x="<<x<<endl;
    setContentsPos(x, contentsY());
}

int KPTTimeScale::getX(const KPTDuration* time)
{
    //kdDebug()<<k_funcinfo<<"Start: "<<m_startTime.toString()<<endl;
    //kdDebug()<<k_funcinfo<<"Time: "<<time->dateTime().toString()<<endl;
    int st = m_startTime.secsTo(time->dateTime());
    int x = 0;
    switch (m_unit)
    {
    case KPTTimeScale::Day:
        x = (st * m_pointsPrUnit)  / 86400; // 24 hours
        break;
    }
    return x;
}

int KPTTimeScale::getWidth(const KPTDuration* dur)
{
    int d = dur->duration() * m_pointsPrUnit / 86400; 
    return d;
}

int KPTTimeScale::calcTotalWidth()
{
    int tot = 300;
    switch ( m_unit )
    {
    case KPTTimeScale::Day:
    {
        int t = m_startTime.daysTo(m_endTime);
        tot = QMAX( (m_pointsPrUnit * 7), (t * m_pointsPrUnit) );
        kdDebug()<<k_funcinfo<<"Days="<<t<<endl;
        break;
    }
    default:
        break;
    }
    kdDebug()<<k_funcinfo<<" total width= "<<tot<<endl;
    return tot;
}

void KPTTimeScale::draw(const KPTDuration &startTime,  const KPTDuration &endTime, int height )
{
    kdDebug()<<k_funcinfo<<" start="<<startTime.dateTime().toString()<<" end="<<endTime.dateTime().toString()<<endl;
    m_startTime = startTime.dateTime();
    m_startTime = m_startTime.addDays( 1 - m_startTime.date().dayOfWeek()); // start of week
    m_endTime = endTime.dateTime();
    m_endTime = m_endTime.addDays( 7 - m_endTime.date().dayOfWeek()); // end of week

    m_height = height;
    m_totalWidth = calcTotalWidth();
    m_canvas->resize( m_totalWidth+2, m_height );
    draw();
}

void KPTTimeScale::draw()
{
    kdDebug()<<k_funcinfo<<endl;
    clear();
    // Line between date (top) days (bottom)
    QCanvasLine *l = new QCanvasLine(m_canvas);
    l->setPen( QPen(Qt::black) );
    l->setZ(50);
    l->setPoints(0, m_height/2-1, m_totalWidth, m_height/2-1);
    l->show();
    
    QFont f( QString("Helvetica"), m_height/2 - 3);
    QDateTime t = m_startTime;
    int x = getX(&KPTDuration(t));
    do
    {
        kdDebug()<<k_funcinfo<<" time="<<t.date().toString()<<" x="<<x<<" width="<<m_totalWidth<<endl;
        QCanvasText *text = new QCanvasText(t.date().toString(),m_canvas);
        text->setX(x+2);
        text->setY(0);
        text->setFont(f);
        text->show();
        addWeekDays(x, m_pointsPrUnit);
        t = t.addDays(7);
        
        x = getX(&KPTDuration(t));
        l = new QCanvasLine(m_canvas);
        l->setZ(50);
        l->setPoints(x, 0, x, m_height/2-1);
        l->show();
    } while (t < m_endTime);
    
    update();
    kdDebug()<<k_funcinfo<<" end"<<endl;
}

void KPTTimeScale::addWeekDays(int x, int size)
{
    kdDebug()<<k_funcinfo<<endl;
    QFont f( QString("Helvetica"), m_height/2 - 3);
    QString days = "MTOTFLS";
    
    int i=0;
    while ( i < 7)
    {
        QCanvasText *text = new QCanvasText(QString(days[i]),m_canvas);
        text->setX(x + i*size + 2);
        text->setY(m_height/2-1);
        text->setFont(f);
        text->show();
        
        i++;
        QCanvasLine *l = new QCanvasLine(m_canvas);
        l->setZ(50);
        l->setPoints(x+i*size, m_height, x+i*size, m_height/2);
        l->show();        
    }
}

void KPTTimeScale::clear()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) 
    {
        if ( *it )
            delete *it;
    }
}

#include "kpttimescale.moc"
