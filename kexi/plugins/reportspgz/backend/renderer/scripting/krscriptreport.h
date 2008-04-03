//
// C++ Interface: krscriptreport
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCRIPTINGKRSCRIPTREPORT_H
#define SCRIPTINGKRSCRIPTREPORT_H

#include <QObject>
class KRReportData;
class KRObjectData;
namespace Scripting
{

	/**
		@author Adam Pigg <adam@piggz.co.uk>
	*/
	class Report : public QObject
	{
			Q_OBJECT
		public:
			Report ( KRReportData* );

			~Report();

		public slots:
			QString title();
			QString recordSource();
			KRObjectData* objectByName(const QString &);
			
		private:
			KRReportData *_reportdata;
	};

}

#endif
