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
