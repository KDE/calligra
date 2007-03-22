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
        
        for(int i=0;i<3;i++)
        {
              for(int j=0;j<2;j++)
                    sizeSave[i][j]=0; 
        }
        
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


    void Chart::api(QVector<QPointF> & BCWP, QVector<QPointF> & BCWS, QVector<QPointF> & ACWP ,QVector<QPointF> & BCWP_display, QVector<QPointF> & BCWS_display, QVector<QPointF> & ACWP_display ,const int n_curve, int maximumHeight, int maximumWidth)
    {
        

        if(n_curve==ChartWidget::BCWP)
        {
            reCalculateY(BCWP,BCWP_display,ChartWidget::BCWP,maximumHeight);
            reCalculateX(BCWP,BCWP_display,ChartWidget::BCWP,maximumWidth);
        }
        if(n_curve==ChartWidget::BCWS)
        {
            reCalculateY(BCWS,BCWS_display,ChartWidget::BCWS,maximumHeight);
            reCalculateX(BCWS,BCWS_display,ChartWidget::BCWS,maximumWidth);
        }
        if(n_curve==ChartWidget::ACWP)
        {
            //calculatePlannedCost();
            reCalculateY(ACWP,ACWP_display,ChartWidget::ACWP,maximumHeight);
            //timeToPercent(ACWP);
            reCalculateX(ACWP,ACWP_display,ChartWidget::ACWP,maximumWidth);
        }
    }


    /* Calculate the new value of every Y-axis when the window hab been re-sized */
    void Chart::reCalculateY(QVector<QPointF> & vect, QVector<QPointF> & vect_display,const int n_curve, int maximumHeight)// WORKS, TESTED
    {    
        float inverse;
        float tmp;
        
        if(maximumHeight != sizeSave[n_curve][1])
        {
    
            QVector<QPointF>::iterator it= vect.begin();
            QVector<QPointF>::iterator it_display= vect_display.begin();
            while (it != vect.end())
            {
                inverse = totalYPercent - it->y();
                tmp=(maximumHeight - ChartWidget::BOTTOMMARGIN - ChartWidget::TOPMARGIN)*inverse/100;
                kDebug()<<"reCalculateY Y:"<<tmp<<endl;
                it_display->setY(tmp+ChartWidget::TOPMARGIN);
                it++;it_display++;
            }
            sizeSave[n_curve][1]=maximumHeight;
        // Save to not doing all the work next time if the data are already the same
        }
    }
    /* Calculate the new value of X-axis when the window had been re-sized */
    void Chart::reCalculateX(QVector<QPointF> & vect, QVector<QPointF> & vect_display,const int n_curve, int maximumWidth)// WORKS, TESTED
    {
        float tmp; 
        if(maximumWidth != sizeSave[n_curve][0])
        {
            QVector<QPointF>::iterator it= vect.begin();
            QVector<QPointF>::iterator it_display= vect_display.begin();
            while (it != vect.end())
            {
                
     
                tmp= (maximumWidth - ChartWidget::RIGHTMARGIN - ChartWidget::LEFTMARGIN)*it->x()/100;
                kDebug()<<"reCalculateX X:"<<tmp<<endl;
                it_display->setX(tmp+ChartWidget::LEFTMARGIN);
                it++;it_display++;
            }
            sizeSave[n_curve][0]=maximumWidth;
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
    kDebug()<<"TOTALyPERCENT : "<<totalYPercent<<endl;
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
            kDebug()<<"itY avant"<<it->y()<<endl;
            it->setY(it->y()*100/totalCostPlanned);
            kDebug()<<"cost apres %"<<it->y()<<endl;
            it++;
        }
    }

    /* Calculate the percentage of the time and replace the result in the vector */
    void Chart::timeToPercent(QVector<QPointF> & vect)
    {
        QVector<QPointF>::iterator it= vect.begin();
        while(it != vect.end())
        {
            kDebug()<<"itx avant"<<it->x()<<endl;
            it->setX(it->x()*100/(vect.size()-1));
            kDebug()<<"time apres %"<<it->x()<<endl;
            it++;
        }
    }

    void Chart::setMaxCost(QVector<QPointF> BCWS)
    {
        totalCostPlanned=BCWS.last().y();
    }

    void Chart::calculatePlannedCost(QVector<QPointF> & vect,QVector<QDate> weeks,Project & p)
    {
        QVector<QPointF>::iterator it= vect.begin();
        QVector<QDate>::iterator it_weeks = weeks.begin();
        float sum=0;
        it->setY(sum);
        it++;
        while(it != vect.end())
        {
            sum+=(float)p.plannedCost(*it_weeks);
            it->setY(sum);
            kDebug()<<"Planned cost pour la tache : "<<(float)p.plannedCost(*it_weeks)<<endl;
            it++;
            it_weeks++;
        }
    }

    void Chart::calculateActualCost(QVector<QPointF> & vect,QVector<QDate> weeks,Project & p)
    {
        QVector<QPointF>::iterator it= vect.begin();
        QVector<QDate>::iterator it_weeks = weeks.begin();
	float sum=0;
        it->setY(sum);
        it++;
        while(it != vect.end())
        {
            sum+=(float)p.actualCost(*it_weeks);
            it->setY(sum);
            kDebug()<<"Acutal cost : "<<it->y()<<endl;
            it++;
            it_weeks++;
        }
    }

    void Chart::initXCurvesVectors(QVector<QDate> weeks,QVector<QPointF> & BCWP, QVector<QPointF> & BCWS, QVector<QPointF> & ACWP )
    {
            for(int i=0;i<weeks.size();i++)
            {
                BCWP.push_back(QPointF(i,0));
                BCWS.push_back(QPointF(i,0));
                ACWP.push_back(QPointF(i,0));
                kDebug()<<"Init : Nb semaines :"<<i<<endl;
            }
    }

    void Chart::calculateWeeks(QVector<QDate> & weeks,Project & p)
    {
            QDate myDate = p.startTime().date();
            while(myDate < p.endTime().date())
            {
                kDebug()<<"Calcul weeks : "<<myDate<<endl;
                weeks.push_back(myDate);
                myDate=myDate.addDays(7);
                
            }
            weeks.push_back(p.endTime().date());
            kDebug()<<"Calcul weeks fin : "<<p.endTime().date()<<endl;
    }
}
