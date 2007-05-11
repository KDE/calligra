/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
#ifndef __KDCHARTTEXTAREASERIALIZER_P_H__
#define __KDCHARTTEXTAREASERIALIZER_P_H__

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

#include <KDChartSerializeCollector.h>

/**
 * \internal
 */
class KDChart::SerializeCollector::Private
{
    friend class ::KDChart::SerializeCollector;
    SerializeCollector * const q;
public:
    explicit Private( SerializeCollector * qq );
    ~Private(); // non-virtual, since nothing inherits this
protected:
    static QString unresolvedTagName();
    static QString unresolvedMapName();

    QMap< QString, QDomElement* > m_map;
    InitializedPointersMap m_initializedPointersMap;
};


#endif // KDChartTextAreaSerializer_p_H
