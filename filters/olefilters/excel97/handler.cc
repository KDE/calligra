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

#include <kdebug.h>
#include <qdatastream.h>
#include <qstringlist.h>
#include "handler.h"

Handler::Handler()
{
	m_worker = new Worker();
}

Handler::~Handler()
{
	delete m_worker;
}

Worker *Handler::worker()
{
	return m_worker;
}

bool Handler::invokeHandler(Q_UINT16 opcode, Q_UINT32 bytes, QDataStream &operands)
{
	typedef bool (Worker::*Method)(Q_UINT32 size, QDataStream &operands);

	typedef struct
	{
		const char *name;
		Q_UINT16 opcode;
		Method handler;
	} OPCode;

	static const OPCode functions[] =
	{
		{ "1904",   	        	0x0022, &Worker::op_1904 },
		{ "ADDIN",          		0x0087, 0 /* &Worker::op_addin */ },
		{ "ADDMENU",        		0x00c1, 0 /* &Worker::op_addmenu */ },
		{ "ARRAY",					0x0021, 0 /* &Worker::op_array */ },
		{ "AUTOFILTERINFO",			0x009d, 0 /* &Worker::op_autofilterinfo */ },
		{ "AUTOFILTER",				0x009e, 0 /* &Worker::op_autofilter */ },
		{ "BACKUP",					0x0040, 0 /* &Worker::op_backup */ },
		{ "BLANK",					0x0001, &Worker::op_blank },
		{ "BOF",					0x0009, &Worker::op_bof },
		{ "BOOKBOOL",				0x00da, 0 /* &Worker::op_bookbool */ },
		{ "BOOLERR",				0x0005, &Worker::op_boolerr },
		{ "BOTTOMMARGIN",			0x0029, &Worker::op_bottommargin },
		{ "BOUNDSHEET",				0x0085, &Worker::op_boundsheet },
		{ "CALCCOUNT",				0x000c, 0 /* &Worker::op_calccount */ },
		{ "CALCMODE",				0x000d, 0 /* &Worker::op_calcmode */ },
		{ "CF",						0x01b1, 0 /* &Worker::op_cf */ },
		{ "CHART-UNITS",			0x1001, &Worker::op_chart_units },
		{ "CHART-CHART",			0x1002, &Worker::op_chart_chart },
		{ "CHART-SERIES",			0x1003, &Worker::op_chart_series },
		{ "CHART-DATAFORMAT",		0x1006, &Worker::op_chart_dataformat },
		{ "CHART-LINEFORMAT",		0x1007, &Worker::op_chart_lineformat },
		{ "CHART-MARKERFORMAT",		0x1009, 0 /* &Worker::op_chart_markerformat */ },
		{ "CHART-AREAFORMAT",		0x100a, &Worker::op_chart_areaformat },
		{ "CHART-PIEFORMAT",		0x100b, 0 /* &Worker::op_chart_pieformat */ },
		{ "CHART-ATTACHEDLABEL",	0x100c, 0 /* &Worker::op_chart_attachedlabel */ },
		{ "CHART-SERIESTEXT",		0x100d, 0 /* &Worker::op_chart_seriestext */ },
		{ "CHART-CHARTFORMAT",		0x1014, &Worker::op_chart_chartformat },
		{ "CHART-LEGEND",			0x1015, &Worker::op_chart_legend },
		{ "CHART-SERIESLIST",		0x1016, 0 /* &Worker::op_chart_serieslist */ },
		{ "CHART-BAR",				0x1017, &Worker::op_chart_bar },
		{ "CHART-LINE",				0x1018, 0 /* &Worker::op_chart_line */ },
		{ "CHART-PIE",				0x1019, 0 /* &Worker::op_chart_pie */ },
		{ "CHART-AREA",				0x101a, 0 /* &Worker::op_chart_area */ },
		{ "CHART-SCATTER",			0x101b, 0 /* &Worker::op_chart_scatter */ },
		{ "CHART-CHARTLINE",		0x101c, 0 /* &Worker::op_chart_chartline */ },
		{ "CHART-AXIS",				0x101d, &Worker::op_chart_axis },
		{ "CHART-TICK",				0x101e, &Worker::op_chart_tick },
		{ "CHART-VALUERANGE",		0x101f, 0 /* &Worker::op_chart_valuerange */ },
		{ "CHART-CATSERRANGE",		0x1020, 0 /* &Worker::op_chart_catserrange */ },
		{ "CHART-AXISLINEFORMAT",	0x1021, &Worker::op_chart_axislineformat },
		{ "CHART-CHARTFORMATLINK",	0x1022, 0 /* &Worker::op_chart_chartformatlink */ },
		{ "CHART-DEFAULTTEXT",		0x1024, 0 /* &Worker::op_chart_defaulttext */ },
		{ "CHART-TEXT",				0x1025, 0 /* &Worker::op_chart_text */ },
		{ "CHART-FONTX",			0x1026, 0 /* &Worker::op_chart_fontx */ },
		{ "CHART-OBJECTLINK",		0x1027, 0 /* &Worker::op_chart_objectlink */ },
		{ "CHART-FRAME",			0x1032, &Worker::op_chart_frame },
		{ "CHART-BEGIN",			0x1033, &Worker::op_chart_begin },
		{ "CHART-END",				0x1034, &Worker::op_chart_end },
		{ "CHART-PLOTAREA",			0x1035, 0 /* &Worker::op_chart_plotarea */ },
		{ "CHART-3D",				0x103a, 0 /* &Worker::op_chart_3d */ },
		{ "CHART-PICF",				0x103c, 0 /* &Worker::op_chart_picf */ },
		{ "CHART-DROPBAR",			0x103d, 0 /* &Worker::op_chart_dropbar */ },
		{ "CHART-RADAR",			0x103e, 0 /* &Worker::op_chart_radar */ },
		{ "CHART-SURF",				0x103f, 0 /* &Worker::op_chart_surf */ },
		{ "CHART-RADARAREA",		0x1040, 0 /* &Worker::op_chart_radararea */ },
		{ "CHART-AXISPARENT",		0x1041, &Worker::op_chart_axisparent },
		{ "CHART-LEGENDXN",			0x1043, 0 /* &Worker::op_chart_legendxn */ },
		{ "CHART-SHTPROPS",			0x1044, &Worker::op_chart_shtprops },
		{ "CHART-SERTOCRT",			0x1045, &Worker::op_chart_sertocrt },
		{ "CHART-AXESUSED",			0x1046, &Worker::op_chart_axesused },
		{ "CHART-SBASEREF",			0x1048, 0 /* &Worker::op_chart_sbaseref */ },
		{ "CHART-SERPARENT",		0x104a, 0 /* &Worker::op_chart_serparent */ },
		{ "CHART-SERAUXTREND",		0x104b, 0 /* &Worker::op_chart_serauxtrend */ },
		{ "CHART-IFMT",				0x104e, 0 /* &Worker::op_chart_ifmt */ },
		{ "CHART-POS",				0x104f, 0 /* &Worker::op_chart_pos */ },
		{ "CHART-ALRUNS",			0x1050, 0 /* &Worker::op_chart_alruns */ },
		{ "CHART-AI",				0x1051, &Worker::op_chart_ai },
		{ "CHART-SERAUXERRBAR",		0x105b, 0 /* &Worker::op_chart_serauxerrbar */ },
		{ "CHART-CLRTCLIENT",		0x105c, 0 /* &Worker::op_chart_clrtclient */ },
		{ "CHART-SERFMT",			0x105d, 0 /* &Worker::op_chart_serfmt */ },
		{ "CHART-3DBARSHAPE",		0x105f, &Worker::op_chart_3dbarshape },
		{ "CHART-FBI",				0x1060, &Worker::op_chart_fbi },
		{ "CHART-BOPPOP",			0x1061, 0 /* &Worker::op_chart_boppop */ },
		{ "CHART-AXCEXT",			0x1062, 0 /* &Worker::op_chart_axcext */ },
		{ "CHART-DAT",				0x1063, 0 /* &Worker::op_chart_dat */ },
		{ "CHART-PLOTGROWTH",		0x1064, &Worker::op_chart_plotgrowth },
		{ "CHART-SIINDEX",			0x1065, &Worker::op_chart_siindex },
		{ "CHART-GELFRAME",			0x1066, 0 /* &Worker::op_chart_gelframe */ },
		{ "CHART-BOPPOPCUSTOM",		0x1067, 0 /* &Worker::op_chart_boppopcustom */ },
		{ "CONDFMT",				0x01b0, 0 /* &Worker::op_condfmt */ },
		{ "CODENAME",				0x00ba, 0 /* &Worker::op_codename */ },
		{ "CODEPAGE",				0x0042, &Worker::op_codepage },
		{ "COLINFO",				0x007d, &Worker::op_colinfo },
		{ "CONTINUE",				0x003c, 0 /* &Worker::op_continue */ },
		{ "COORDLIST",				0x00a9, 0 /* &Worker::op_coordlist */ },
		{ "COUNTRY",				0x008c, 0 /* &Worker::op_country */ },
		{ "CRN",					0x005a, 0 /* &Worker::op_crn */ },
		{ "DBCELL",					0x00d7, 0 /* &Worker::op_dbcell */ },
		{ "DCON",					0x0050, 0 /* &Worker::op_dcon */ },
		{ "DCONREF",				0x0051, 0 /* &Worker::op_dconref */ },
		{ "DCONNAME",				0x0052, 0 /* &Worker::op_dconname */ },
		{ "DELMENU",				0x00c3, 0 /* &Worker::op_delmenu */ },
		{ "DEFAULTROWHEIGHT",		0x0025, 0 /* &Worker::op_defaultrowheight */ },
		{ "DEFCOLWIDTH",			0x0055, &Worker::op_defcolwidth },
		{ "DELTA",					0x0010, 0 /* &Worker::op_delta */ },
		{ "DOCROUTE",				0x00b8, 0 /* &Worker::op_docroute */ },
		{ "DSF",					0x0061, 0 /* &Worker::op_dsf */ },
		{ "COLWIDTH",				0x0024, 0 /* &Worker::op_colwidth */ },
		{ "COLUMNDEFAULT",			0x0020, 0 /* &Worker::op_coldefault */ },
		{ "DIMENSIONS",				0x0000, 0 /* &Worker::op_dimensions */ },
		{ "EDG",					0x0088, 0 /* &Worker::op_edg */ },
		{ "EOF",					0x000a, &Worker::op_eof },
		{ "EXTERNCOUNT",			0x0016, 0 /* &Worker::op_externcount */ },
		{ "EXTERNNAME",				0x0023, &Worker::op_externname },
		{ "EXTERNSHEET",			0x0017, 0 /* &Worker::op_externsheet */ },
		{ "EXTSST",					0x00ff, 0 /* &Worker::op_extsst */ },
		{ "FILEPASS",				0x002f, &Worker::op_filepass },
		{ "FILESHARING",			0x005b, 0 /* &Worker::op_filesharing */ },
		{ "FILESHARING2",			0x00a5, 0 /* &Worker::op_filesharing2 */ },
		{ "FILTERMODE",				0x009b, 0 /* &Worker::op_filtermode */ },
		{ "FNGROUPCOUNT",			0x009c, 0 /* &Worker::op_fngroupcount */ },
		{ "FNGROUPNAME",			0x009a, 0 /* &Worker::op_fngroupname */ },
		{ "FONT",					0x0031, &Worker::op_font },
		{ "FONT2",					0x0032, 0 /* &Worker::op_font2 */ },
		{ "FOOTER",					0x0015, &Worker::op_footer },
		{ "FORMAT",					0x001e, &Worker::op_format },
		{ "FORMATCOUNT",			0x001f, 0 /* &Worker::op_formatcount */ },
		{ "FORMULA",				0x0006, &Worker::op_formula },
		{ "GCW",					0x00ab, 0 /* &Worker::op_gcw */ },
		{ "GRIDSET",				0x0082, 0 /* &Worker::op_gridset */ },
		{ "GUTS",					0x0080, 0 /* &Worker::op_guts */ },
		{ "HCENTER",				0x0083, &Worker::op_hcenter },
		{ "HEADER",					0x0014, &Worker::op_header },
		{ "HIDEOBJ",				0x008d, 0 /* &Worker::op_hideobj */ },
		{ "HLINK",					0x01b8, 0 /* &Worker::op_hlink */},
		{ "HORIZONTALPAGEBREAKS",	0x001b, 0 /* &Worker::op_horizontalpagebreaks */ },
		{ "IMDATA",					0x007f, &Worker::op_imdata },
		{ "INDEX",					0x000b, 0 /* &Worker::op_index */ },
		{ "INTEGER",				0x0002, 0 /* &Worker::op_integer */ },
		{ "INTERFACEHDR",			0x00e1, 0 /* &Worker::op_interfacehdr */ },
		{ "INTERFACEEND",			0x00e2, 0 /* &Worker::op_interfaceend */ },
		{ "ITERATION",				0x0011, 0 /* &Worker::op_iteration */ },
		{ "LABELSST",				0x00fd, &Worker::op_labelsst },
		{ "LABEL",					0x0004, &Worker::op_label },
		{ "LEFTMARGIN",				0x0026, &Worker::op_leftmargin },
		{ "LHRECORD",				0x0094, 0 /* &Worker::op_lhrecord */ },
		{ "LHNGRAPH",				0x0095, 0 /* &Worker::op_lhngraph */ },
		{ "LPR",					0x0098, 0 /* &Worker::op_lpr */ },
		{ "MERGECELL",				0x00e5, &Worker::op_mergecell },
		{ "MMS",					0x00c1, 0 /* &Worker::op_mms */ },
		{ "MSOD",					0x00ec, 0 /* &Worker::op_msod */ },
		{ "MSODGROUP",				0x00eb, 0 /* &Worker::op_msodgroup */ },
		{ "MSODSEL",				0x00ed, 0 /* &Worker::op_msodsel */ },
		{ "MULBLANK",				0x00be, &Worker::op_mulblank },
		{ "MULRK",					0x00bd, &Worker::op_mulrk },
		{ "NAME",					0x0018, &Worker::op_name },
		{ "NOTE",					0x001c, &Worker::op_note },
		{ "NUMBER",					0x0003, &Worker::op_number },
		{ "OBJ",					0x005d, 0 /* &Worker::op_obj */ },
		{ "OBJPROJ",				0x00d3, 0 /* &Worker::op_objproj */ },
		{ "OBJPROTECT",				0x0063, 0 /* &Worker::op_objprotect */ },
		{ "OLESIZE",				0x00de, 0 /* &Worker::op_olesize */ },
		{ "PALETTEINFO",			0x0092, 0 /* &Worker::op_paletteinfo */ },
		{ "PANE",					0x0041, 0 /* &Worker::op_pane */ }, 
		{ "PARAMQRY",				0x00dc, 0 /* &Worker::op_paramqry */ }, 
		{ "PASSWORD",				0x0013, 0 /* &Worker::op_password */ },
		{ "PLS",					0x004d, 0 /* &Worker::op_pls */ },
		{ "PRECISION",				0x000e, 0 /* &Worker::op_precision */ },
		{ "PRINTGRIDLINES",			0x002b, 0 /* &Worker::op_printgridlines */ },
		{ "PRINTROWHEADERS",		0x002a, 0 /* &Worker::op_printrowheaders */ },
		{ "PRINTSIZE",				0x0033, 0 /* &Worker::op_printsize */ },
		{ "PROTECT",				0x0012, 0 /* &Worker::op_protect */ },
		{ "PUB",					0x0089, 0 /* &Worker::op_pub */ },
		{ "QSI",					0x00ad, 0 /* &Worker::op_qsi */ },
		{ "RECIPNAME",				0x00b9, 0 /* &Worker::op_recipname */ },
		{ "REFMODE",				0x000f, 0 /* &Worker::op_refomde */ },
		{ "REFRESHALL",				0x00b7, 0 /* &Worker::op_refreshall */ },
		{ "RIGHTMARGIN",			0x0027, &Worker::op_rightmargin },
		{ "RK",						0x007e, &Worker::op_rk },
		{ "ROW",					0x0008, &Worker::op_row },
		{ "RSTRING",				0x00d6, 0 /* &Worker::op_rstring */ },
		{ "SAVERECALC",				0x005f, 0 /* &Worker::op_saverecalc */ },
		{ "SCENMAN",				0x00ae, 0 /* &Worker::op_scenman */ },
		{ "SCENARIO",				0x00af, 0 /* &Worker::op_scenario */ },
		{ "SCENPROTECT",			0x00dd, 0 /* &Worker::op_scenprotect */ },
		{ "SCL",					0x00a0,	0 /* &Worker::op_scl */ },
		{ "SELECTION",				0x001d, 0 /* &Worker::op_selection */ },
		{ "SETUP",					0x00a1, &Worker::op_setup },
		{ "SHRFMLA",				0x00bc, &Worker::op_shrfmla },
		{ "SORT",					0x0090, 0 /* &Worker::op_sort */ },
		{ "SOUND",					0x0096, 0 /* &Worker::op_sound */ },
		{ "SUB",					0x0091, 0 /* &Worker::op_sub */ },
		{ "SST",					0x00fc, &Worker::op_sst },
		{ "STANDARDWIDTH",			0x0099, &Worker::op_standardwidth },
		{ "STRING",					0x0007, &Worker::op_string },
		{ "STYLE",					0x0093, 0 /* &Worker::op_style */ },
		{ "SXDB",					0x00c6, 0 /* &Worker::op_sxdb */ },
		{ "SXDI",					0x00c5, 0 /* &Worker::op_sxdi */ },
		{ "SXEX",					0x00f1, 0 /* &Worker::op_sxex */ },
		{ "SXFILT",					0x00f2, 0 /* &Worker::op_sxfilt */ },
		{ "SXFMLA",					0x00f9, 0 /* &Worker::op_sxfmla */ },
		{ "SXFORMAT",				0x00fb, 0 /* &Worker::op_sxformat */ },
		{ "SXIDSTM",				0x00d5, 0 /* &Worker::op_sxidstm */ },
		{ "SXLI",					0x00b5, 0 /* &Worker::op_sxli */ },
		{ "SXIVD",					0x00b4, 0 /* &Worker::op_sxivd */ },
		{ "SXNAME",					0x00f6, 0 /* &Worker::op_sxname */ },
		{ "SXPAIR",					0x00f8, 0 /* &Worker::op_sxpair */ },
		{ "SXPI",					0x00b6, 0 /* &Worker::op_sxpi */ },
		{ "SXRULE",					0x00f0, 0 /* &Worker::op_sxrule */ },
		{ "SXSELECT",				0x00f7, 0 /* &Worker::op_sxselect */ },
		{ "SXSTRING",				0x00cd, 0 /* &Worker::op_sxstring */ },
		{ "SXTBL",					0x00d0, 0 /* &Worker::op_sxtbl */ },
		{ "SXTBRGIITM",				0x00d1, 0 /* &Worker::op_sxtbrgiitm */ },
		{ "SXTBPG",					0x00d2, 0 /* &Worker::op_sxtbpg */ },
		{ "SXVD",					0x00b1, 0 /* &Worker::op_sxvd */ },
		{ "SXVI",					0x00b2, 0 /* &Worker::op_sxvi */ },
		{ "SXVIEW",					0x00b0, 0 /* &Worker::op_sxview */ },
		{ "SXVX",					0x00e3, 0 /* &Worker::op_sxvx */ },
		{ "TABID",					0x003d, 0 /* &Worker::op_tabid */ },
		{ "TABIDCONF",				0x00ea, 0 /* &Worker::op_tabidconf */ },
		{ "TABLE",					0x0036, 0 /* &Worker::op_table */ },
		{ "TABLE2",					0x0037, 0 /* &Worker::op_table2 */ },
		{ "TEMPLATE",				0x0060, 0 /* &Worker::op_template */ },
		{ "TOPMARGIN",				0x0028, &Worker::op_topmargin },
		{ "UDDESC",					0x00df, 0 /* &Worker::op_uddesc */ },
		{ "UNCALCED",				0x005e, 0 /* &Worker::op_uncalced */ },
		{ "VCENTER",				0x0084, &Worker::op_vcenter },
		{ "VERTICALPAGEBREAKS",		0x001a, 0 /* &Worker::op_verticalpagebreaks */ },
		{ "WINDOW1",				0x003d, 0 /* &Worker::op_window1 */ },
		{ "WINDOW2",				0x003e, &Worker::op_window2 },
		{ "WINDOWPROT",				0x0019, 0 /* &Worker::op_windowprot */ },
		{ "WRITEACCESS",			0x005c, &Worker::op_writeaccess },
		{ "WRITEPROT",				0x0086, 0 /* &Worker::op_writeprot */ },
		{ "WSBOOL",					0x0081, 0 /* &Worker::op_wsbool */ },
		{ "XCT",					0x0059, 0 /* &Worker::op_xct */ },
		{ "XF",						0x00e0, &Worker::op_xf },
		{ "XF-OLD",					0x0043, 0 /* &Worker::op_xfold */ },
	};
	
	unsigned int i;
	Method result;

	if(!QString::number(opcode, 16).startsWith("10"))
		opcode = opcode & 0x00FF;

	// Scan lookup table for operation.
	for(i = 0; functions[i].name; i++)
	{
		if(functions[i].opcode == opcode)
			break;
	}

	// Invoke handler.
	result = functions[i].handler;
	if(!result)
	{
		if(functions[i].name)
			kdWarning(30511) << "invokeHandler: unsupported opcode: " << functions[i].name << " operands: " << bytes << endl;
		else
			kdWarning(30511) << "invokeHandler: unsupported opcode: 0x" << QString::number(opcode, 16) << " operands: " << bytes << endl;

		// Skip data we cannot use.
		//skip(bytes, operands);
	}
	else
	{
		kdDebug(30511) << "invokeHandler: opcode: " << functions[i].name << " operands: " << bytes << endl;
		return (m_worker->*result)(bytes, operands);
	}
	
	return true;
}
