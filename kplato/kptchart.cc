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
        for(int i=0;i<3;i++)
        {
              for(int j=0;j<2;j++)
                    sizeSave[i][j]=0; // CRASH KPLATO
        }
        //init totalBudget
    }

    void Chart::api(QVector<QPointF> & BCWP, QVector<QPointF> & BCWS, QVector<QPointF> & ACWP ,const int n_curve, int maximumHeight, int maximumWidth)
    {
    
        if(n_curve==ChartWidget::BCWP)
        {

            costToPercent(BCWP);
            timeToPercent(BCWP);

            reCalculateY(BCWP,ChartWidget::BCWP,maximumHeight);
            reCalculateX(BCWP,ChartWidget::BCWP,maximumWidth);
        }
        if(n_curve==ChartWidget::BCWS)
        {
            costToPercent(BCWS);
            timeToPercent(BCWS);

            reCalculateY(BCWS,ChartWidget::BCWS,maximumHeight);
            reCalculateX(BCWS,ChartWidget::BCWS,maximumWidth);
        }
        if(n_curve==ChartWidget::ACWP)
        {
            //calculatePlannedCost();
            costToPercent(ACWP);
            reCalculateY(ACWP,ChartWidget::ACWP,maximumHeight);
            //timeToPercent(ACWP);
            reCalculateX(ACWP,ChartWidget::ACWP,maximumWidth);
        }
    }


    /* Calculate the new value of every Y-axis when the window hab been re-sized */
    void Chart::reCalculateY(QVector<QPointF> & vect,const int n_curve, int maximumHeight)// WORKS, TESTED
    {    
        float inverse;
        float tmp;
        
        if(maximumHeight != sizeSave[n_curve][1])
        {
    
            QVector<QPointF>::iterator it= vect.begin();
            while (it != vect.end())
            {
                inverse = totalYPercent - it->y();
                tmp=(maximumHeight - ChartWidget::BOTTOMMARGIN - ChartWidget::TOPMARGIN)*inverse/100;
                it->setY(tmp+ChartWidget::TOPMARGIN);
                it++;
            }
            sizeSave[n_curve][1]=maximumHeight;
        // Save to not doing all the work next time if the data are already the same
        }
    }

    /* Calculate the new value of X-axis when the window had been re-sized */
    void Chart::reCalculateX(QVector<QPointF> & vect,const int n_curve, int maximumWidth)// WORKS, TESTED
    {
        float tmp; 
        if(maximumWidth != sizeSave[n_curve][0])
        {
            QVector<QPointF>::iterator it= vect.begin();
            while (it != vect.end())
            {
                tmp= (maximumWidth - ChartWidget::RIGHTMARGIN - ChartWidget::LEFTMARGIN)*it->x()/100;
                it->setX(tmp+ChartWidget::LEFTMARGIN);
                it++;
            }
            sizeSave[n_curve][0]=maximumWidth;

        }
    }

    // Set a variable to know how much is the higher percent of Y 
    float Chart::setMaxYPercent(QVector<QPointF> BCWP, QVector<QPointF> BCWS, QVector<QPointF> ACWP )// WORKS, TESTED
    {
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
    return(totalYPercent);
    }

    void Chart::setMaxCost(QVector<QPointF> BCWP)
    {
        totalCostPlanned=BCWP.last().y();
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
            it->setY(it->y()*100/totalCostPlanned);
            it++;
        }
    }

    /* Calculate the percentage of the time and replace the result in the vector */
    void Chart::timeToPercent(QVector<QPointF> & vect)
    {
        QVector<QPointF>::iterator it= vect.begin();
        while(it != vect.end())
        {
            it->setX(it->x()*100/vect.size());
            it++;
        }
    }

    void Chart::calculatePlannedCost(QVector<QPointF> & vect,QVector<QDate> weeks,Project & p)
    {
        QVector<QPointF>::iterator it= vect.begin();
        QVector<QDate>::iterator it_weeks = weeks.begin();
            while(it != vect.end())
            {
                it->setX((float)p.plannedCostTo(*it_weeks));
                it++;
                it_weeks++;
                kDebug()<<"Planned cost"<<endl;
            }
    }

    void Chart::calculateActualCost(QVector<QPointF> & vect,QVector<QDate> weeks,Project & p)
    {
        QVector<QPointF>::iterator it= vect.begin();
        QVector<QDate>::iterator it_weeks = weeks.begin();
            while(it != vect.end())
            {
                it->setX((float)p.actualCostTo(*it_weeks));
                it++;
                it_weeks++;
                kDebug()<<"Acutal cost"<<endl;
            }
    }

    void Chart::initXCurvesVectors(QVector<QDate> weeks,QVector<QPointF> & BCWP, QVector<QPointF> & BCWS, QVector<QPointF> & ACWP )
    {
            for(int i=0;i<weeks.size();i++)
            {
                BCWP.push_back(QPointF(i,0));
                BCWS.push_back(QPointF(i,0));
                ACWP.push_back(QPointF(i,0));
                kDebug()<<"Init"<<endl;
            }
    }

    void Chart::calculateWeeks(QVector<QDate> & weeks,Project & p)
    {
            QDate myDate = p.startTime().date();
            while(myDate < p.endTime().date())
            {
                weeks.push_back(myDate);
                myDate=myDate.addDays(7);
                kDebug()<<"Calcul weeks"<<endl;
            }
            weeks.push_back(p.endTime().date());
    }

// function : Time : vect.pushback();


    /*Caculate the actual cost of the project at the d day passed in parameter*/
    /*float Chart::calculateActualCost(Project &p, QDate day)
    {
        float cost=0;
        foreach(Node * currentNode, p.projectNode()->childNodeIterator()){
                //Si le jour pour lequel on veux le budget est > a la date de fin de la tachz
            if (day > (currentNode->workEndTime().dateTime().date())  )
            {
                //on ajoute le cout total de la tache
                cost+=currentNode->actualCost();
                }
            else
            {
                //sinon on ajoute le cout a la date en cours
                cost+=currentNode->actualCost(day);
                break;
            }
        }
        return (cost);
    }

    /*Caculate the planned cost of the project at the d day passed in parameter*/
//MODIFIER ICI PAREIL QU'AU DESSUS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   /* float Chart::calculatePlannedCost(Project &p, QDate day)
    {
        float cost=0;
        foreach(Node * currentNode, p.projectNode()->childNodeIterator()){
                cost+=currentNode->plannedCost(day);
            }
        return (cost);
    }*/
}
