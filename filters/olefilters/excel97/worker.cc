/* This file is part of the KDE project

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>
#include <qdatetime.h>
#include "helper.h"
#include "worker.h"

#define ASSERT_SIZE(size,value)		\
	if ((size) != (value))		\
		{ kdWarning(30511) << __FUNCTION__ << "wanted<->got size mismatch: " << size << " != " << value << endl; }


Worker::Worker()
{
	m_root = new QDomDocument("spreadsheet");
	m_root->appendChild(m_root->createProcessingInstruction("xml", "version=\"1.0\" encoding =\"UTF-8\""));

	m_doc = m_root->createElement("spreadsheet");
	m_doc.setAttribute("editor", "KSpread");
	m_doc.setAttribute("mime", "application/x-kspread");
	m_root->appendChild(m_doc);

	m_paper = m_root->createElement("paper");
	m_paper.setAttribute("format", "A4");
	m_paper.setAttribute("orientation", "Portrait");
	m_doc.appendChild(m_paper);

	m_borders = m_root->createElement("borders");
	m_borders.setAttribute("left", 20);
	m_borders.setAttribute("top", 20);
	m_borders.setAttribute("right", 20);
	m_borders.setAttribute("bottom", 20);
	m_paper.appendChild(m_borders);

	m_map = m_root->createElement("map");
	m_doc.appendChild(m_map);

	m_mergeList.setAutoDelete(true);

	m_helper = new Helper(m_root, &m_tables);

	m_chartSeriesCount = 0;
	m_footerCount = 0;
	m_headerCount = 0;
	m_streamDepth = 0;
	m_chartDepth = 0;
	m_fontCount = 0;
	m_date1904 = 0;
	m_xfCount = 0;
	m_table = 0;
	m_biff = 0;
}

Worker::~Worker()
{
	delete m_root;
	delete m_helper;
}

const QDomDocument *const Worker::part()
{
	return m_root;
}

void Worker::done()
{
	m_helper->done();
}

bool Worker::op_1904(Q_UINT32 size, QDataStream &body)
{
	ASSERT_SIZE(size,sizeof(m_date1904));
	body >> m_date1904;
	m_helper->setDate1904(m_date1904);

	return true;
}

bool Worker::op_blank(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 row, column, xf;
	ASSERT_SIZE(size,sizeof(row)+sizeof(column)+sizeof(xf));
	body >> row >> column >> xf;

	QDomElement e = m_root->createElement("cell");
	e.appendChild(m_helper->getFormat(xf));
	e.setAttribute("row", (int) ++row);
	e.setAttribute("column", (int) ++column);
	if( m_table ) m_table->appendChild(e);

	return true;
}

bool Worker::op_bof(Q_UINT32, QDataStream &body)
{
	Q_UINT16 dt;
	Q_UINT16 rupBuild;
	Q_UINT16 rupYear;
	Q_UINT32 bfh;
	Q_UINT32 sfo;

	body >> m_biff;

	switch(m_biff)
	{
		case BIFF_5_7:
			body >> dt >> rupBuild >> rupYear;
			break;
		case BIFF_8:
			body >> dt >> rupBuild >> rupYear >> bfh >> sfo;
			break;
		default:
			return false;
	}

	m_streamDepth++;
	if(m_streamDepth == 1)
	{
		if(m_table != 0)
		{
			delete m_table;
			m_table = 0;
		}

		switch(dt)
		{
			case 0x5:
				kdDebug(30511) << "BOF: Workbook globals" << endl;
				break;
			case 0x10:
				m_table = m_tables.take(m_tables.find(m_tables.getFirst()));

				// FIXME: can happen as long as
				//        the boundsheet stuff isn't _fully_ implemented
				//		  (macrosheet etc..) (Niko)
				if(!m_table)
					break;

				kdDebug(30511) << "BOF: Worksheet: " << m_table->attribute("name") << endl;
				break;
			case 0x20:
				m_table = m_tables.take(m_tables.find(m_tables.getFirst()));
				// FIXME: can happen as long as
				//        the boundsheet stuff isn't _fully_ implemented
				//		  (macrosheet etc..) (Niko)
				if(!m_table)
					break;

				kdDebug(30511) << "BOF: Chart: " << m_table->attribute("name") << endl;
				break;
			default:
				kdWarning(30511) << "BOF: Unsupported substream: 0x" << QString::number(dt, 16) << endl;
				//        m_table = m_tables.first();
				//        delete m_table;
				m_table = 0;
				break;
		}
	}

	return true;
}

bool Worker::op_boolerr(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 row, column, xf;
	Q_UINT8 value, flag;
	ASSERT_SIZE(size,sizeof(row)+sizeof(column)+
		sizeof(xf)+sizeof(value)+sizeof(flag));
	body >> row >> column >> xf >> value >> flag;

	// boolean value
	if( flag == 0 )
	{
		QString str = value ? "True" : "False";
		QDomElement e = m_root->createElement("cell");
		e.appendChild(m_helper->getFormat(xf));
		e.setAttribute("row", (int) ++row);
		e.setAttribute("column", (int) ++column);
		QDomElement text = m_root->createElement("text");
		text.appendChild(m_root->createTextNode( str ));
		text.setAttribute("outStr",str);
		e.appendChild(text);
		if( m_table ) m_table->appendChild(e);
	}

	// TODO error value (flag==1)

	return true;
}

bool Worker::op_bottommargin(Q_UINT32 size, QDataStream &body)
{
	double valueInch;

	ASSERT_SIZE(size,sizeof(valueInch));
	body >> valueInch;

	m_borders.setAttribute("bottom", (valueInch * 2.54));

	return true;
}

bool Worker::op_boundsheet(Q_UINT32, QDataStream &body)
{
	Q_UINT32 lbPlyPos;
	Q_UINT16 grbit;
	Q_UINT8 cch;
	Q_UINT8 flag = 0;
	Q_UINT8 tmp8;
	QDomElement *e;

	switch(m_biff)
	{
		case BIFF_5_7:
			body >> lbPlyPos >> grbit >> tmp8;
			cch = tmp8;
			break;
		case BIFF_8:
			body >> lbPlyPos >> grbit >> cch >> flag;
			break;
		default:
			return false;
	}

	QString s; // name of sheet/macro/chart

	if( flag & 1 )
	{
		// Unicode
		for( int i=0; i<cch; i++ )
		{
			Q_UINT16 ch;
			body >> ch;
			s.append( QChar( ch ) );
		}
	}
	else
	{
		// Latin1
		for( int i=0; i<cch; i++ )
		{
			Q_UINT8 ch;
			body >> ch;
			s.append( (char)ch );
		}
	}

	if((grbit & 0x0f) == 0)
	{
		kdDebug(30511) << "Worksheet: " << s << " at: " << lbPlyPos << endl;
		e = new QDomElement(m_root->createElement("table"));
		e->setAttribute("name", s);

		// Hide the table if it is either hidden, or very hidden.
		if(((grbit >> 8) & 0x03) != 0)
			e->setAttribute("hide", true);
		
		m_map.appendChild(*e);
		m_tables.append(e);
	}
	else if((grbit & 0x0f) == 1)
	{
		// Macrosheet
		kdDebug(30511) << "Macrosheet: " << s << " at: " << lbPlyPos << "! UNIMPLEMENTED" << endl;
	}
	else if ((grbit & 0x0f) == 2)
	{
		// Chart
		kdDebug(30511) << "Chart: " << s << " at: " << lbPlyPos << "! UNIMPLEMENTED" << endl;
	}

	return true;
}

bool Worker::op_chart_fbi(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 xbase, ybase, height, scalebase, index;

	ASSERT_SIZE(size,5*sizeof(Q_UINT16));
	body >> xbase >> ybase >> height >> scalebase >> index;

	kdDebug(30511) << "CHART: XBase: " << xbase << " YBase: " << ybase << " Height: " 
			<< height << " ScaleBase: " << scalebase << " Index: " << index << endl;

	return true;
}

bool Worker::op_chart_units(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 type;

	ASSERT_SIZE(size,sizeof(type));
	body >> type;

	return (type == 0);
}

bool Worker::op_chart_chart(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 xposr, yposr, xsizer, ysizer;
	double xpos, ypos, xsize, ysize;

	ASSERT_SIZE(size,4*sizeof(Q_UINT16));
	body >> xposr >> yposr >> xsizer >> ysizer;

	xpos = xposr / (65535. * 72.);
	ypos = yposr / (65535. * 72.);
	xsize = xsizer / (65535. * 72.);
	ysize = ysizer / (65535. * 72.);

	kdDebug(30511) << "CHART: X: " << xpos << " Y: " << ypos << " X-Size: " << xsize << " Y-Size: " << ysize << endl;

	return true;
}

bool Worker::op_chart_begin(Q_UINT32, QDataStream &)
{
	m_chartDepth++;

	return true;
}

bool Worker::op_chart_plotgrowth(Q_UINT32 size, QDataStream &body)
{
	Q_INT16 skip, horizontal, vertical;

	ASSERT_SIZE(size,4*sizeof(Q_INT16));
	body >> skip >> horizontal >> skip >> vertical;

	if(horizontal != -1)
		kdDebug(30511) << "CHART: Horizontal-Scale: " << horizontal << endl;
	if(vertical != -1)
		kdDebug(30511) << "CHART: Vertical-Scale: " << vertical << endl;

	return true;
}

bool Worker::op_chart_frame(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 type, flags;
	bool borderShadow, autoSize, autoPos;

	ASSERT_SIZE(size,2*sizeof(Q_INT16));
	body >> type >> flags;

	borderShadow = (type == 4) ? true : false;
	autoSize = (flags & 0x01) ? true : false;
	autoPos = (flags & 0x02) ? true : false;

	kdDebug(30511) << "CHART: borderShadow: " << borderShadow << " autoSize: " << autoSize << " autoPos: " << autoPos << endl;

	return true;
}

bool Worker::op_chart_lineformat(Q_UINT32 size, QDataStream &body)
{
	Q_UINT32 color;
	Q_INT16 weight;
	Q_UINT16 pattern, flags, r, g, b;
	
	bool autoFormat, drawTicks;
	LinePattern pat;
	LineWeight wgt;

	ASSERT_SIZE(size,sizeof(color)+3*sizeof(Q_UINT16));
	body >> color;
	body >> pattern >> weight >> flags;

	if(pattern >= PAT_MODE_MAX)
	{
		kdDebug(30511) << "CHART: Pattern higher than PAT_MODE_MAX-1! Should never happen!" << endl;
		return false;
	}

	pat = (LinePattern) pattern;
	kdDebug(30511) << "CHART: Lines have a " << LinePatternText[pat] << " pattern" << endl;

	if(weight <= WGT_MODE_MIN)
	{
		kdDebug(30511) << "CHART: Weight lower than WGT_MODE_MIN-1! Should never happen!" << endl;
		return false;
	}

	if(weight >= WGT_MODE_MAX)
	{
		kdDebug(30511) << "CHART: Weight higher than WGT_MODE_MAX-1! Should never happen!" << endl;
		return false;
	}

	wgt = (LineWeight) weight;
	kdDebug(30511) << "CHART: Lines are " << LineWeightText[wgt + 1] << " wide" << endl;

	autoFormat = (flags & 0x01) ? true : false;
	drawTicks = (flags & 0x04) ? true : false;

	kdDebug(30511) << "CHART: autoFormat: " << autoFormat << " drawTicks: " << drawTicks << endl;

	r = (color >> 0) & 0xff;
	g = (color >> 8) & 0xff;
	b = (color >> 16) & 0xff;

	kdDebug(30511) << "CHART: Color R: " << r << " G: " << g << " B: " << b << endl;
	return true;
}

bool Worker::op_chart_areaformat(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 pattern, flags;
	Q_UINT32 skip;
	bool autoFormat, swapColors;

	ASSERT_SIZE(size,2*sizeof(skip)+2*sizeof(Q_UINT16));
	body >> skip >> skip;
	body >> pattern >> flags;

	autoFormat = (flags & 0x01) ? true : false;
	swapColors = (flags & 0x02) ? true : false;

	kdDebug(30511) << "CHART: autoFormat: " << autoFormat << " Swap-Colors-For-Negative-Numbers: " << swapColors << endl;

	return true;
}

bool Worker::op_chart_end(Q_UINT32, QDataStream &)
{
	m_chartDepth--;

	return true;
}

bool Worker::op_chart_series(Q_UINT32, QDataStream &body)
{
	Q_UINT16 cattype, catcount, valuetype, valuecount, bubblestype, bubblescount;

	body >> cattype >> valuetype;

	if(cattype >= CHART_SERIES_MAX)
	{
		kdDebug(30511) << "CHART: Category Type Number higher than CHART_SERIES_MAX-1! Should never happen!" << endl;
		return false;
	}

	if(valuetype >= CHART_SERIES_MAX)
	{
		kdDebug(30511) << "CHART: Value Type Number higher than CHART_SERIES_MAX-1! Should never happen!" << endl;
		return false;
	}

	body >> catcount >> valuecount;

	kdDebug(30511) << "CHART: " << catcount << " Categories are " << ChartSeriesText[cattype] << endl;
	kdDebug(30511) << "CHART: " << valuecount << " Values are " << ChartSeriesText[valuetype] << endl;

	if(m_biff >= BIFF_8)
	{
		body >> bubblestype >> bubblescount;

		if(bubblestype >= CHART_SERIES_MAX)
		{
			kdDebug(30511) << "CHART: Bubbles Type Number higher than CHART_SERIES_MAX-1! Should never happen!" << endl;
			return false;
		}

		kdDebug(30511) << "CHART: " << bubblescount << " Bubbles are " << ChartSeriesText[bubblestype] << endl;
	}

	return true;
}

bool Worker::op_chart_ai(Q_UINT32, QDataStream &body)
{
	Q_UINT8 linkType, refType;
	Q_UINT16 flags;

	body >> linkType >> refType >> flags;

	if(flags & 0x01)
	{
		kdDebug(30511) << "CHART: Has custom number format!" << endl;
		Q_UINT16 formatIndex;

		body >> formatIndex;

		formatrec *fmt = static_cast<formatrec *>(m_helper->queryDict(D_FORMAT, formatIndex));
		if(fmt)
			QString description = QString::fromLatin1(fmt->rgch, fmt->cch);
	}
	else
		kdDebug(30511) << "CHART: Using Number format from data source!" << endl;

	switch(linkType)
	{
		case 0:
			kdDebug(30511) << "CHART: Linking title or text" << endl;
			break;

		case 1:
			kdDebug(30511) << "CHART: Linking values" << endl;
			break;

		case 2:
			kdDebug(30511) << "CHART: Linking categories" << endl;
			break;

		case 3:
			kdDebug(30511) << "CHART: Linking bubbles" << endl;
			break;

		default:
			kdDebug(30511) << "CHART: Unknown linkType: " << linkType << endl;
			break;
	}

	switch(refType)
	{
		case 0:
			kdDebug(30511) << "CHART: Use default categories" << endl;
			break;

		case 1:
			kdDebug(30511) << "CHART: Text/Value entered directly" << endl;
			break;

		case 2:
			kdDebug(30511) << "CHART: Linked to container" << endl;
			break;

		default:
			kdDebug(30511) << "CHART: Unknown refType: " << refType << endl;
			break;
	}

	return true;
}

bool Worker::op_chart_dataformat(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 pointNumber, seriesIndex;

	ASSERT_SIZE(size,2*sizeof(Q_UINT16));
	body >> pointNumber >> seriesIndex;

	if(pointNumber == 0xffff)
		kdDebug(30511) << "CHART: All points use series " << seriesIndex << endl;
	else
		kdDebug(30511) << "CHART: Point " << pointNumber << " use series " << seriesIndex << endl;

	return true;
}

bool Worker::op_chart_3dbarshape(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 type;

	ASSERT_SIZE(size,sizeof(Q_UINT16));
	body >> type;

	switch(type)
	{
		case 0:
			kdDebug(30511) << "CHART: 3dbarshape is a, box" << endl;
			break;

		case 1:
			kdDebug(30511) << "CHART: 3dbarshape is a, cylinder" << endl;
			break;

		case 256:
			kdDebug(30511) << "CHART: 3dbarshape is a, pyramid" << endl;
			break;

		case 257:
			kdDebug(30511) << "CHART: 3dbarshape is a, cone" << endl;
			break;

		default:
			kdDebug(30511) << "CHART: 3dbarshape, type: " << type << ", unknown!" << endl;
	}

	return true;
}

bool Worker::op_chart_sertocrt(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 index;

	ASSERT_SIZE(size,sizeof(Q_UINT16));
	body >> index;

	kdDebug(30511) << "CHART: Series chart group index: " << index << endl;

	return true;
}

bool Worker::op_chart_shtprops(Q_UINT32 size, QDataStream &body)
{
	Q_UINT8 tmp;
	Q_UINT16 flags;
	bool manualFormat, onlyPlotVisibleCells, noResize, hasPositionRecord, ignorePosRecord = false;
	ChartBlank blanks;

	ASSERT_SIZE(size,sizeof(flags)+sizeof(tmp));
	body >> flags >> tmp;

	manualFormat = (flags & 0x01) ? true : false;
	onlyPlotVisibleCells = (flags & 0x02) ? true : false;
	noResize = (flags & 0x04) ? true : false;
	hasPositionRecord = (flags & 0x08) ? true : false;

	if(tmp >= CHART_BLANK_MAX)
	{
		kdDebug(30511) << "CHART: The 'tmp' field is higher than CHART_BLANK_MAX-1! Should never happen!" << endl;
		return false;
	}

	blanks = (ChartBlank) tmp;

	kdDebug(30511) << "CHART: Sheet Properties: " << ChartBlankText[tmp] << endl;


	if(m_biff >= BIFF_8)
		ignorePosRecord = (flags & 0x10) ? true : false;

	if(noResize)
		kdDebug(30511) << "CHART: Don't resize chart with window!" << endl;
	else
		kdDebug(30511) << "CHART: Resize chart with window!" << endl;

	if(hasPositionRecord && !ignorePosRecord)
		kdDebug(30511) << "CHART: We have a position record!" << endl;
	else if(hasPositionRecord && ignorePosRecord)
		kdDebug(30511) << "CHART: We have a position record, but ignore it!" << endl;

	if(manualFormat)
		kdDebug(30511) << "CHART: Manually formatted!" << endl;

	if(onlyPlotVisibleCells)
		kdDebug(30511) << "CHART: Only plot visible cells!" << endl;

	return true;
}

bool Worker::op_chart_axesused(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 axes;

	ASSERT_SIZE(size,sizeof(axes));
	body >> axes;

	kdDebug(30511) << "CHART: There are " << axes << " Axes!" << endl;

	return true;
}

bool Worker::op_chart_axisparent(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 index;
	Q_UINT32 x, y, w, h;

	ASSERT_SIZE(size,sizeof(index)+4*sizeof(Q_UINT32));
	body >> index;
	body >> x >> y >> w >> h;

	kdDebug(30511) << "CHART: Axis x,y: " << x << "," << y << " w,h: " << w << "," << h << endl;

	return true;
}

bool Worker::op_chart_axis(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 axisType;
	Axis ax;

	ASSERT_SIZE(size,sizeof(axisType));
	body >> axisType;

	if(axisType >= AXIS_MAX)
	{
		kdDebug(30511) << "CHART: The axisType is higher than AXIS_MAX-1! Should never happen!" << endl;
		return false;
	}

	ax = (Axis) axisType;

	kdDebug(30511) << "CHART: Found a " << AxisText[ax] << endl;

	return true;
}

bool Worker::op_chart_tick(Q_UINT32, QDataStream &body)
{
	Q_UINT8 major, minor, pos, flags = 0; // FIXME: flags!
	Q_UINT16 r, g, b;

	body >> major >> minor >> pos;

	switch(major)
	{
		case 0:
			kdDebug(30511) << "CHART: No major tick!" << endl;
			break;

		case 1:
			kdDebug(30511) << "CHART: Major tick inside axis!" << endl;
			break;

		case 2:
			kdDebug(30511) << "CHART: Major tick outside axis!" << endl;
			break;

		case 3:
			kdDebug(30511) << "CHART: Major tick across axis!" << endl;
			break;

		default:
			kdDebug(30511) << "CHART: Unknown major tick type: " << major << endl;
	}

	switch(minor)
	{
		case 0:
			kdDebug(30511) << "CHART: No minor tick!" << endl;
			break;

		case 1:
			kdDebug(30511) << "CHART: Minor tick inside axis!" << endl;
			break;

		case 2:
			kdDebug(30511) << "CHART: Minor tick outside axis!" << endl;
			break;

		case 3:
			kdDebug(30511) << "CHART: Minor tick across axis!" << endl;
			break;

		default:
			kdDebug(30511) << "CHART: Unknown minor tick type: " << minor << endl;
	}

	switch(pos)
	{
		case 0:
			kdDebug(30511) << "CHART: No tick label!" << endl;
			break;

		case 1:
			kdDebug(30511) << "CHART: Tick label at low end!" << endl;
			break;

		case 2:
			kdDebug(30511) << "CHART: Tick label at high end!" << endl;
			break;

		case 3:
			kdDebug(30511) << "CHART: Tick label near axis!" << endl;
			break;

		default:
			kdDebug(30511) << "CHART: Unknown tick label position: " << pos << endl;
	}


	if(flags & 0x02)
		kdDebug(30511) << "CHART: Auto text background mode!" << endl;
	else
	{
		Q_UINT8 mode;
		body >> mode;
		kdDebug(30511) << "CHART: Background mode: " << mode << endl;
	}
	if(flags & 0x01)
		kdDebug(30511) << "CHART: Auto tick label color!" << endl;
	else
	{
		Q_UINT16 color;
		body >> color;

		r = (color >> 0) & 0xff;
		g = (color >> 8) & 0xff;
		b = (color >> 16) & 0xff;

		kdDebug(30511) << "CHART: Tick label color: R: " << r << " G: " << g << " B: " << b << endl;
	}

	switch(flags & 0x1c)
	{
		case 0:
			kdDebug(30511) << "CHART: No rotation!" << endl;
			break;

		case 1:
			kdDebug(30511) << "CHART: Top to bottom rotation, letters upright!" << endl;
			break;

		case 2:
			kdDebug(30511) << "CHART: Rotate 90 deg. counter-clockwise!" << endl;
			break;

		case 3:
			kdDebug(30511) << "CHART: Rotate 90 deg. clockwise!" << endl;
			break;

		default:
			kdDebug(30511) << "CHART: Unknown rotation mode: " << (flags & 0x1c) << endl;
			break;
	}

	if(flags & 0x20)
		kdDebug(30511) << "CHART: Auto Rotate!" << endl;

	return true;
}

bool Worker::op_chart_axislineformat(Q_UINT32, QDataStream &body)
{
	Q_UINT16 type;

	body >> type;

	switch(type)
	{
		case 0:
			kdDebug(30511) << "CHART: Axisline is the standard axisline!" << endl;
			break;

		case 1:
			kdDebug(30511) << "CHART: Axisline is a major grid line along the axis!" << endl;
			break;

		case 2:
			kdDebug(30511) << "CHART: Axisline is a minor grid line along the axis!" << endl;
			break;

		case 3:
			kdDebug(30511) << "CHART: Axisline is a floor/wall along the axis!" << endl;
			break;

		default:
			kdDebug(30511) << "CHART: Axisline-Detetcion Error, unsupported type: " << type << endl;
			break;
	}

	return true;
}

bool Worker::op_chart_chartformat(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 flags, zorder, skipl;
	Q_UINT32 skip;
	bool varyColor;

	ASSERT_SIZE(size,4*sizeof(skip)+3*sizeof(Q_UINT16));
	body >> skip >> skip >> skip >> skip;
	body >> skipl >> flags >> zorder;

	varyColor = (flags & 0x01) ? true : false;

	kdDebug(30511) << "CHART: Z-Order: " << zorder << endl;

	if(varyColor)
		kdDebug(30511) << "CHART: Vary color of every data point!" << endl;

	return true;
}

bool Worker::op_chart_siindex(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 index;

	ASSERT_SIZE(size,sizeof(index));
	body >> index;

	m_chartSeriesCount++;

	kdDebug(30511) << "CHART: Series " << m_chartSeriesCount << " is " << index << endl;

	return true;
}

bool Worker::op_chart_legend(Q_UINT32 size, QDataStream &body)
{
	Q_UINT32 x, y, w, h;
	Q_UINT8 tmp;
	LegendLocation location;

	ASSERT_SIZE(size,4*sizeof(Q_UINT32)+sizeof(tmp));
	body >> x >> y >> w >> h;
	body >> tmp;

	if(tmp >= LEGEND_LOCATION_MAX || tmp == LEGEND_LOCATION_INVALID1 || tmp == LEGEND_LOCATION_INVALID2)
	{
		kdDebug(30511) << "CHART: The 'tmp' field is higher than LEGEND_LOCATION_MAX-1! (Or == invalid1 or 2) Should never happen!" << endl;
		return false;
	}

	location = (LegendLocation) tmp;

	kdDebug(30511) << "CHART: Legend is at " << LegendLocationText[location] << endl;
	kdDebug(30511) << "CHART: Legend x,y: " << x / 4000. << "," << y / 4000. << " w,h: " << w / 4000. << "," << h / 4000. << endl;

	return true;
}

bool Worker::op_chart_bar(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 spaceBetweenBar, spaceBetweenCategories, flags;
	bool horizontalBar, stacked, asPercentage, hasShadow = false;

	ASSERT_SIZE(size,3*sizeof(Q_UINT16));
	body >> spaceBetweenBar >> spaceBetweenCategories >> flags;

	horizontalBar = (flags & 0x01) ? true : false;
	stacked = (flags & 0x02) ? true : false;
	asPercentage = (flags & 0x04) ? true : false;

	if(horizontalBar)
		kdDebug(30511) << "CHART: Horizontal bar!" << endl;
	else
		kdDebug(30511) << "CHART: Vertical bar!" << endl;

	if(asPercentage)
		kdDebug(30511) << "CHART: Stacked Percentage. (" << stacked << " should be = true)" << endl;
	else if(stacked)
		kdDebug(30511) << "CHART: Stacked Percentage values." << endl;
	else
		kdDebug(30511) << "CHART: Overlayed values." << endl;

	kdDebug(30511) << "CHART: Space between bars = " << spaceBetweenBar << "% of width!" << endl;
	kdDebug(30511) << "CHART: Space between categories = " << spaceBetweenCategories << "% of width!" << endl;

	if(m_biff >= BIFF_8)
	{
		hasShadow = (flags & 0x04) ? true : false;
		if(hasShadow)
			kdDebug(30511) << "CHART: In 3D Mode!" << endl;
	}

	return true;
}

bool Worker::op_codepage(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 cv;

	ASSERT_SIZE(size,sizeof(cv));
	body >> cv;
	return true;
}

bool Worker::op_colinfo(Q_UINT32, QDataStream &body)
{
	Q_UINT16 first, last, width, xf, options;

	body >> first >> last >> width >> xf >> options;

	bool hidden = (options & 0x0001) ? true : false;

	for(Q_UINT32 i = first; i <= last; ++i)
	{
		QDomElement col = m_root->createElement("column");
		col.setAttribute("column", (int) i + 1);
		col.setAttribute("width", (int) width / 120);
		
		if(hidden)
			col.setAttribute("hide", hidden);
		
		col.appendChild(m_helper->getFormat(xf));
		if( m_table ) m_table->appendChild(col);
	}

	return true;
}

bool Worker::op_eof(Q_UINT32, QDataStream &)
{
	MergeInfo *merge = 0;

	QDomElement map = m_root->documentElement().namedItem("map").toElement();
	QDomNode n = map.firstChild();
	while(!n.isNull() && !m_mergeList.isEmpty())
	{
		QDomElement e = n.toElement();
		if(!e.isNull() && e.tagName() == "table")
		{
			QDomNode n2 = e.firstChild();
			while(!n2.isNull() && !m_mergeList.isEmpty())
			{
				QDomElement e2 = n2.toElement();
				if(!e2.isNull() && e2.tagName() == "cell")
				{
					QDomNode n3 = e2.firstChild();
					while(!n3.isNull() && !m_mergeList.isEmpty())
					{
						QDomElement e3 = n3.toElement();
						if(!e3.isNull() && e3.tagName() == "format")
						{
							int row = e2.attribute("row").toInt();
							int col = e2.attribute("column").toInt();
							
							for(merge = m_mergeList.first(); merge != 0; merge = m_mergeList.next())
							{
								if(row == merge->row() && col == merge->col())
								{
									e3.setAttribute("rowspan", QString::number(merge->rowspan()));
									e3.setAttribute("colspan", QString::number(merge->colspan()));

									m_mergeList.remove(m_mergeList.current());
									break;
								}
							}
						}
						
						n3 = n3.nextSibling();
					}
				}
				
				n2 = n2.nextSibling();
			}
		}
		
		n = n.nextSibling();
	}

	m_streamDepth--;
	return true;
}

bool Worker::op_externname(Q_UINT32, QDataStream &body)
{
	Q_UINT16 option, sheetIndex, dummy;
	Q_UINT8 len, flag, ch;

	body >> option;

	body >> sheetIndex >> dummy;

	// at the moment, only handle AddIn function
        // hence, in the case sheetIndex = 0
	if( sheetIndex != 0 ) return true;

	body >> len >> flag;


	QString name;
	for( unsigned int i=0; i<len; i++ )
	{
		body >> ch;
		name.append( QChar( ch ) );
	}

	m_helper->addName( name );

	return true;
}

bool Worker::op_filepass(Q_UINT32, QDataStream &body)
{
	Q_UINT16 temp;
	body >> temp >> temp >> temp;

	char *read = new char[16];
	body.readRawBytes(read, 16);

	QString documentId = QString::fromLatin1(read, 16);

	read = new char[16];
	body.readRawBytes(read, 16);

	QString saltData = QString::fromLatin1(read, 16);

	read = new char[16];
	body.readRawBytes(read, 16);

	QString hashedSaltData = QString::fromLatin1(read, 16);
	
	kdDebug() << "DOCUMENTID " << documentId << " SALTDATA " << saltData << " HASHEDSALTDATA " << hashedSaltData << endl;

	return false;
}

bool Worker::op_font(Q_UINT32, QDataStream &body)
{
	QChar *c;
	Q_UINT8 lsb, msb;

	fontrec *f = new fontrec();
	body >> f->dyHeight >> f->grbit >> f->icv >> f->bls >> f->sss;
	body >> f->uls >> f->bFamily >> f->bCharSet >> f->reserved >> f->cch;

	if(m_biff == BIFF_5_7)
	{
		for(int i = 0; i < f->cch; i++)
		{
			body >> lsb;
			c = new QChar(lsb, 0);
			f->rgch += *c;
		}
	}
	else // BIFF 8
	{
		body >> lsb;
		for(int i = 0; i < f->cch; i++)
		{
			body >> lsb >> msb;
			c = new QChar(lsb, msb);
			f->rgch += *c;
		}
	}

	m_helper->addDict(D_FONT, m_fontCount++, f);

	return true;
}

bool Worker::op_footer(Q_UINT32, QDataStream &body)
{
	if(m_footerCount++ == 0)
	{
		Q_UINT8 cch;

		body >> cch;
		if(!cch)
			return true;
		
		char *name = new char[cch];
		body.readRawBytes(name, cch);

		QString s = QString::fromLatin1(name, cch);

		QDomElement e = m_root->createElement("foot");
		QDomElement text = m_root->createElement("center");
		text.appendChild(m_root->createTextNode(s));
		e.appendChild(text);
		m_paper.appendChild(e);
		delete []name;
	}
	
	return true;
}

bool Worker::op_format(Q_UINT32, QDataStream &body)
{
	Q_UINT16 id;
	
	formatrec *f = new formatrec();
	body >> id >> f->cch;
	f->rgch = new char[f->cch];
	body.readRawBytes(f->rgch, f->cch);
	
	m_helper->addDict(D_FORMAT, id, f);

	return true;
}

bool Worker::op_formula(Q_UINT32 size, QDataStream &body)
{
	char *store = new char[size];
	Q_UINT16 row, column, xf, skip;
	QByteArray a;

	body >> row >> column >> xf;
	body >> skip >> skip >> skip >> skip >> skip >> skip >> skip >> skip;

	body.readRawBytes(store, size - 22);
	a.setRawData(store, size - 22);
	QDataStream fbody(a, IO_ReadOnly);
	fbody.setByteOrder(QDataStream::LittleEndian);

	QDomElement e = m_root->createElement("cell");
	e.appendChild(m_helper->getFormat(xf));
	e.setAttribute("row", (int) row + 1);
	e.setAttribute("column", (int) column + 1);

	QDomElement text = m_root->createElement("text");
	text.appendChild(m_root->createTextNode(m_helper->getFormula(row, column, fbody, m_biff)));
	e.appendChild(text);
	if( m_table ) m_table->appendChild(e);

	a.resetRawData(store, size - 22);
	delete []store;

	return true;
}

bool Worker::op_hcenter(Q_UINT32, QDataStream &body)
{
	Q_UINT16 flag;
	body >> flag;

	bool hcenter = (flag & 0x1) ? true : false;

	if(hcenter)
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Center horizontally when printing!" << endl;
	}
	else
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Don't center horizontally when printing!" << endl;
	}

	return true;
}

bool Worker::op_header(Q_UINT32, QDataStream &body)
{
	if(m_headerCount++ == 0)
	{
		Q_UINT8 cch;

		body >> cch;
		if(!cch)
			return true;

		char *name = new char[cch];
		body.readRawBytes(name, cch);

		QString s = QString::fromLatin1(name, cch);

		QDomElement e = m_root->createElement("head");
		QDomElement text = m_root->createElement("center");
		text.appendChild(m_root->createTextNode(s));
		e.appendChild(text);
		m_paper.appendChild(e);
		delete []name;
	}
	
	return true;
}

bool Worker::op_imdata(Q_UINT32, QDataStream &)
{
	/* Could be used to find out if this document was writing
	 * on Windows or on Macintosh, same for the bitmap format
	 */
	return true;
}

