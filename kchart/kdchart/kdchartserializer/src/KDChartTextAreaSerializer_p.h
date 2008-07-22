/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
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

#include <KDChartTextAreaSerializer.h>

namespace KDChart
{
    class HeaderFooter;
}

/**
 * \internal
 */
class KDChart::TextAreaSerializer::Private
{
    friend class ::KDChart::TextAreaSerializer;
    TextAreaSerializer * const q;
public:
    explicit Private( TextAreaSerializer * qq );
    ~Private(); // non-virtual, since nothing inherits this

protected:
    static bool doParseHeaderFooter( const QDomElement& container, HeaderFooter*& ptr );

    static void saveHeaderFooter( QDomDocument& doc, QDomElement& hdFtElement, const KDChart::HeaderFooter& hdFt );

    static bool parseTextArea( const QDomElement& container, TextArea& area );
    static void saveTextArea( QDomDocument& doc, QDomElement& e, const TextArea& area, const QString& title );

    static bool parseTextLayoutItem( const QDomElement& container, TextLayoutItem& item );
    static void saveTextLayoutItem( QDomDocument& doc, QDomElement& e, const TextLayoutItem& item, const QString& title );
};


#endif // KDChartTextAreaSerializer_p_H
