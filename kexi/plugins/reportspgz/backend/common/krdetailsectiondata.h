//
// C++ Interface: krdetailsectiondata
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KRDETAILSECTIONDATA_H
#define KRDETAILSECTIONDATA_H

#include <QObject>
#include <qdom.h>

class KRSectionData;
class ORDetailGroupSectionData;
/**
	@author Adam Pigg <adam@piggz.co.uk>
*/
class KRDetailSectionData : public QObject
{
		Q_OBJECT
	public:
		KRDetailSectionData();
		KRDetailSectionData(const QDomElement &);
		~KRDetailSectionData();

		enum PageBreak
		{
			BreakNone = 0,
			BreakAtEnd = 1
		};

		QString name;
		int pagebreak;

		KRSectionData * detail;

		QList<ORDetailGroupSectionData*> groupList;
		// QList<ORDataData> trackTotal;
		bool isValid(){return _valid;}
		
	private:
		bool _valid;

};

#endif