bool Worker::op_label(Q_UINT32, QDataStream &body)
{
	Q_UINT16 row, column, xf, length;

	body >> row >> column >> xf >> length;
	QDomElement e = m_root->createElement("cell");
	e.appendChild(m_helper->getFormat(xf));

	char *name = new char[length];
	body.readRawBytes(name, length);
	QString s = QString::fromLatin1(name, length);
	e.setAttribute("row", (int) ++row);
	e.setAttribute("column", (int) ++column);
	QDomElement text = m_root->createElement("text");
	text.appendChild(m_root->createTextNode(s));
	e.appendChild(text);
	if( m_table ) m_table->appendChild(e);

	delete []name;
	return true;
}

bool Worker::op_labelsst(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 row, column, xf;
	Q_UINT32 isst;

	ASSERT_SIZE(size, 3*sizeof(Q_UINT16)+sizeof(isst));
	body >> row >> column >> xf >> isst;
	QDomElement e = m_root->createElement("cell");
	e.appendChild(m_helper->getFormat(xf));

	e.setAttribute("row", (int) ++row);
	e.setAttribute("column", (int) ++column);

	QDomElement text = m_root->createElement("text");
	text.appendChild(m_root->createTextNode(*(static_cast<QString *>(m_helper->queryDict(D_SST, isst)))));
	e.appendChild(text);
	if( m_table ) m_table->appendChild(e);
	return true;
}

