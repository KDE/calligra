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
#ifndef __KDCHARTDIAGRAMOBSERVER_H_
#define __KDCHARTDIAGRAMOBSERVER_H_

#include "KDChartGlobal.h"

#include <QObject>
#include <QPointer>

class QAbstractItemModel;

namespace KDChart {

    class AbstractDiagram;

    /**
     * \brief A DiagramObserver watches the associated diagram for
     * changes and deletion and emits corresponsing signals.
     */
    class KDCHART_EXPORT DiagramObserver : public QObject
    {
        Q_OBJECT
    public:
       /**
         * Constructs a new observer observing the given diagram.
         */
        explicit DiagramObserver( AbstractDiagram * diagram, QObject* parent = 0 );
        ~DiagramObserver();

        const AbstractDiagram* diagram() const;
        AbstractDiagram* diagram();

    Q_SIGNALS:
        /** This signal is emitted immediately before the diagram is
          * being destroyed. */
        void diagramDestroyed( AbstractDiagram* diagram );
        /** This signal is emitted whenever the data of the diagram changes. */
        void diagramDataChanged( AbstractDiagram* diagram );
        /** This signal is emitted whenever the attributes of the diagram change. */
        void diagramAttributesChanged( AbstractDiagram* diagram );

    private Q_SLOTS:
        void slotDestroyed();
        void slotDataChanged();
        void slotAttributesChanged();
        void slotModelsChanged();

    private:
        void init();

        QPointer<AbstractDiagram>    m_diagram;
        QPointer<QAbstractItemModel> m_model;
        QPointer<QAbstractItemModel> m_attributesmodel;
   };
}

#endif // KDChartDiagramObserver_H
