//
// C++ Implementation: krdetailsectiondata
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krdetailsectiondata.h"
#include "krsectiondata.h"
#include "parsexmlutils.h"

KRDetailSectionData::KRDetailSectionData()
{
	name = QString::null;
	pagebreak = BreakNone;
	detail = 0;
}

KRDetailSectionData::KRDetailSectionData(const QDomElement &elemSource)
{
	_valid = false;
	if ( elemSource.tagName() != "section" )
	{
		return;
	}

	bool have_detail = false;

	KRSectionData * old_head = 0;
	KRSectionData * old_foot = 0;

	QDomNodeList section = elemSource.childNodes();
	for ( int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++ )
	{
		QDomElement elemThis = section.item ( nodeCounter ).toElement();
		if ( elemThis.tagName() == "pagebreak" )
		{
			if ( elemThis.attribute ( "when" ) == "at end" )
				pagebreak = BreakAtEnd;
		}
		else if ( elemThis.tagName() == "grouphead" )
		{
			KRSectionData * sd = new KRSectionData(elemThis);
			if (sd->isValid())
			{
				old_head = sd;
			//TODO Track Totals?	sectionTarget.trackTotal += sd->trackTotal;
			}
			else
				delete sd;
		
		}
		else if ( elemThis.tagName() == "groupfoot" )
		{
			KRSectionData * sd = new KRSectionData(elemThis);
			if (sd->isValid())
			{
				old_foot = sd;
			//TODO Track Totals?	sectionTarget.trackTotal += sd->trackTotal;
			}
			else
				delete sd;
		}
		else if ( elemThis.tagName() == "group" )
		{
			QDomNodeList nl = elemThis.childNodes();
			QDomNode node;
			ORDetailGroupSectionData * dgsd = new ORDetailGroupSectionData();
			for ( int i = 0; i < nl.count(); i++ )
			{
				node = nl.item ( i );
				if ( node.nodeName() == "column" )
					dgsd->column = node.firstChild().nodeValue();
				else if ( node.nodeName() == "pagebreak" )
				{
					QDomElement elemThis = node.toElement();
					QString n = elemThis.attribute ( "when" );
					if ( "after foot" == n )
						dgsd->pagebreak = ORDetailGroupSectionData::BreakAfterGroupFoot;
				}
				else if ( node.nodeName() == "head" )
				{
					KRSectionData * sd = new KRSectionData(node.toElement());
					if (sd->isValid())
					{
						dgsd->head = sd;
						//TODO Track Totals?sectionTarget.trackTotal += sd->trackTotal;
						//for ( QList<ORDataData>::iterator it = sd->trackTotal.begin(); it != sd->trackTotal.end(); ++it )
						//	dgsd->_subtotCheckPoints[*it] = 0.0;
					}
					else
						delete sd;
				}
				else if ( node.nodeName() == "foot" )
				{
					KRSectionData * sd = new KRSectionData(node.toElement());
					if ( sd->isValid())
					{
						dgsd->foot = sd;
						//TODO Track Totals?sectionTarget.trackTotal += sd->trackTotal;
						//for ( QList<ORDataData>::iterator it = sd->trackTotal.begin(); it != sd->trackTotal.end(); ++it )
						//	dgsd->_subtotCheckPoints[*it] = 0.0;
					}
					else
						delete sd;
				}
				//else
				//TODO qDebug("While parsing group section encountered an unknown element: %s", node.nodeName().toLatin1());
			}
			groupList.append ( dgsd );
		}
		else if ( elemThis.tagName() == "detail" )
		{
			KRSectionData * sd = new KRSectionData(elemThis);
			if ( sd->isValid() )
			{
				detail = sd;
				//TODO Track Totals?sectionTarget.trackTotal += sd->trackTotal;
				have_detail = true;
			}
			else
				delete sd;
		}
		//else
		//TODO qDebug("While parsing detail section encountered an unknown element: %s",(const char*)elemThis.tagName());
	}

	_valid = true;
}

KRDetailSectionData::~KRDetailSectionData()
{
}