bool Worker::op_leftmargin(Q_UINT32, QDataStream &body)
{
	double valueInch;
	body >> valueInch;
	m_borders.setAttribute("left", (valueInch * 2.54));

	return true;
}

bool Worker::op_mergecell(Q_UINT32, QDataStream &body)
{
	Q_UINT16 count, firstrow, lastrow, firstcol, lastcol;
	body >> count;

	for(int i = 0; i < count; ++i)
	{
		body >> firstrow >> lastrow >> firstcol >> lastcol;
		m_mergeList.append(new MergeInfo(firstrow, lastrow, firstcol, lastcol));
	}

	return true;
}

bool Worker::op_mulblank(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 row, xf, count, first;

	body >> row >> first;
	count = (size - 6) / sizeof(xf);
	
	for(int i = 0; i < count; ++i)
	{
		body >> xf;
		QDomElement e = m_root->createElement("cell");
		e.appendChild(m_helper->getFormat(xf));
		e.setAttribute("row", row + 1);
		e.setAttribute("column", first + i + 1);
		if( m_table ) m_table->appendChild(e);
	}

	return true;
}


void Worker::rk_internal( int row, int column, Q_UINT16 xf, Q_UINT32 number )
{
	double value = m_helper->GetDoubleFromRK(number);
	int format = 0;

	xfrec *xwork = static_cast<xfrec *>(m_helper->queryDict(D_XF, xf));
	if(!xwork)
		kdError(30511) << "Missing format definition: " << xf << endl;
	else
		format = xwork->ifmt;
	// kdWarning(30511) << __FUNCTION__ << " xf/ifmt " << xf << "/" << format << endl;
	QString s = m_helper->formatValue(value, format);
	QDomElement e = m_root->createElement("cell");
	if( xwork ) e.appendChild(m_helper->getFormat(xf));
	e.setAttribute("row", row+1);
	e.setAttribute("column", column+1);
	QDomElement text = m_root->createElement("text");
	text.setAttribute("outStr", s);
	text.setAttribute("dataType","Num");
	text.appendChild(m_root->createTextNode(QString::number(value)));
	e.appendChild(text);
	if( m_table ) m_table->appendChild(e);
}

