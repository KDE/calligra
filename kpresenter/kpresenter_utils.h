/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Utilities (header)                          */
/******************************************************************/

#ifndef _kpresenter_utils_h__
#define _kpresenter_utils_h__

#include <kpoint.h>
#include <qcolor.h>

#include "global.h"

class QPainter;

void drawFigure( LineEnd figure, QPainter* painter, KPoint coord, QColor color, int _w, float angle );
KSize getBoundingSize( LineEnd figure, int _w );

#endif
