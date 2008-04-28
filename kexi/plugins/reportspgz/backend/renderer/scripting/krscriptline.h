//
// C++ Interface: krscriptline
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCRIPTINGKRSCRIPTLINE_H
#define SCRIPTINGKRSCRIPTLINE_H

#include <QObject>
class KRLineData;

namespace Scripting
{

	/**
		@author Adam Pigg <adam@piggz.co.uk>
	*/
	class Line : public QObject
	{
			Q_OBJECT
		public:
			Line ( KRLineData * );

			~Line();
			
		private:
			KRLineData *_line;

	};

}

#endif