bool Worker::op_rk(Q_UINT32, QDataStream &body)
{
	Q_UINT32 number;
	Q_UINT16 row, column, xf;

	body >> row >> column >> xf >> number;
	rk_internal(row, column, xf, number);

	return true;
}

bool Worker::op_mulrk(Q_UINT32 size, QDataStream &body)
{
	QString s;

	if( size == 0 ) return true;

	Q_UINT32 number;
	Q_UINT16 column, row, xf = 0;

	body >> row >> column;

	int i, last = (size - 6) / (sizeof(xf)+sizeof(number));
        if( last > 0 )
	for(i = 0; i < last; ++i, ++column)	{
		body >> xf >> number;
		rk_internal(row, column, xf, number);
	}
	return true;
}

bool Worker::op_name(Q_UINT32, QDataStream &body)
{
	QString name;

	// only support BIFF 8 so far
	if( m_biff >= BIFF_8 )
	{
		Q_UINT16 option, sizeFormula, dummy, global;
		Q_UINT8 shortcutKey, lenName;
		Q_UINT8 lenMenu, lenDesc, lenHelp, lenStatus;

		body >> option >> shortcutKey;
		body >> lenName >> sizeFormula >> dummy >> global;
		body >> lenMenu >> lenDesc >> lenHelp >> lenStatus;

		for( unsigned i = 0; i < lenName+20; i++ )
		{
			Q_UINT8 ch; body >> ch;
			name.append( QChar( ch ) );
		}
		m_helper->addName( name );

	}
	return true;
}

