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

#ifndef KDCHARTHEADERFOOTER_H
#define KDCHARTHEADERFOOTER_H

#include "KDChartTextArea.h"
#include "KDChartPosition.h"

namespace KDChart {

    class Chart;
    class TextAttributes;

class KDCHART_EXPORT HeaderFooter : public TextArea
{
    Q_OBJECT

    Q_DISABLE_COPY( HeaderFooter )
    KDCHART_DECLARE_PRIVATE_DERIVED_PARENT( HeaderFooter, Chart* )

public:
    HeaderFooter( Chart* parent = 0 );
    virtual ~HeaderFooter();

    virtual HeaderFooter * clone() const;

    enum HeaderFooterType{ Header,
                           Footer };

    void setType( HeaderFooterType type );
    HeaderFooterType type() const;

    void setPosition( Position position );
    Position position() const;

    void setParent( QObject* parent );

Q_SIGNALS:
    void destroyedHeaderFooter( HeaderFooter* );
    void positionChanged( HeaderFooter* );

}; // End of class HeaderFooter

}


#endif // KDCHARTHEADERFOOTER_H
