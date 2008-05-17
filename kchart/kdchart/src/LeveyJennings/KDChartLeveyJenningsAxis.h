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
#ifndef KDCHARTLEVEYJENNINGSAXIS_H
#define KDCHARTLEVEYJENNINGSAXIS_H

#include <QList>

#include "../KDChartCartesianAxis.h"

#include "KDChartLeveyJenningsGridAttributes.h"

namespace KDChart {

    class LeveyJenningsDiagram;

    /**
      * The class for levey jennings axes.
      *
      * For being useful, axes need to be assigned to a diagram, see
      * LeveyJenningsDiagram::addAxis and LeveyJenningsDiagram::takeAxis.
      *
      * \sa PolarAxis, AbstractCartesianDiagram
      */
    class KDCHART_EXPORT LeveyJenningsAxis : public CartesianAxis
    {
        Q_OBJECT

        Q_DISABLE_COPY( LeveyJenningsAxis )
        KDCHART_DECLARE_PRIVATE_DERIVED_PARENT( LeveyJenningsAxis, AbstractDiagram* )

    public:
        /**
          * C'tor of the class for levey jennings axes.
          *
          * \note If using a zero parent for the constructor, you need to call
          * your diagram's addAxis function to add your axis to the diagram.
          * Otherwise, there is no need to call addAxis, since the constructor
          * does that automatically for you, if you pass a diagram as parameter.
          *
          * \sa AbstractCartesianDiagram::addAxis
          */
        explicit LeveyJenningsAxis ( LeveyJenningsDiagram* diagram = 0 );
        ~LeveyJenningsAxis();

        LeveyJenningsGridAttributes::GridType type() const;
        void setType( LeveyJenningsGridAttributes::GridType type );

        /**
         * Returns true if both axes have the same settings.
         */
        bool compare( const LeveyJenningsAxis* other ) const;

        /** reimpl */
        void paintCtx( PaintContext* );

    protected:
        virtual void paintAsOrdinate( PaintContext* );

        virtual void paintAsAbscissa( PaintContext* );
    };

    typedef QList<LeveyJenningsAxis*> LeveyJenningsAxisList;
}

#endif
