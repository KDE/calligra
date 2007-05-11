/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2006 Klarävdalens Datakonsult AB.  All rights reserved.
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

#include <KDChartAbstractDiagram.h>
#include <KDChartDiagramObserver.h>
#include <KDChartAttributesModel.h>

#include <KDABLibFakes>

#include <QDebug>

using namespace KDChart;

DiagramObserver::DiagramObserver( AbstractDiagram * diagram, QObject* parent )
    : QObject( parent ), m_diagram( diagram )
{
    if ( m_diagram ) {
        connect( m_diagram, SIGNAL(destroyed(QObject*)), SLOT(slotDestroyed()));
        connect( m_diagram, SIGNAL(modelsChanged()), SLOT(slotModelsChanged()));
    }
}

DiagramObserver::~DiagramObserver() {}

const AbstractDiagram* DiagramObserver::diagram() const
{
    return m_diagram;
}

AbstractDiagram* DiagramObserver::diagram()
{
    return m_diagram;
}

void DiagramObserver::init()
{
    if ( !m_diagram )
        return;

    if ( m_model )
        disconnect(m_model);

    if ( m_attributesmodel )
        disconnect(m_attributesmodel);

    connect( m_diagram->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             SLOT(slotDataChanged()));
    connect( m_diagram->attributesModel(), SIGNAL(attributesChanged(QModelIndex,QModelIndex)),
             SLOT(slotAttributesChanged()));
    connect( m_diagram->model(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
             SLOT(slotDataChanged()));
    m_model = m_diagram->model();
    m_attributesmodel = m_diagram->attributesModel();
}

void DiagramObserver::slotDestroyed()
{
    emit diagramDestroyed( m_diagram );
}

void DiagramObserver::slotModelsChanged()
{
    init();
    slotDataChanged();
    slotAttributesChanged();
}

void DiagramObserver::slotDataChanged()
{
    emit diagramDataChanged( m_diagram );
}

void DiagramObserver::slotAttributesChanged()
{
    emit diagramAttributesChanged( m_diagram );
}

