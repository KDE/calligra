/****************************************************************************
 ** Copyright (C) 2007 Klar�vdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include "KDChartPrintingParameters.h"

using namespace KDChart;

PrintingParameters::PrintingParameters()
    : scaleFactor( 1.0 )
{
}

PrintingParameters* PrintingParameters::instance()
{
    static PrintingParameters instance;
    return &instance;
}

void PrintingParameters::setScaleFactor( const qreal scaleFactor )
{
    instance()->scaleFactor = scaleFactor;
}

void PrintingParameters::resetScaleFactor()
{
    instance()->scaleFactor = 1.0;
}

QPen PrintingParameters::scalePen( const QPen& pen )
{
    if( instance()->scaleFactor == 1.0 )
        return pen;

    QPen resultPen = pen;
    resultPen.setWidthF( resultPen.widthF() * instance()->scaleFactor  );
    if( resultPen.widthF() == 0.0 )
        resultPen.setWidthF( instance()->scaleFactor );

    return resultPen;
}
