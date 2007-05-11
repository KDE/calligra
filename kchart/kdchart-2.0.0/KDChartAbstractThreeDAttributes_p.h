/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Chart API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef KDCHARTABSTRACTTHREEDATTRIBUTES_P_H
#define KDCHARTABSTRACTTHREEDATTRIBUTES_P_H

#include <KDChartAbstractThreeDAttributes.h>

#include <KDABLibFakes>


namespace KDChart {

/**
 * \internal
 */
class AbstractThreeDAttributes::Private
{
    friend class AbstractThreeDAttributes;
public:
    Private();

private:
    bool enabled;
    int height;
    double depth;
};

inline AbstractThreeDAttributes::AbstractThreeDAttributes( Private * p ) : _d( p ) { init(); }

}

#endif // KDCHARTABSTRACTTHREEDATTRIBUTES_P_H
