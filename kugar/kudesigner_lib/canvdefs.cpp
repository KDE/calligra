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
#ifndef PURE_QT
#include <klocale.h>
#else
#include <qapplication.h>
#endif

#include "canvdefs.h"

namespace KuDesigner{

    QString insertItemActionName(int rtti)
    {
        switch( rtti ){
            case KuDesignerRttiCanvasLabel:
#ifndef PURE_QT
                return i18n("Insert Label");
#else
                return qApp->translate("candefs", "Insert Label");
#endif
            case KuDesignerRttiCanvasField:
#ifndef PURE_QT
                return i18n("Insert Field");
#else
                return qApp->translate("candefs", "Insert Field");
#endif
            case KuDesignerRttiCanvasSpecial:
#ifndef PURE_QT
                return i18n("Insert Special");
#else
                return qApp->translate("candefs", "Insert Special");
#endif
            case KuDesignerRttiCanvasCalculated:
#ifndef PURE_QT
                return i18n("Insert Calculated Field");
#else
                return qApp->translate("candefs", "Insert Calculated Field");
#endif
            case KuDesignerRttiCanvasLine:
#ifndef PURE_QT
                return i18n("Insert Line");
#else
                return qApp->translate("candefs", "Insert Line");
#endif
            case KuDesignerRttiCanvasReportItem:
            default:
#ifndef PURE_QT
                return i18n("Insert Report Item");
#else
                return qApp->translate("candefs", "Insert Report Item");
#endif
        }
    }

}
