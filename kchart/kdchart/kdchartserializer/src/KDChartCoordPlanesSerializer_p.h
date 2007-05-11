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
#ifndef __KDCHARTCOORDPLANESSERIALIZER_P_H__
#define __KDCHARTCOORDPLANESSERIALIZER_P_H__

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

#include <KDChartCoordPlanesSerializer.h>

/**
 * \internal
 */
class KDChart::CoordPlanesSerializer::Private
{
    friend class ::KDChart::CoordPlanesSerializer;
    CoordPlanesSerializer * const q;
public:
    explicit Private( CoordPlanesSerializer * qq );
    ~Private(); // non-virtual, since nothing inherits this

protected:
    DiagramsSerializer* m_diagS;
    QAbstractItemModel* m_model;

    void savePlane( QDomDocument& doc, QDomElement& e, const AbstractCoordinatePlane* p ) const;

    bool doParsePlane( const QDomElement& container, AbstractCoordinatePlane* plane ) const;
    
    bool parseAbstractPlane( const QDomElement& container, AbstractCoordinatePlane& plane ) const;
    void saveAbstractPlane( QDomDocument& doc, QDomElement& e, const AbstractCoordinatePlane& plane, const QString& title ) const;

    bool parseCartPlane( const QDomElement& container, CartesianCoordinatePlane& plane ) const;
    void saveCartPlane( QDomDocument& doc, QDomElement& planeElement, const CartesianCoordinatePlane& plane ) const;

    bool parsePolPlane( const QDomElement& container, PolarCoordinatePlane& plane ) const;
    void savePolPlane( QDomDocument& doc, QDomElement& planeElement, const PolarCoordinatePlane& plane ) const;

    bool parseAxesCalcMode( const QDomElement& container, AbstractCoordinatePlane::AxesCalcMode& mode ) const;
    void saveAxesCalcMode( QDomDocument& doc, QDomElement& e, const CartesianCoordinatePlane::AxesCalcMode& mode, const QString& title ) const;
};


#endif // KDChartCoordPlanesSerializer_p_H
