//
// C++ Interface: krscriptchart
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCRIPTINGKRSCRIPTCHART_H
#define SCRIPTINGKRSCRIPTCHART_H

#include <QObject>
#include <QPointF>
#include <QSizeF>

class KRChartData;

namespace Scripting
{

	/**
		@author Adam Pigg <adam@piggz.co.uk>
	*/
	class Chart : public QObject
	{
			Q_OBJECT
		public:
			Chart ( KRChartData * );

			~Chart();
		public slots:


			/**
		 * Get the position of the barcode
		 * @return position in points
			 */
			QPointF position();


			/**
			 * Sets the position of the barcode in points
			 * @param Position
			 */
			void setPosition ( const QPointF& );

			/**
			 * Get the size of the barcode
			 * @return size in points
			 */
			QSizeF size();

			/**
			 * Set the size of the barcode in points
			 * @param Size
			 */
			void setSize ( const QSizeF& );	
		private:
			KRChartData* _chart;

	};

}

#endif
