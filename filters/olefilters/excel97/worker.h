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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef WORKER_H
#define WORKER_H

#include <qdatastream.h>
#include <qptrlist.h>
#include <qintdict.h>
#include <qdom.h>
#include <qstringlist.h>

#include "definitions.h"

class MergeInfo;
class Helper;
class Worker
{
public:
	Worker();
	~Worker();

	void done();
	const QDomDocument *const part();
	
protected:
	friend class Handler;

	bool op_1904(Q_UINT32 size, QDataStream &body);
	bool op_blank(Q_UINT32 size, QDataStream &body);
	bool op_bof(Q_UINT32 size, QDataStream &body);
	bool op_boolerr(Q_UINT32 size, QDataStream &body);
	bool op_bottommargin(Q_UINT32 size, QDataStream &body);
	bool op_boundsheet(Q_UINT32 size, QDataStream &body);
	bool op_chart_fbi(Q_UINT32 size, QDataStream &body);
	bool op_chart_units(Q_UINT32 size, QDataStream &body);
	bool op_chart_chart(Q_UINT32 size, QDataStream &body);
	bool op_chart_begin(Q_UINT32 size, QDataStream &body);
	bool op_chart_plotgrowth(Q_UINT32 size, QDataStream &body);
	bool op_chart_frame(Q_UINT32 size, QDataStream &body);
	bool op_chart_lineformat(Q_UINT32 size, QDataStream &body);
	bool op_chart_areaformat(Q_UINT32 size, QDataStream &body);
	bool op_chart_end(Q_UINT32 size, QDataStream &body);
	bool op_chart_series(Q_UINT32 size, QDataStream &body);
	bool op_chart_ai(Q_UINT32 size, QDataStream &body);
	bool op_chart_dataformat(Q_UINT32 size, QDataStream &body);
	bool op_chart_3dbarshape(Q_UINT32 size, QDataStream &body);
	bool op_chart_sertocrt(Q_UINT32 size, QDataStream &body);
	bool op_chart_shtprops(Q_UINT32 size, QDataStream &body);
	bool op_chart_axesused(Q_UINT32 size, QDataStream &body);
	bool op_chart_axisparent(Q_UINT32 size, QDataStream &body);
	bool op_chart_axis(Q_UINT32 size, QDataStream &body);
	bool op_chart_tick(Q_UINT32 size, QDataStream &body);
	bool op_chart_axislineformat(Q_UINT32 size, QDataStream &body);
	bool op_chart_chartformat(Q_UINT32 size, QDataStream &body);
	bool op_chart_siindex(Q_UINT32 size, QDataStream &body);
	bool op_chart_legend(Q_UINT32 size, QDataStream &body);
	bool op_chart_bar(Q_UINT32 size, QDataStream &body);
	bool op_codepage(Q_UINT32 size, QDataStream &body);
	bool op_colinfo(Q_UINT32 size, QDataStream &body);
	bool op_eof(Q_UINT32 size, QDataStream &body);
	bool op_externname(Q_UINT32 size, QDataStream &body);
	bool op_filepass(Q_UINT32 size, QDataStream &body);
	bool op_font(Q_UINT32 size, QDataStream &body);
	bool op_footer(Q_UINT32 size, QDataStream &body);
	bool op_format(Q_UINT32 size, QDataStream &body);
	bool op_formula(Q_UINT32 size, QDataStream &body);
	bool op_hcenter(Q_UINT32 size, QDataStream &body);
	bool op_header(Q_UINT32 size, QDataStream &body);
	bool op_imdata(Q_UINT32 size, QDataStream &body);
	bool op_label(Q_UINT32 size, QDataStream &body);
	bool op_labelsst(Q_UINT32 size, QDataStream &body);
	bool op_leftmargin(Q_UINT32 size, QDataStream &body);
	bool op_mergecell(Q_UINT32 size, QDataStream &body);
	bool op_mulblank(Q_UINT32 size, QDataStream &body);
	bool op_mulrk(Q_UINT32 size, QDataStream &body);
	bool op_name(Q_UINT32 size, QDataStream &body);
	bool op_note(Q_UINT32 size, QDataStream &body);
	bool op_number(Q_UINT32 size, QDataStream &body);
	bool op_rightmargin(Q_UINT32 size, QDataStream &body);
	bool op_rk(Q_UINT32 size, QDataStream &body);
	bool op_row(Q_UINT32 size, QDataStream &body);
	bool op_setup(Q_UINT32 size, QDataStream &body);
	bool op_shrfmla(Q_UINT32 size, QDataStream &body);
	bool op_sst(Q_UINT32 size, QDataStream &body);
	bool op_standardwidth(Q_UINT32 size, QDataStream &body);
	bool op_defcolwidth(Q_UINT32 size, QDataStream &body);
	bool op_string(Q_UINT32 size, QDataStream &body);
	bool op_topmargin(Q_UINT32 size, QDataStream &body);
	bool op_vcenter(Q_UINT32 size, QDataStream &body);
	bool op_window2(Q_UINT32 size, QDataStream &body);
	bool op_writeaccess(Q_UINT32 size, QDataStream &body);
	bool op_xf(Q_UINT32 size, QDataStream &body);

private:
	QDomDocument *m_root;
	QDomElement m_doc, m_paper, m_map, m_borders, *m_table;

	void rk_internal( int row, int column, Q_UINT16 xf, Q_UINT32 number );
		
	QPtrList<QDomElement> m_tables;
	QPtrList<MergeInfo> m_mergeList;

	Helper *m_helper;

	unsigned int m_chartSeriesCount;
	unsigned int m_streamDepth;
	unsigned int m_chartDepth;

	int m_fontCount, m_footerCount, m_headerCount, m_xfCount;

	Q_UINT16 m_biff, m_date1904;
};

#endif
