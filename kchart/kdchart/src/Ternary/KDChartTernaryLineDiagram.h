/* -*- Mode: C++ -*-

   KDChart - a multi-platform charting engine

   */



/****************************************************************************

 ** Copyright (C) 2005-2007 Klarälvdalens Datakonsult AB.  All rights reserved.

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



#ifndef KDCHARTTERNARYLINEDIAGRAM_H

#define KDCHARTTERNARYLINEDIAGRAM_H



#include "KDChartTernaryCoordinatePlane.h"

#include "KDChartAbstractTernaryDiagram.h"





namespace KDChart {



    /**

      * @brief A TernaryLineDiagram is a line diagram with a ternary coordinate plane

      */

    class KDCHART_EXPORT TernaryLineDiagram : public AbstractTernaryDiagram

    {

        Q_OBJECT

        Q_DISABLE_COPY( TernaryLineDiagram )

        KDCHART_DECLARE_DERIVED_DIAGRAM( TernaryLineDiagram, TernaryCoordinatePlane )



    public:

        explicit TernaryLineDiagram ( QWidget* parent = 0, TernaryCoordinatePlane* plane = 0 );

        virtual ~TernaryLineDiagram();



        void resize (const QSizeF &area);

        void paint (PaintContext *paintContext);



    protected:

        const QPair< QPointF, QPointF > calculateDataBoundaries () const;



    };

}



#endif

