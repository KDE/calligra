/***************************************************************************
                          candefs.cpp  -  description
                             -------------------
    begin                : 05.04.2003
    copyright            : (C) 2003 by Alexander Dymo
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

#include <klocale.h>

#include "canvdefs.h"

namespace KuDesigner{

    QString insertItemActionName(int rtti)
    {
        switch( rtti ){
            case KuDesignerRttiCanvasLabel:
                return i18n("Insert Label");
            case KuDesignerRttiCanvasField:
                return i18n("Insert Field");
            case KuDesignerRttiCanvasSpecial:
                return i18n("Insert Special");
            case KuDesignerRttiCanvasCalculated:
                return i18n("Insert Calculated Field");
            case KuDesignerRttiCanvasLine:
                return i18n("Insert Line");
            case KuDesignerRttiCanvasReportItem:
            default:
                return i18n("Insert Report Item");
        }
    }

}
