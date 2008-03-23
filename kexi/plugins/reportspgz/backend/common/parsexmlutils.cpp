/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
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

#include "parsexmlutils.h"


#include "reportpageoptions.h"

#include <qdom.h>
#include <kdebug.h>
#include <QPainter>
#include <KoGlobal.h>
#include <KoUnit.h>

//TODO Graph
//bool ORGraphData::isGraph() { return TRUE; }
//ORGraphData * ORGraphData::toGraph() { return this; }

ORDetailGroupSectionData::ORDetailGroupSectionData()
{
	column = QString::null;
	pagebreak = BreakNone;
	_subtotCheckPoints.clear();
	head = 0;
	foot = 0;
}

ORDetailSectionData::ORDetailSectionData()
{
	name = QString::null;
	pagebreak = BreakNone;
	detail = 0;
}

ORReportData::ORReportData()
{
	title = QString::null;

	pghead_first = pghead_odd = pghead_even = pghead_last = pghead_any = NULL;
	pgfoot_first = pgfoot_odd = pgfoot_even = pgfoot_last = pgfoot_any = NULL;
	rpthead = rptfoot = NULL;
}

//
// functions
//

bool parseReportTextStyleData ( const QDomElement & elemSource, ORTextStyleData & ts )
{
	if ( elemSource.tagName() == "textstyle" )
	{
		QDomNode  nodeCursor = elemSource.firstChild();
		ts.bgOpacity = 255;
		while ( !nodeCursor.isNull() )
		{
			
			if ( nodeCursor.isElement() )
			{
				QDomElement elemThis = nodeCursor.toElement();
				if ( elemThis.tagName() == "bgcolor" )
				{
					ts.bgColor = elemThis.text();
				}
				else if ( elemThis.tagName() == "fgcolor" )
				{
					ts.fgColor = elemThis.text();
				}
				else if ( elemThis.tagName() == "bgopacity" )
				{
					ts.bgOpacity = elemThis.text().toInt();
				}
				else if ( elemThis.tagName() == "font" )
				{
					parseReportFont(elemThis, ts.font);
				}
				else
				{
					// we have encountered a tag that we don't understand.
					// for now we will just inform a debugger about it
					kDebug() << "Tag not Parsed at <textstle>:" << elemThis.tagName() << endl;
				}
			}
			
			nodeCursor = nodeCursor.nextSibling();
		}
		return TRUE;
	}
	return FALSE;
}

bool parseReportLineStyleData ( const QDomElement & elemSource, ORLineStyleData & ls )
{
	if ( elemSource.tagName() == "linestyle" )
	{
		QDomNode  nodeCursor = elemSource.firstChild();

		while ( !nodeCursor.isNull() )
		{
			
			if ( nodeCursor.isElement() )
			{
				QDomElement elemThis = nodeCursor.toElement();
				if ( elemThis.tagName() == "color" )
				{
					ls.lnColor = elemThis.text();
				}
				else if ( elemThis.tagName() == "weight" )
				{
					ls.weight = elemThis.text().toInt();
				}
				else if ( elemThis.tagName() == "style" )
				{
					QString l = elemThis.text();
					if (l == "nopen")
					{
						ls.style = Qt::NoPen;
					}
					else if (l == "solid")
					{
						ls.style = Qt::SolidLine;
					}
					else if (l == "dash")
					{
						ls.style = Qt::DashLine;
					}
					else if (l == "dot")
					{
						ls.style = Qt::DotLine;
					}
					else if (l == "dashdot")
					{
						ls.style = Qt::DashDotLine;
					}
					else if (l == "dashdotdot")
					{
						ls.style = Qt::DashDotDotLine;
					}
					else 
					{
						ls.style = Qt::SolidLine;
					}
				}
				else
				{
					// we have encountered a tag that we don't understand.
					// for now we will just inform a debugger about it
					kDebug() << "Tag not Parsed at <linestle>:" << elemThis.tagName() << endl;
				}
			}
			
			nodeCursor = nodeCursor.nextSibling();
		}
		return TRUE;
	}
	return FALSE;
}


