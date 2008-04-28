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
			
		private:
			KRChartData* _chart;

	};

}

#endif
