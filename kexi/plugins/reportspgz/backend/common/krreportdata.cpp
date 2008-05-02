/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "krreportdata.h"
#include <kdebug.h>
#include <KoUnit.h>
#include <KoGlobal.h>
#include "krdetailsectiondata.h"
#include "krobjectdata.h"
#include "parsexmlutils.h"

void KRReportData::init()
{
	title = QString::null;

	pghead_first = pghead_odd = pghead_even = pghead_last = pghead_any = NULL;
	pgfoot_first = pgfoot_odd = pgfoot_even = pgfoot_last = pgfoot_any = NULL;
	rpthead = rptfoot = NULL;	
}

KRReportData::KRReportData()
{
	init();
	_valid = true;
}

KRReportData::KRReportData(const QDomElement & elemSource)
{
	_valid = false;
	init();
	bool valid; //used for local unit conversions
	
	if ( elemSource.tagName() != "report" )
	{
		kDebug () << "QDomElement passed to parseReport() was not <report> tag" << endl;
		kDebug() << elemSource.text() << endl;
		return;
	}

	qreal d = 0.0;

	QDomNodeList section = elemSource.childNodes();
	for ( int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++ )
	{
		QDomElement elemThis = section.item ( nodeCounter ).toElement();
		if ( elemThis.tagName() == "title" )
			title = elemThis.text();
		else if ( elemThis.tagName() == "datasource" )
			query = elemThis.text();
		else if ( elemThis.tagName() == "script" )
			script = elemThis.text();
		else if ( elemThis.tagName() == "size" )
		{
			if ( elemThis.firstChild().isText() )
				page.setPageSize ( elemThis.firstChild().nodeValue() );
			else
			{
				//bad code! bad code!
				// this code doesn't check the elemts and assums they are what
				// they should be.
				QDomNode n1 = elemThis.firstChild();
				QDomNode n2 = n1.nextSibling();
				if ( n1.nodeName() == "width" )
				{
					page.setCustomWidth ( n1.firstChild().nodeValue().toDouble() / 100.0 );
					page.setCustomHeight ( n2.firstChild().nodeValue().toDouble() / 100.0 );
				}
				else
				{
					page.setCustomWidth ( n2.firstChild().nodeValue().toDouble() / 100.0 );
					page.setCustomHeight ( n1.firstChild().nodeValue().toDouble() / 100.0 );
				}
				page.setPageSize ( "Custom" );
			}
		}
		else if ( elemThis.tagName() == "labeltype" )
			page.setLabelType ( elemThis.firstChild().nodeValue() );
		else if ( elemThis.tagName() == "portrait" )
			page.setPortrait ( true );
		else if ( elemThis.tagName() == "landscape" )
			page.setPortrait ( false );
		else if ( elemThis.tagName() == "topmargin" )
		{
			d = elemThis.text().toDouble ( &valid );
			if ( !_valid || d < 0.0 )
			{
				//TODO qDebug("Error converting topmargin value: %s",(const char*)elemThis.text());
				d = 50.0;
			}
			page.setMarginTop (POINT_TO_INCH( d ) * KoGlobal::dpiY());
		}
		else if ( elemThis.tagName() == "bottommargin" )
		{
			d = elemThis.text().toDouble ( &valid );
			if ( !valid || d < 0.0 )
			{
				//TODO qDebug("Error converting bottommargin value: %s",(const char*)elemThis.text());
				d = 50.0;
			}
			page.setMarginBottom (POINT_TO_INCH( d ) * KoGlobal::dpiY() );
		}
		else if ( elemThis.tagName() == "leftmargin" )
		{
			d = elemThis.text().toDouble ( &valid );
			if ( !valid || d < 0.0 )
			{
				//TODO qDebug("Error converting leftmargin value: %s",(const char*)elemThis.text());
				d = 50.0;
			}
			page.setMarginLeft ( POINT_TO_INCH( d ) * KoGlobal::dpiX() );
		}
		else if ( elemThis.tagName() == "rightmargin" )
		{
			d = elemThis.text().toDouble ( &valid );
			if ( !valid || d < 0.0 )
			{
				//TODO qDebug("Error converting rightmargin value: %s",(const char*)elemThis.text());
				d = 50.0;
			}
			page.setMarginRight ( POINT_TO_INCH( d ) * KoGlobal::dpiX());
		}
		else if ( elemThis.tagName() == "rpthead" )
		{
			KRSectionData * sd = new KRSectionData(elemThis);
			if ( sd->isValid() )
			{
				rpthead = sd;
//TODO Track Totals?				reportTarget.trackTotal += sd->trackTotal;
			}
			else
				delete sd;
		}
		else if ( elemThis.tagName() == "rptfoot" )
		{
			KRSectionData * sd = new KRSectionData(elemThis);
			if ( sd->isValid() )
			{
				rptfoot = sd;
//TODO Track Totals?				reportTarget.trackTotal += sd->trackTotal;
			}
			else
				delete sd;
		}
		else if ( elemThis.tagName() == "pghead" )
		{
			KRSectionData * sd = new KRSectionData(elemThis);
			if ( sd->isValid() )
			{
				if ( sd->extra() == "firstpage" )
					pghead_first = sd;
				else if ( sd->extra() == "odd" )
					pghead_odd = sd;
				else if ( sd->extra() == "even" )
					pghead_even = sd;
				else if ( sd->extra() == "lastpage" )
					pghead_last = sd;
				else if ( sd->extra() == QString::null )
					pghead_any = sd;
				else
				{
					//TODO qDebug("don't know which page this page header is for: %s",(const char*)sd->extra);
					delete sd;
				}
//TODO Track Totals?				reportTarget.trackTotal += sd->trackTotal;
			}
			else
				delete sd;
		}
		else if ( elemThis.tagName() == "pgfoot" )
		{
			KRSectionData * sd = new KRSectionData(elemThis);
			if ( sd->isValid() )
			{
				if ( sd->extra() == "firstpage" )
					pgfoot_first = sd;
				else if ( sd->extra() == "odd" )
					pgfoot_odd = sd;
				else if ( sd->extra() == "even" )
					pgfoot_even = sd;
				else if ( sd->extra() == "lastpage" )
					pgfoot_last = sd;
				else if ( sd->extra() == QString::null )
					pgfoot_any = sd;
				else
				{
					//TODO qDebug("don't know which page this page footer is for: %s",(const char*)sd->extra);
					delete sd;
				}
//TODO Track Totals?				reportTarget.trackTotal += sd->trackTotal;
			}
			else
				delete sd;
		}
		else if ( elemThis.tagName() == "section" )
		{
			KRDetailSectionData * dsd = new KRDetailSectionData(elemThis);
			
			if (dsd->isValid())
			{
				detailsection = dsd;
				//reportTarget.trackTotal += dsd->trackTotal;
			}
			else
			{
				delete dsd;
			}
		}

		//else
		//TODO qDebug("While parsing report encountered an unknown element: %s",(const char*)elemThis.tagName());
	}

	_valid = true;
}