bool parseReportRect ( const QDomElement & elemSource, QRectF & rectTarget )
{
	if ( elemSource.tagName() == "rect" )
	{
		QDomNode  nodeCursor = elemSource.firstChild();

		while ( !nodeCursor.isNull() )
		{
			if ( nodeCursor.isElement() )
			{
				QDomElement elemThis = nodeCursor.toElement();
				int         intTemp;
				bool        valid;

				if ( elemThis.tagName() == "x" )
				{
					intTemp = elemThis.text().toFloat ( &valid );
					if ( valid )
						rectTarget.setX ( intTemp );
					else
						return FALSE;
				}
				else if ( elemThis.tagName() == "y" )
				{
					intTemp = elemThis.text().toFloat ( &valid );
					if ( valid )
						rectTarget.setY ( intTemp );
					else
						return FALSE;
				}
				else if ( elemThis.tagName() == "width" )
				{
					intTemp = elemThis.text().toFloat ( &valid );
					if ( valid )
						rectTarget.setWidth ( intTemp );
					else
						return FALSE;
				}
				else if ( elemThis.tagName() == "height" )
				{
					intTemp = elemThis.text().toFloat ( &valid );
					if ( valid )
						rectTarget.setHeight ( intTemp );
					else
						return FALSE;
				}
			}
			nodeCursor = nodeCursor.nextSibling();
		}
		return TRUE;
	}
	return FALSE;
}

bool parseReportFont ( const QDomElement & elemSource, QFont & fontTarget )
{
	if ( elemSource.tagName() == "font" )
	{
		fontTarget.fromString(elemSource.text());
#if 0
		QDomNode  nodeCursor = elemSource.firstChild();

		while ( !nodeCursor.isNull() )
		{
			if ( nodeCursor.isElement() )
			{
				QDomElement elemThis = nodeCursor.toElement();
				int intTemp;
				bool valid;

				if ( elemThis.tagName() == "face" )
					fontTarget.setFamily ( elemThis.text() );
				else if ( elemThis.tagName() == "size" )
				{
					intTemp = elemThis.text().toInt ( &valid );
					if ( valid )
						fontTarget.setPointSize ( intTemp );
					else
						kDebug() << "Text not Parsed at <font>:" << elemThis.text() << endl;
				}
				else if ( elemThis.tagName() == "weight" )
				{
					if ( elemThis.text() == "normal" )
						fontTarget.setWeight ( 50 );
					else if ( elemThis.text() == "bold" )
						fontTarget.setWeight ( 75 );
					else
					{
						// This is where we want to convert the string to an int value
						// that should be between 1 and 100
						intTemp = elemThis.text().toInt ( &valid );
						if ( valid && intTemp >= 1 && intTemp <= 100 )
							fontTarget.setWeight ( intTemp );
						else
							kDebug() << "Text not Parsed at <font>:" << elemThis.text() << endl;
					}
				}
				else
				{
					// we have encountered a tag that we don't understand.
					// for now we will just inform a debugger about it
					kDebug() << "Tag not Parsed at <font>:" << elemThis.tagName() << endl;
				}
			}
			nodeCursor = nodeCursor.nextSibling();
		}
#endif
		return TRUE;
	}
	return FALSE;
}

//TODO Graphs



bool parseReportDetailSection ( const QDomElement & elemSource, ORDetailSectionData & sectionTarget )
{
	if ( elemSource.tagName() != "section" )
		return FALSE;

	bool have_detail = FALSE;

	KRSectionData * old_head = 0;
	KRSectionData * old_foot = 0;

	QDomNodeList section = elemSource.childNodes();
	for ( int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++ )
	{
		QDomElement elemThis = section.item ( nodeCounter ).toElement();
		if ( elemThis.tagName() == "pagebreak" )
		{
			if ( elemThis.attribute ( "when" ) == "at end" )
				sectionTarget.pagebreak = ORDetailSectionData::BreakAtEnd;
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
			sectionTarget.groupList.append ( dgsd );
		}
		else if ( elemThis.tagName() == "detail" )
		{
			KRSectionData * sd = new KRSectionData(elemThis);
			if ( sd->isValid() )
			{
				sectionTarget.detail = sd;
				//TODO Track Totals?sectionTarget.trackTotal += sd->trackTotal;
				have_detail = true;
			}
			else
				delete sd;
		}
		//else
		//TODO qDebug("While parsing detail section encountered an unknown element: %s",(const char*)elemThis.tagName());
	}

	return ( have_detail );
}

