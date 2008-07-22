/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
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
#ifndef __KDCHARTATTRIBUTESMODELSERIALIZER_H__
#define __KDCHARTATTRIBUTESMODELSERIALIZER_H__

/** \file KDChartAttributesModelSerializer.h
    \brief Auxiliary methods for reading/saving KD Chart data and configuration in streams.
  */

#include "KDChartAttributesSerializer.h"

#include <KDChartAttributesModel>
#include <KDChartChart>

namespace KDChart {

    class KDCHARTSERIALIZER_EXPORT AttributesModelSerializer : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY( AttributesModelSerializer )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC( AttributesModelSerializer )

    public:
        AttributesModelSerializer();
        virtual ~AttributesModelSerializer();

        virtual bool parseAttributesModel(
                const QDomNode& rootNode,
                const QString& globalName,
                AttributesModel& model )const;

        virtual bool parseAttributesNode(
                const QDomElement& e,
                QVariant& attributes,
                int& role )const;

        virtual void saveAttributesModel(
                QDomDocument& doc,
                QDomElement& e,
                const AttributesModel* model,
                bool isExternalModel )const;
        virtual void createAttributesNode(
                QDomDocument& doc,
                QDomElement& e,
                const AttributesModel* model,
                int role,
                const QVariant& attributes )const;
    };

} // end of namespace

#endif
