/***************************************************************************
                          candefs.h  -  description
                             -------------------
    begin                : 07.06.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef CANVDEFS_H
#define CANVDEFS_H

#include <qapplication.h>
#include <qcanvas.h>
#include <qstringlist.h>

enum MyCanvasRtti { RttiCanvasBox = 1700, RttiCanvasSection, RttiCanvasBand,
	   RttiKugarTemplate=1800, RttiReportHeader, RttiPageHeader, RttiDetailHeader,
	   RttiDetail, RttiDetailFooter, RttiPageFooter, RttiReportFooter,
	   RttiTextBox=2000, RttiCanvasReportItem, RttiCanvasLabel, RttiCanvasField,
	   RttiCanvasSpecial, RttiCanvasCalculated, RttiCanvasLine};

#endif