bool Worker::op_note(Q_UINT32, QDataStream &body)
{
	Q_UINT16 row, col;

	body >> row >> col;
	kdDebug(30511) << "Note: col : " << ++col << "row :" << ++row <<endl;

	return true;
}

bool Worker::op_number(Q_UINT32, QDataStream &body)
{
	Q_UINT16 row, column, xf;
	double value;
	int format = 0;
	
	body >> row >> column >> xf >> value;
	
	xfrec *xwork = static_cast<xfrec *>(m_helper->queryDict(D_XF, xf));
	if(!xwork)
		kdError(30511) << "Missing format definition: " << xf << endl;
	else
		format = xwork->ifmt;
	QString s = m_helper->formatValue(value, format);
	QDomElement e = m_root->createElement("cell");
	if(xwork) e.appendChild(m_helper->getFormat(xf));
	e.setAttribute("row", row+1);
	e.setAttribute("column", column+1);

	QDomElement text = m_root->createElement("text");
	text.appendChild(m_root->createTextNode(s));
	e.appendChild(text);
	if( m_table ) m_table->appendChild(e);

	return true;
}

bool Worker::op_rightmargin(Q_UINT32, QDataStream &body)
{
	double valueInch;
	body >> valueInch;
	m_borders.setAttribute("right", (valueInch * 2.54));

	return true;
}



