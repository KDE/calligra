/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2006 Klaralvdalens Datakonsult AB.  All rights reserved.
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
#ifndef __KDCHARTDIAGRAMSSERIALIZER_P_H__
#define __KDCHARTDIAGRAMSSERIALIZER_P_H__

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

#include <KDChartDiagramsSerializer.h>

/**
 * \internal
 */
class KDChart::DiagramsSerializer::Private
{
    friend class ::KDChart::DiagramsSerializer;
    DiagramsSerializer * const q;
public:
    explicit Private( DiagramsSerializer * qq );
    ~Private(); // non-virtual, since nothing inherits this

protected:
    bool doParseDiagram( const QDomElement& container, AbstractDiagram*& diagramPtr ) const;
    void saveDiagram( QDomDocument& doc, QDomElement& e, const AbstractDiagram* diagram ) const;

    bool parseAbstractDiagram( const QDomElement& container, AbstractDiagram& diagram ) const;
    void saveAbstractDiagram( QDomDocument& doc, QDomElement& e, const AbstractDiagram& diagram, const QString& title ) const;

    bool parseQtProperties( const QDomElement& container, AbstractDiagram& diagram ) const;
    void saveQtProperties( QDomDocument& doc, QDomElement& e, const AbstractDiagram& diagram ) const;

    bool parseCartCoordDiagram( const QDomElement& container, AbstractCartesianDiagram& diagram ) const;
    void saveCartCoordDiagram( QDomDocument& doc, QDomElement& e, const AbstractCartesianDiagram& diagram, const QString& title ) const;

    bool parsePolCoordDiagram( const QDomElement& container, AbstractPolarDiagram& diagram ) const;
    void savePolCoordDiagram( QDomDocument& doc, QDomElement& e, const AbstractPolarDiagram& diagram, const QString& title ) const;

    bool parseAbstractPieDiagram( const QDomElement& container, AbstractPieDiagram& diagram ) const;
    void saveAbstractPieDiagram( QDomDocument& doc, QDomElement& e, const AbstractPieDiagram& diagram, const QString& title ) const;

    bool parseLineDiagram( const QDomElement& container, LineDiagram& diagram ) const;
    void saveLineDiagram( QDomDocument& doc, QDomElement& diagElement, const LineDiagram& diagram ) const;

    bool parsePlotter( const QDomElement& container, Plotter& diagram ) const;
    void savePlotter( QDomDocument& doc, QDomElement& diagElement, const Plotter& diagram ) const;

    bool parseBarDiagram( const QDomElement& container, BarDiagram& diagram ) const;
    void saveBarDiagram( QDomDocument& doc, QDomElement& diagElement, const BarDiagram& diagram ) const;

    bool parsePieDiagram( const QDomElement& container, PieDiagram& diagram ) const;
    void savePieDiagram( QDomDocument& doc, QDomElement& diagElement, const PieDiagram& diagram ) const;

    bool parsePolarDiagram( const QDomElement& container, PolarDiagram& diagram ) const;
    void savePolarDiagram( QDomDocument& doc, QDomElement& diagElement, const PolarDiagram& diagram ) const;

    bool parseRingDiagram( const QDomElement& container, RingDiagram& diagram ) const;
    void saveRingDiagram( QDomDocument& doc, QDomElement& diagElement, const RingDiagram& diagram ) const;

    bool m_haveOwnCoordS;
    CoordPlanesSerializer* m_coordS;
    AxesSerializer* m_axesS;
    AttributesModelSerializer* m_attrModelS;
    mutable QString m_globalList;
};


#endif // KDChartDiagramsSerializer_p_H
