/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart1 licenses may use this file in
 ** accordance with the KDChart1 Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart1 Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef __KDCHARTABSTRACTSERIALIZER_H__
#define __KDCHARTABSTRACTSERIALIZER_H__

/** \file KDChartAbstractSerializer.h
  */

#include <KDChartGlobal>

#include "kdchartserializer_export.h"

class QDomDocument;
class QDomElement;
class QDomNode;
class QObject;

namespace KDChart {

    class KDCHARTSERIALIZER_EXPORT AbstractSerializer
    {
    public:
        virtual ~AbstractSerializer() {}

        virtual void saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const = 0;
        virtual bool parseElement( const QDomElement& container, QObject* ptr ) const = 0;
    };

} // end of namespace

#endif
