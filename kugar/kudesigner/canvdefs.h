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
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
#ifndef CANVDEFS_H
#define CANVDEFS_H

#include <qapplication.h>
#include <qcanvas.h>
#include <qstringlist.h>

enum KuDesignerCanvasRtti { KuDesignerRttiCanvasBox = 1700, KuDesignerRttiCanvasSection, KuDesignerRttiCanvasBand,
    KuDesignerRttiKugarTemplate=1800, KuDesignerRttiReportHeader, KuDesignerRttiPageHeader, KuDesignerRttiDetailHeader,
    KuDesignerRttiDetail, KuDesignerRttiDetailFooter, KuDesignerRttiPageFooter, KuDesignerRttiReportFooter,
    KuDesignerRttiTextBox=2000, KuDesignerRttiCanvasReportItem, KuDesignerRttiCanvasLabel, KuDesignerRttiCanvasField,
    KuDesignerRttiCanvasSpecial, KuDesignerRttiCanvasCalculated, KuDesignerRttiCanvasLine};

/**
    Holder size for report items. TODO: configurable
*/
const int HolderSize = 6;    
    
#endif
