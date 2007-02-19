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
        for(int i=0;i<6;i++)
        {
              for(int j=0;j<6;j++)
                    sizeSave[i][j]=0;
        }
	//init totalBudget
    }

    /* Calculate the new value of every Y-axis when the window hab been re-sized */
    void Chart::reCalculateY(QVector<QPointF> & vect,const int n_curve, int maximumHeight)
    {    
        float inverse;
        float tmp;
	int i=0;
	kDebug()<<"ReCalculateY ! : ";
	
        if(maximumHeight != sizeSave[n_curve][1])
        {
	    QVector<QPointF>::iterator it= vect.begin();
            while (i<vect.size())
            {
                inverse = maxYPercent - it->y(); /* A MODIFIER :D */

                tmp=(maximumHeight - ChartWidget::BOTTOMMARGIN - ChartWidget::TOPMARGIN)*inverse/100;
                it->setY(tmp+ChartWidget::TOPMARGIN);

                it++;i++;
            }
		sizeSave[n_curve][1]=maximumHeight;
		// Save to not doing all the work next time if the data are already the same
        }
    }

    /* Calculate the new value of X-axis when the window had been re-sized */
    void Chart::reCalculateX(QVector<QPointF> & vect,const int n_curve, int maximumWidth)
    {
        float tmp; /* temporary result */

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

    /* Calculate the maximum of a curve */
//FIX ME !      Bug : When the second and following exec  of the method, data are not any more % but coordinate!!
    void Chart::setMaxVector(QVector<QPointF> vect)
    {
        float max_tmp=0;
        QVector<QPointF>::iterator it= vect.begin();
	int i=0;
        while(i<vect.size())
        {
            if(it->y() > max_tmp)
            {
                max_tmp=it->y();
            }
            it++;i++;
        }
	maxYPercent=max_tmp;
    }

    /* Calculate the percentage of the cost and replace the result in the vector */
    void Chart::CostToPercent(QVector<QPointF> & vect)
    {
        QVector<QPointF>::iterator it= vect.begin();
        while(it != vect.end())
        {
            it->setY(it->y()*100/totalBudget);
            it++;
        }
    }

    /* Calculate the percentage of the time and replace the result in the vector */
    void Chart::TimeToPercent(QVector<QPointF> & vect)
    {
        QVector<QPointF>::iterator it= vect.begin();
        while(it != vect.end())
        {
            it->setX(it->x()*100/totalWeek);
            it++;
        }
    }

    /*Caculate the actual cost of the project at the d day passed in parameter*/
    float Chart::calculateActualCost(Project &p, QDate day)
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
    float Chart::calculatePlannedCost(Project &p, QDate day)
    {
        float cost=0;
        foreach(Node * currentNode, p.projectNode()->childNodeIterator()){
                cost+=currentNode->plannedCost(day);
            }
        return (cost);
    }

    void calculateValueOfBCWS(Project &, QVector<QPoint>*)
    {
        
    }
}