bool Worker::op_row(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 rowNr, skip, height, flags, flags2, xf;

	ASSERT_SIZE(size, 8*sizeof(Q_UINT16));
	body >> rowNr >> skip >> skip >> height >> flags >> flags >> flags >> flags2;

	xf = flags2 & 0xffff;

	xfrec *xwork = static_cast<xfrec *>(m_helper->queryDict(D_XF, xf));
	if(!xwork)
	{
		kdError(30511) << "Missing format definition: " << xf << " in row: " << rowNr << endl;
		xf = 0;
	}
	
	QDomElement row = m_root->createElement("row");
	row.setAttribute("row", rowNr+1);
	row.setAttribute("height", height/40);
	
	if(flags & 0x30)
		row.setAttribute("hide", true);

	if(flags & 0x80)
		row.appendChild(m_helper->getFormat(xf));
	
	if( m_table ) m_table->appendChild(row);

	return true;
}

bool Worker::op_setup(Q_UINT32, QDataStream &body)
{
	Q_UINT16 papersize, scale, resolution, verresolution, copies, flags;

	body >> papersize >> scale >> resolution >> verresolution >> copies >> flags;

	if((flags & 0x4) != 0x4)
	{
		if((flags & 0x40) != 0x40)
		{
			if((flags & 0x2) == 0x2)
			{
				m_paper.setAttribute("orientation", "portrait");
				kdDebug(30511) << "Printing Information: Orientation: Vertical!" << endl;
			}
			else
			{
				m_paper.setAttribute("orientation", "landscape");
				kdDebug(30511) << "Printing Information: Orientation: Horizontal!" << endl;
			}
		}
	}

	if((flags & 0x1) == 0x1)
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Print Order: Right then Down" << endl;
	}
	else
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Print Order: Down then Right" << endl;
	}

	if((flags & 0x8) == 0x8)
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Black and White only!" << endl;
	}
	else
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Color Printing!" << endl;
	}

	if((flags & 0x10) == 0x10)
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Print as Draft!" << endl;
	}

	if((flags & 0x20) == 0x20)
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Printing Comments allowed!" << endl;
	}

	return true;
}