bool parseReport ( const QDomElement & elemSource, ORReportData & reportTarget )
{
	if ( elemSource.tagName() != "report" )
	{
		kDebug () << "QDomElement passed to parseReport() was not <report> tag" << endl;
		kDebug() << elemSource.text() << endl;
		return false;
	}

	qreal d = 0.0;
	bool valid = false;

	QDomNodeList section = elemSource.childNodes();
	for ( int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++ )
	{
		QDomElement elemThis = section.item ( nodeCounter ).toElement();
		if ( elemThis.tagName() == "title" )
			reportTarget.title = elemThis.text();
		else if ( elemThis.tagName() == "datasource" )
			reportTarget.query = elemThis.text();
		else if ( elemThis.tagName() == "script" )
			reportTarget.script = elemThis.text();
		else if ( elemThis.tagName() == "size" )
		{
			if ( elemThis.firstChild().isText() )
				reportTarget.page.setPageSize ( elemThis.firstChild().nodeValue() );
			else
			{
				//bad code! bad code!
				// this code doesn't check the elemts and assums they are what
				// they should be.
				QDomNode n1 = elemThis.firstChild();
				QDomNode n2 = n1.nextSibling();
				if ( n1.nodeName() == "width" )
				{
					reportTarget.page.setCustomWidth ( n1.firstChild().nodeValue().toDouble() / 100.0 );
					reportTarget.page.setCustomHeight ( n2.firstChild().nodeValue().toDouble() / 100.0 );
				}
				else
				{
					reportTarget.page.setCustomWidth ( n2.firstChild().nodeValue().toDouble() / 100.0 );
					reportTarget.page.setCustomHeight ( n1.firstChild().nodeValue().toDouble() / 100.0 );
				}
				reportTarget.page.setPageSize ( "Custom" );
			}
		}
		else if ( elemThis.tagName() == "labeltype" )
			reportTarget.page.setLabelType ( elemThis.firstChild().nodeValue() );
		else if ( elemThis.tagName() == "portrait" )
			reportTarget.page.setPortrait ( TRUE );
		else if ( elemThis.tagName() == "landscape" )
			reportTarget.page.setPortrait ( FALSE );
		else if ( elemThis.tagName() == "topmargin" )
		{
			d = elemThis.text().toDouble ( &valid );
			if ( !valid || d < 0.0 )
			{
				//TODO qDebug("Error converting topmargin value: %s",(const char*)elemThis.text());
				d = 50.0;
			}
			reportTarget.page.setMarginTop (POINT_TO_INCH( d ) * KoGlobal::dpiY());
		}
		else if ( elemThis.tagName() == "bottommargin" )
		{
			d = elemThis.text().toDouble ( &valid );
			if ( !valid || d < 0.0 )
			{
				//TODO qDebug("Error converting bottommargin value: %s",(const char*)elemThis.text());
				d = 50.0;
			}
			reportTarget.page.setMarginBottom (POINT_TO_INCH( d ) * KoGlobal::dpiY() );
		}
		else if ( elemThis.tagName() == "leftmargin" )
		{
			d = elemThis.text().toDouble ( &valid );
			if ( !valid || d < 0.0 )
			{
				//TODO qDebug("Error converting leftmargin value: %s",(const char*)elemThis.text());
				d = 50.0;
			}
			reportTarget.page.setMarginLeft ( POINT_TO_INCH( d ) * KoGlobal::dpiX() );
		}
		else if ( elemThis.tagName() == "rightmargin" )
		{
			d = elemThis.text().toDouble ( &valid );
			if ( !valid || d < 0.0 )
			{
				//TODO qDebug("Error converting rightmargin value: %s",(const char*)elemThis.text());
				d = 50.0;
			}
			reportTarget.page.setMarginRight ( POINT_TO_INCH( d ) * KoGlobal::dpiX());
		}
		else if ( elemThis.tagName() == "rpthead" )
		{
			KRSectionData * sd = new KRSectionData(elemThis);
			if ( sd->isValid() )
			{
				reportTarget.rpthead = sd;
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
				reportTarget.rptfoot = sd;
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
					reportTarget.pghead_first = sd;
				else if ( sd->extra() == "odd" )
					reportTarget.pghead_odd = sd;
				else if ( sd->extra() == "even" )
					reportTarget.pghead_even = sd;
				else if ( sd->extra() == "lastpage" )
					reportTarget.pghead_last = sd;
				else if ( sd->extra() == QString::null )
					reportTarget.pghead_any = sd;
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
					reportTarget.pgfoot_first = sd;
				else if ( sd->extra() == "odd" )
					reportTarget.pgfoot_odd = sd;
				else if ( sd->extra() == "even" )
					reportTarget.pgfoot_even = sd;
				else if ( sd->extra() == "lastpage" )
					reportTarget.pgfoot_last = sd;
				else if ( sd->extra() == QString::null )
					reportTarget.pgfoot_any = sd;
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
			ORDetailSectionData * dsd = new ORDetailSectionData();
			if ( parseReportDetailSection ( elemThis, *dsd ) == TRUE )
			{
				reportTarget.detailsection = dsd;
				//reportTarget.trackTotal += dsd->trackTotal;
			}
			else
				delete dsd;
		}

		//else
		//TODO qDebug("While parsing report encountered an unknown element: %s",(const char*)elemThis.tagName());
	}

	return TRUE;
}
