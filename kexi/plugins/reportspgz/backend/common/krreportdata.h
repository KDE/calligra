//
// C++ Interface: krreportdata
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KRREPORTDATA_H
#define KRREPORTDATA_H

#include <QObject>
#include <qdom.h>
#include "krsectiondata.h"
#include "reportpageoptions.h"
#include "parsexmlutils.h"

class KRDetailSectionData;

namespace Scripting
{
	class Report;
}
/**
	@author Adam Pigg <adam@piggz.co.uk>
*/
class KRReportData : public QObject
{
		Q_OBJECT

	public:
		KRReportData ( const QDomElement & elemSource );
		KRReportData ();
		~KRReportData();
		
		
		/**
		\return a list of all objects in the report
		*/
		QList<KRObjectData*> objects();
		
		/**
		\return a report object given its name
		*/
		KRObjectData* objectByName(const QString&);
		
	protected:
		QString title;
		QString query;
		QString script;
		
		ReportPageOptions page;
		


		KRSectionData * pghead_first;
		KRSectionData * pghead_odd;
		KRSectionData * pghead_even;
		KRSectionData * pghead_last;
		KRSectionData * pghead_any;

		KRSectionData * rpthead;
		KRSectionData * rptfoot;

		KRSectionData * pgfoot_first;
		KRSectionData * pgfoot_odd;
		KRSectionData * pgfoot_even;
		KRSectionData * pgfoot_last;
		KRSectionData * pgfoot_any;

		KRDetailSectionData* detailsection;
	private:
		bool _valid;
		void init();
		
		friend class ORPreRenderPrivate;
		friend class ORPreRender;
		friend class KRScriptHandler;
		friend class Scripting::Report;
//    QList<ORDataData> trackTotal;
};

#endif
