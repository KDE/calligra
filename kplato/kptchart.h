/* This file is part of the KDE project
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
#ifndef KPTCHART_H
#define KPTCHART_H

#include "kptchartpanel.h"

#include <klocale.h>

#include <kdebug.h>
#include <QDateTime>
#include <QVector>
#include "kptproject.h"

namespace KPlato
{


	class Chart
	{
		private:
			float sizeSave[3][2];
			float totalBudget;	
			int totalWeek;
			
		public:
			void reCalculateY(QVector<QPoint> *,int,int,int,float,int);
			void reCalculateX(QVector<QPoint> *,int,int,int,float);			
			int maxVector(QVector<QPoint>);
			void CostToPercent(QVector<QPoint>*);
			void TimeToPercent(QVector<QPoint>*);
			//think to add a parameter to the function (date)
			float calculateActualCost(Project &, QDate);
			//think to add a parameter to the function (date)
			float calculatePlannedCost(Project &, QDate);
			//For latter but not to forget to do it :D
			void calculateValueOfBCWS(Project &, QVector<QPoint>*);
			/*void calculateValueOfBCWP(Project &, QVector<QPoint>*);
			void calculateValueOfACWP(Project &, QVector<QPoint>*);*/
			

			
	};

} //namespace KPlato

#endif
