/* This file is part of the KDE projectbotto
   Copyright (C) 2007 Nicolas MICAS <nicolas.micas@gmail.com> and Laure Colbrant <laure.colbrant@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "kptchart.h"
#include "kptchartpanel.h"

#include <klocale.h>
#include <cmath>
#include <kdebug.h>

namespace KPlato
{


    Chart::Chart()
    {
        totalYPercent = 0.0;
        //init totalBudget
    }

    void Chart::init_display(QVector<QPointF> & BCWP_display, QVector<QPointF> & BCWS_display, QVector<QPointF> & ACWP_display, int nb_weeks )
    {
        int i=0;
        while (i<nb_weeks)
        {
            BCWP_display.push_back(QPointF(0,0));
            BCWS_display.push_back(QPointF(0,0));
            ACWP_display.push_back(QPointF(0,0));
            i++;
        }
    }

    void Chart::api(QVector<QPointF> &data, QVector<QPointF> &display, const QSize &size )
    {
        kDebug()<<"api() :"<<data<<", "<<display<<endl;
        reCalculateY( data, display, size.height() );
        reCalculateX( data, display, size.width() );
        kDebug()<<"api():"<<data<<", "<<display<<endl;
    }


    /* Calculate the new value of every Y-axis when the window hab been re-sized */
    void Chart::reCalculateY(QVector<QPointF> & vect, QVector<QPointF> & vect_display, int maximumHeight)// WORKS, TESTED
    {    
        float inverse;
        float tmp;
        
        kDebug()<<k_funcinfo<<maximumHeight<<endl;
        QVector<QPointF>::iterator it= vect.begin();
        QVector<QPointF>::iterator it_display= vect_display.begin();
        while (it != vect.end())
        {
            inverse = totalYPercent - it->y();
            tmp=((maximumHeight - ChartWidget::BOTTOMMARGIN - ChartWidget::TOPMARGIN)*inverse)/100.0;
            it_display->setY(tmp+ChartWidget::TOPMARGIN);
            it++;it_display++;
        }
    }
    /* Calculate the new value of X-axis when the window had been re-sized */
    void Chart::reCalculateX(QVector<QPointF> & vect, QVector<QPointF> & vect_display, int maximumWidth)// WORKS, TESTED
    {
        float tmp; 
        //kDebug()<<k_funcinfo<<maximumWidth<<endl;
        QVector<QPointF>::iterator it= vect.begin();
        QVector<QPointF>::iterator it_display= vect_display.begin();
        while (it != vect.end())
        {
            tmp= ((maximumWidth - ChartWidget::RIGHTMARGIN - ChartWidget::LEFTMARGIN)*it->x())/100.0;
            it_display->setX(tmp+ChartWidget::LEFTMARGIN);
            it++;it_display++;
        }
    }

    // Set a variable to know how much is the higher percent of Y 
    float Chart::setMaxYPercent(QVector<QPointF> BCWP, QVector<QPointF> BCWS, QVector<QPointF> ACWP )// WORKS, TESTED
    {
        totalYPercent = 0.0;

        if( !BCWP.isEmpty())
        {
            if( !BCWS.isEmpty() )
            {
                if( (BCWP.last()).y() <=  BCWS.last().y() )
                {
        
                    if( !ACWP.isEmpty() )
                    {
                            if(   (BCWS.last()).y() <=  (ACWP.last()).y() )
                            {
                                totalYPercent=(ACWP.last()).y();
                            }
                            else
                            {
                                totalYPercent=(BCWS.last()).y();
                            }
                    }
                    else
                    {
                        totalYPercent=(BCWS.last()).y();
                    }
                }
                else
                {
                    if( !ACWP.isEmpty() )
                    {
                            if( (BCWP.last()).y()  <= (ACWP.last()).y() )
                            {
                                totalYPercent=(ACWP.last()).y();
                            }
                            else
                            {
                                totalYPercent=(BCWP.last()).y();
                            }
                    }
                    else
                    {
                        totalYPercent=(BCWP.last()).y();
                    }
                }
            }
            else
            {
                if( !ACWP.isEmpty() )
                {
                        if( (BCWP.last()).y()  <= (ACWP.last()).y() )
                        {
                            totalYPercent=(ACWP.last()).y();
                        }
                        else
                        {
                            totalYPercent=(BCWP.last()).y();
                        }
                }
                else
                {
                    totalYPercent=(BCWP.last()).y();
                }
            }
        }
        else
        {
            if( !ACWP.isEmpty() )
            {
                    if(   (BCWS.last()).y() <=  (ACWP.last()).y() )
                    {
                        totalYPercent=(ACWP.last()).y();
                    }
                    else
                    {
                        totalYPercent=(BCWS.last()).y();
                    }
            }
            else
            {
                totalYPercent=(BCWS.last()).y();
            }
        }
    kDebug()<<"TOTAL PERCENT : "<<totalYPercent<<endl;
    return(totalYPercent);
    }

    

  /*  void Chart::setMaxTime(QVector<QPointF> weeks)
    {
        totalTimePlanned=weeks.last();
    }*/

    /* Calculate the percentage of the cost and replace the result in the vector */
    void Chart::costToPercent(QVector<QPointF> & vect)
    {
        QVector<QPointF>::iterator it= vect.begin();
        while(it != vect.end())
        {
            //kDebug()<<"itY before"<<it->y()<<endl;
            if (totalCostPlanned > 0.0) { // avoid divide by 0
                it->setY(it->y()*100/totalCostPlanned);
            } else {
                it->setY(0);
            }
            //kDebug()<<"cost after %"<<it->y()<<endl;
            it++;
        }
    }

    /* Calculate the percentage of the time and replace the result in the vector */
    void Chart::timeToPercent(QVector<QPointF> & vect)
    {
        QVector<QPointF>::iterator it= vect.begin();
        while(it != vect.end())
        {
            //kDebug()<<"itx before"<<it->x()<<endl;
            if (vect.size() > 1 ) { // avoid divide by 0
                it->setX(it->x()*100/(vect.size()-1));
            } else {
                it->setX(0);
            }
            //kDebug()<<"time after %"<<it->x()<<endl;
            it++;
        }
    }

    void Chart::setMaxCost(QVector<QPointF> BCWS)
    {
        totalCostPlanned=BCWS.last().y();
    }

    void Chart::calculatePlannedCost(QVector<QPointF> & vect,QVector<QDate> weeks,Project & p)
    {
        //kDebug()<<"calculatePlannedCost() "<<weeks<<", "<<vect<<endl;
        QVector<QPointF>::iterator it= vect.begin();
        QVector<QDate>::iterator it_weeks = weeks.begin();
        float sum=0;
        it->setY(sum);
        it++;
        while(it != vect.end())
        {
            for (int i = 0; i < 7; ++i ) {
                QDate date = (*it_weeks).addDays(i);
                sum+=(float)p.plannedCost(date, p.currentViewScheduleId());
                if ( date == weeks.last() ) {
                    break;
                }
            }
            it->setY(sum);
            if ( *it_weeks == weeks.last() ) {
                break; // The last entry is the last date, not the start of a week!
            }
            it++;
            it_weeks++;
        }
        //kDebug()<<"calculatePlannedCost() "<<vect<<endl;
    }

    void Chart::calculateActualCost(QVector<QPointF> & vect,QVector<QDate> weeks,Project & p)
    {
        //kDebug()<<"calculateActualCost() "<<weeks<<vect<<endl;
        QVector<QPointF>::iterator it= vect.begin();
        QVector<QDate>::iterator it_weeks = weeks.begin();
        float sum=0;
        it->setY(sum);
        it++;
        while(it != vect.end())
        {
            for (int i = 0; i < 7; ++i ) {
                QDate date = (*it_weeks).addDays(i);
                sum+=(float)p.actualCost((*it_weeks).addDays(i));
                if ( date == weeks.last() ) {
                    break;
                }
            }
            it->setY(sum);
            if ( *it_weeks == weeks.last() ) {
                break; // The last entry is the last date, not the start of a week!
            }
            it++;
            it_weeks++;
        }
        //kDebug()<<"calculateActualCost() "<<vect<<endl;
    }

    void Chart::calculateBCWP(QVector<QPointF> & vect,QVector<QDate> weeks,Project & p)
    {
        kDebug()<<"calculateBCWP() "<<weeks<<vect<<endl;
        QVector<QPointF>::iterator it= vect.begin();
        QVector<QDate>::iterator it_weeks = weeks.begin();
        float sum=0;
        it->setY(sum);
        it++;
        while(it != vect.end())
        {
            sum = (float)p.bcwp((*it_weeks), p.currentViewScheduleId()); // up to date
            it->setY(sum);
            it++;
            it_weeks++;
        }
        kDebug()<<"calculateBCWP() "<<vect<<endl;
    }

    void Chart::initXCurvesVectors(QVector<QDate> weeks,QVector<QPointF> & BCWP, QVector<QPointF> & BCWS, QVector<QPointF> & ACWP )
    {
        for(int i=0;i<weeks.size();i++)
        {
            BCWP.push_back(QPointF(i,0));
            BCWS.push_back(QPointF(i,0));
            ACWP.push_back(QPointF(i,0));
            //kDebug()<<"Init : Number of weeks :"<<i<<endl;
        }
    }

    void Chart::calculateWeeks(QVector<QDate> & weeks,Project & p)
    {
        //Schedule *s = p.findSchedule( p.currentViewScheduleId() );
        //kDebug()<<k_funcinfo<<weeks.count()<<" Schedule: "<<(s==0?"None":s->name()+QString(", %1").arg(s->type()))<<endl;
        QDate myDate = p.startTime( p.currentViewScheduleId() ).date();
        while(myDate < p.endTime( p.currentViewScheduleId() ).date())
        {
            weeks.push_back(myDate);
            myDate=myDate.addDays(7);
        }
        weeks.push_back(p.endTime( p.currentViewScheduleId() ).date());
        for ( int i = 0; i < weeks.count(); ++i ) {
            kDebug()<<i<<": "<<weeks[i]<<endl;
        }
    }
}