KRReportData::~KRReportData()
{
}

QList<KRObjectData*> KRReportData::objects()
{
	QList<KRObjectData*> obs;
	KRSectionData *sec;
	
	for (int i = 0; i <12 ; ++i)
	{
		sec = section((KRReportData::Section)(i+1));
		if (sec)
		{
			obs << sec->objects();
		}
	}
	
	if (detailsection )
	{
		kDebug() << "Number of groups: " << detailsection->groupList.count() << endl;
		foreach (ORDetailGroupSectionData* g, detailsection->groupList)
		{
			if (g->head)
			{
				obs << g->head->objects();
			}
			if (g->foot)
			{
				obs << g->foot->objects();	
			}
		}
		if (detailsection->detail)
			obs << detailsection->detail->objects();
	}
	
	kDebug() << "Object List:" << endl;
	foreach (KRObjectData* o, obs)
	{
		kDebug() << o->entityName() << endl;
	}
	return obs;
}

KRObjectData* KRReportData::object(const QString& n)
{
	QList<KRObjectData*> obs = objects();
	
	foreach (KRObjectData* o, obs)
	{
		if (o->entityName() == n)
		{
			return o;
		}
	}
	return 0;
}

QList<KRSectionData*> KRReportData::sections()
{
	QList<KRSectionData*> secs;
	KRSectionData *sec;
	for (int i = 0; i <12 ; ++i)
	{
		sec = section((KRReportData::Section)(i+1));
		if (sec)
		{
			secs << sec;
		}
	}
	
	if (detailsection )
	{
		kDebug() << "Number of groups: " << detailsection->groupList.count() << endl;
		foreach (ORDetailGroupSectionData* g, detailsection->groupList)
		{
			if (g->head)
			{
				secs << g->head;
			}
			if (g->foot)
			{
				secs << g->foot;	
			}
		}
		if (detailsection->detail)
			secs << detailsection->detail;
	}
	
	return secs;
}

KRSectionData* KRReportData::section(const QString& sn)
{
	QList<KRSectionData*> secs = sections();
	
	foreach(KRSectionData *sec, secs)
	{
		if (sec->name() == sn)
		{
			return sec;
		}
	}
	return 0;
}

KRSectionData* KRReportData::section(KRReportData::Section s)
{
	KRSectionData *sec;
	switch ( s )
	{
		case KRReportData::PageHeadAny:
			sec = pghead_any;
			break;
		case KRReportData::PageHeadEven:
			sec = pghead_even;
			break;
		case KRReportData::PageHeadOdd:
			sec = pghead_odd;
			break;
		case KRReportData::PageHeadFirst:
			sec = pghead_first;
			break;
		case KRReportData::PageHeadLast:
			sec = pghead_last;
			break;
		case KRReportData::PageFootAny:
			sec = pgfoot_any;
			break;
		case KRReportData::PageFootEven:
			sec = pgfoot_even;
			break;
		case KRReportData::PageFootOdd:
			sec = pgfoot_odd;
			break;
		case KRReportData::PageFootFirst:
			sec = pgfoot_first;
			break;
		case KRReportData::PageFootLast:
			sec = pgfoot_last;
			break;
		case KRReportData::ReportHead:
			sec = rpthead;
			break;
		case KRReportData::ReportFoot:
			sec = rptfoot;
			break;
		default:
			sec = NULL;
	}
	return sec;
}