bool Worker::op_shrfmla(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 firstrow, lastrow;
	Q_UINT8 firstcol, lastcol;
	Q_UINT16 dataLen, temp;

	ASSERT_SIZE(size,2*sizeof(Q_UINT16)+2*sizeof(Q_UINT8)+2*sizeof(Q_UINT16));
	body >> firstrow >> lastrow >> firstcol >> lastcol;
	body >> temp >> dataLen;

	char *store = new char[dataLen];
	body.readRawBytes(store, dataLen);
	
	QByteArray *a = new QByteArray();
	a->setRawData(store, dataLen);

	QDataStream *fbody = new QDataStream(*a, IO_ReadOnly);
	fbody->setByteOrder(QDataStream::LittleEndian);

	m_helper->addSharedFormula(new SharedFormula(firstrow, lastrow, firstcol, lastcol, fbody, a, store, dataLen));

	return true;
}

bool Worker::op_sst(Q_UINT32, QDataStream &body)
{
	char *buffer8bit;
	QString *s;
	Q_UINT8 grbit;
	Q_UINT16 cch, fRichSt, uniShort;
	Q_UINT32 count, extsst, richSt;

	body >> count >> extsst;
	for(int i = 0; i < (int) count; ++i)
	{
		body >> cch >> grbit;
		if(!(grbit & 0x01)) // compressed strings with 1 byte per character
		{
			fRichSt = 0;
			
			if(grbit & 0x08)
				body >> fRichSt;
			
			buffer8bit = new char[cch + 1];
			body.readRawBytes(buffer8bit, cch);
			
			buffer8bit[cch] = '\0';
			s = new QString(buffer8bit);

			m_helper->addDict(D_SST, i, s);
			
			delete[] buffer8bit;
			
			for(; fRichSt; --fRichSt)
				body >> richSt;
			
		}
		else if (grbit & 0x01) // not compressed; strings are unicode
		{
			// this has to be tested
			fRichSt = 0;
			
			if (grbit & 0x08)
				body >> fRichSt;
			
			s = new QString();
			
			for(int j = 0; j < cch; ++j)
			{
				body >> uniShort;
				*s += QChar(uniShort);
			}
		
			m_helper->addDict(D_SST, i, s);
			
			for (; fRichSt; --fRichSt)
				body >> richSt;
		}
	}

	return true;
}

