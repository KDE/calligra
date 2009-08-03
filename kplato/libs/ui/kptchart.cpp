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
#include "kptschedule.h"
#include "kpteffortcostmap.h"

#include <klocale.h>
#include <cmath>
#include <kdebug.h>

namespace KPlato
{


Chart::Chart()
{
    mTotalTime = 0.0;

    mBCWPPoints.clear();
    mBCWSPoints.clear();
    mACWPPoints.clear();
    //init totalBudget
}

//******************
// CALCULATE DATA 
//******************


QVector<QDate> Chart::calculateWeeks(const Project & p,const ScheduleManager &sm)
{
    QVector<QDate> lWeeks;
    //Schedule *s = p.findSchedule( sm.id() );
    //kDebug()<<" Schedule: " <<(s==0?"None":s->name()+QString(", %1").arg(s->type()));

    QDate myDate = acwpPrDay.startDate();
    if ( ! myDate.isValid() || bcwpPrDay.startDate() < myDate ) {
        myDate = bcwpPrDay.startDate();
    }
    QDate endDate = acwpPrDay.endDate();
    if ( ! endDate.isValid() || bcwpPrDay.endDate() > endDate ) {
        endDate = bcwpPrDay.endDate();
    }

    while(myDate < endDate)
    {
        lWeeks.push_back(myDate);
        myDate=myDate.addDays(7);
    }
    lWeeks.push_back(endDate);
    kDebug()<<lWeeks;
    return lWeeks;
}

void Chart::calculatePlannedCost( const Project & p, const ScheduleManager &sm)
{
    //kDebug()<<"calculatePlannedCost()"<<weeks<<","<<vect;
    mBCWSPoints.clear();

    const QVector<QDate>  lWeeks = calculateWeeks(p,sm);
    QVector<QDate>::const_iterator it_weeks = lWeeks.begin();
    float sum=0;
    //mBCWSPoints.push_back(QPointF(0,0));

    for(int i=0;i<lWeeks.size();i++)
    {
        QPointF lTemPoint;
        lTemPoint.setX(i);

        for (int j = 0; j < 7; ++j ) {
            QDate date = (*it_weeks).addDays(j);
            sum += bcwpPrDay.costOnDate( date );
            kDebug()<<date<<sum;
            if ( date == lWeeks.last() ) {
                break;
            }
        }
        lTemPoint.setY(sum);
        /*if ( *it_weeks == weeks.last() ) {
            break; // The last entry is the last date, not the start of a week!
        }*/
        mBCWSPoints.push_back(lTemPoint);

        it_weeks++;
    }
    setMaxCost(sum);
    kDebug()<<mBCWSPoints;
}

void Chart::calculateActualCost(const Project & p, const ScheduleManager &sm)
{
    //kDebug()<<"calculateActualCost()"<<weeks<<vect;
    mACWPPoints.clear();
    
    const QVector<QDate>  lWeeks = calculateWeeks(p,sm);
    QVector<QDate>::const_iterator it_weeks = lWeeks.begin();
    float sum=0;
    //mACWPPoints.push_back(QPointF(0,0));

    for(int i=0;i<lWeeks.size();i++)
    {
        QPointF lTemPoint;
        lTemPoint.setX(i);

        for (int j = 0; j < 7; ++j ) {
            QDate date = (*it_weeks).addDays(j);
            sum += acwpPrDay.costOnDate( date );
            if ( date == lWeeks.last() ) {
                break;
            }
        }
        lTemPoint.setY(sum);
        /*if ( *it_weeks == weeks.last() ) {
        break; // The last entry is the last date, not the start of a week!
    }*/
        mACWPPoints.push_back(lTemPoint);

        it_weeks++;
    }
    setMaxCost(sum);
    kDebug()<<mACWPPoints;
}

void Chart::calculateBCWP(const Project & p, const ScheduleManager &sm)
{
    kDebug()<<"calculateBCWP()";

    mBCWPPoints.clear();

    const QVector<QDate>  lWeeks = calculateWeeks(p,sm);
    QVector<QDate>::const_iterator it_weeks = lWeeks.begin();
    float sum=0;
//    mBCWPPoints.push_back(QPointF(0,0));

    for(int i=0;i<lWeeks.size();i++)
    {
        QPointF lTemPoint;
        lTemPoint.setX(i);

        for (int j = 0; j < 7; ++j ) {
            QDate date = (*it_weeks).addDays(j);
            double c = bcwpPrDay.bcwpCostOnDate( date );
            if ( c > 0.0 ) {
                sum = c;
            }
            kDebug()<<date<<sum;
            if ( date == lWeeks.last() ) {
                break;
            }
        }
        lTemPoint.setY(sum);
        kDebug()<<i<<lTemPoint;
        /*if ( *it_weeks == weeks.last() ) {
        break; // The last entry is the last date, not the start of a week!
    }*/
        mBCWPPoints.push_back(lTemPoint);

        it_weeks++;
    }
    setMaxCost(sum);
    kDebug()<<mBCWPPoints;
}

float Chart::getMaxCost()
{
    return mTotalCostPlanned;
}


float Chart::getMaxTime()
{
    return mTotalTime;
}

void Chart::calculateData(const Project & p, const ScheduleManager & sm)
{
    mTotalTime = 0.0;
    mTotalCostPlanned = 0.0;
    kDebug()<<p.name()<<sm.id();
    acwpPrDay = p.acwp( sm.id() );
    bcwpPrDay = p.bcwpPrDay( sm.id() );

    calculateBCWP(p,sm);
    calculateActualCost(p,sm);
    calculatePlannedCost(p,sm);

    if ( ! mBCWSPoints.isEmpty() ) {
        mTotalTime = (float)mBCWSPoints.last().x();
    }
    if ( ! mBCWPPoints.isEmpty() ) {
        mTotalTime = qMax( (float)mBCWPPoints.last().x(), mTotalTime );
    }
    if ( ! mACWPPoints.isEmpty() ) {
        mTotalTime = qMax( (float)mACWPPoints.last().x(), mTotalTime );
    }
    //costToPercent();
}

//******************************************
// TRANSFORM DATA TO CHART COORDINATES
//******************************************

/* Calculate the percentage of the cost and replace the result in the vector */
void Chart::costToPercent(QVector<QPointF> & vect)
{
    QVector<QPointF>::iterator it= vect.begin();
    while(it != vect.end())
    {
        //kDebug()<<"itY before"<<it->y();
        if (mTotalCostPlanned > 0.0) { // avoid divide by 0
            it->setY(it->y()*100/mTotalCostPlanned);
        } else {
            it->setY(0);
        }
        //kDebug()<<"cost after %"<<it->y();
        it++;
    }
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
    kDebug()<<"api() :"<<data<<","<<display;
    reCalculateY( data, display, size.height() );
    reCalculateX( data, display, size.width() );
    kDebug()<<"api():"<<data<<","<<display;
}


/* Calculate the new value of every Y-axis when the window hab been re-sized */
void Chart::reCalculateY(QVector<QPointF> & vect, QVector<QPointF> & vect_display, int maximumHeight)// WORKS, TESTED
{    
    float inverse;
    float tmp;
    
    kDebug()<<maximumHeight;
    QVector<QPointF>::iterator it= vect.begin();
    QVector<QPointF>::iterator it_display= vect_display.begin();
    while (it != vect.end())
    {
        inverse = mTotalTime - it->y();
        tmp=((maximumHeight - ChartWidget::BOTTOMMARGIN - ChartWidget::TOPMARGIN)*inverse)/100.0;
        it_display->setY(tmp+ChartWidget::TOPMARGIN);
        it++;it_display++;
    }
}
/* Calculate the new value of X-axis when the window had been re-sized */
void Chart::reCalculateX(QVector<QPointF> & vect, QVector<QPointF> & vect_display, int maximumWidth)// WORKS, TESTED
{
    float tmp; 
    //kDebug()<<maximumWidth;
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
void Chart::setMaxTime( )// WORKS, TESTED
{
    mTotalTime = 0.0;

    
    if( !mBCWPPoints.isEmpty())
    {
        if( !mBCWSPoints.isEmpty() )
        {
            if( (mBCWPPoints.last()).y() <=  mBCWSPoints.last().y() )
            {
    
                if( !mACWPPoints.isEmpty() )
                {
                        if(   (mBCWSPoints.last()).y() <=  (mACWPPoints.last()).y() )
                        {
                            mTotalTime=(mACWPPoints.last()).y();
                        }
                        else
                        {
                            mTotalTime=(mBCWSPoints.last()).y();
                        }
                }
                else
                {
                    mTotalTime=(mBCWSPoints.last()).y();
                }
            }
            else
            {
                if( !mACWPPoints.isEmpty() )
                {
                        if( (mBCWPPoints.last()).y()  <= (mACWPPoints.last()).y() )
                        {
                            mTotalTime=(mACWPPoints.last()).y();
                        }
                        else
                        {
                            mTotalTime=(mBCWPPoints.last()).y();
                        }
                }
                else
                {
                    mTotalTime=(mBCWPPoints.last()).y();
                }
            }
        }
        else
        {
            if( !mACWPPoints.isEmpty() )
            {
                    if( (mBCWPPoints.last()).y()  <= (mACWPPoints.last()).y() )
                    {
                        mTotalTime=(mACWPPoints.last()).y();
                    }
                    else
                    {
                        mTotalTime=(mBCWPPoints.last()).y();
                    }
            }
            else
            {
                mTotalTime=(mBCWPPoints.last()).y();
            }
        }
    }
    else
    {
        if( !mACWPPoints.isEmpty() )
        {
                if(   (mBCWSPoints.last()).y() <=  (mACWPPoints.last()).y() )
                {
                    mTotalTime=(mACWPPoints.last()).y();
                }
                else
                {
                    mTotalTime=(mBCWSPoints.last()).y();
                }
        }
        else
        {
            mTotalTime=(mBCWSPoints.last()).y();
        }
    }
    kDebug()<<"TOTAL TIME :"<<mTotalTime;
}



/* Calculate the percentage of the time and replace the result in the vector */
void Chart::timeToPercent(QVector<QPointF> & vect)
{
    QVector<QPointF>::iterator it= vect.begin();
    while(it != vect.end())
    {
        //kDebug()<<"itx before"<<it->x();
        if (vect.size() > 1 ) { // avoid divide by 0
            it->setX(it->x()*100/(vect.size()-1));
        } else {
            it->setX(0);
        }
        //kDebug()<<"time after %"<<it->x();
        it++;
    }
}

void Chart::setMaxCost(float pMaxCost)
{
    if ( pMaxCost*1.1 > mTotalCostPlanned )
        mTotalCostPlanned=pMaxCost*1.1; // add 10%
}

}