bool Worker::op_standardwidth(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 width;

	ASSERT_SIZE(size,2);

	body >> width;
	kdDebug(30511) << "Standard width :" << width <<endl;

	return true;
}

bool Worker::op_defcolwidth(Q_UINT32 size, QDataStream &body)
{
	Q_UINT16 width;
	
	ASSERT_SIZE(size,2);
	
	body >> width;
	kdDebug(30511) << "Standard column width :" << width <<endl;

	return true;
}

bool Worker::op_string(Q_UINT32, QDataStream &body)
{
	Q_UINT8 temp;
	Q_UINT16 length;
	body >> length;
	body >> temp; // Skip first '0'

	QString result;
	for(int i = 0; i < length; i++)
	{
		body >> temp;
		result += QChar(temp);
	}

	kdDebug() << "RESULT " << result << endl;
	
	return true;
}

bool Worker::op_topmargin(Q_UINT32, QDataStream &body)
{
	double valueInch;
	body >> valueInch;
	m_borders.setAttribute("top", (valueInch * 2.54));

	return true;
}

bool Worker::op_vcenter(Q_UINT32, QDataStream &body)
{
	Q_UINT16 flag;
	body >> flag;

	bool vcenter = (flag & 0x1) ? true : false;

	if(vcenter)
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Center vertically when printing!" << endl;
	}
	else
	{
		// FIXME: Add functionality to kspread
		kdDebug(30511) << "Printing Information: Don't center vertically when printing!" << endl;
	}

	return true;
}

bool Worker::op_window2(Q_UINT32, QDataStream &body)
{
	Q_UINT16 nOpt;
	body >> nOpt;
	
	if(m_table != 0)
	{
		if(nOpt & 0x0001)
			m_table->setAttribute("formular", 1);
		else
			m_table->setAttribute("formular", 0);

		if(nOpt & 0x0002)
			m_table->setAttribute("grid", 1);
		else
			m_table->setAttribute("grid", 0);

		if(nOpt & 0x0004)
			kdDebug(30511) << "Show col/row header" << endl;
		else
			kdDebug(30511) << "Hide col/row header. Not store in table" << endl;

		if(nOpt & 0x0010)
			m_table->setAttribute("hidezero", 0);
		else
			m_table->setAttribute("hidezero", 1);
	}

	return true;
}

bool Worker::op_writeaccess(Q_UINT32, QDataStream &body)
{
	Q_UINT8 length;

	body >> length;

	if(m_biff == BIFF_8)
	{
		Q_UINT16 skip;
		body >> skip;
	}

	char *name = new char[length];
	body.readRawBytes(name, length);
	// FIXME: Port!
	//emit gotAuthor(QString::fromLatin1(name, length));

	delete []name;

	return true;
}

bool Worker::op_xf(Q_UINT32, QDataStream &body)
{
	xfrec *x = new xfrec();

	body >> x->ifnt >> x->ifmt >> x->attr >> x->align >> x->indent;
	body >> x->borderStyle >> x->sideBColor >> x->topBColor >> x->cellColor;

	m_helper->addDict(D_XF, m_xfCount++, x);

	return true;
}
