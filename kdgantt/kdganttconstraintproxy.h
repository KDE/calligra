/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Gantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Gantt licenses may use this file in
 ** accordance with the KD Gantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdgantt for
 **   information about KD Gantt Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef KDGANTTCONSTRAINTPROXY_H
#define KDGANTTCONSTRAINTPROXY_H

#include "kdganttglobal.h"

#include <QPointer>

class QAbstractProxyModel;

namespace KDGantt {
    class Constraint;
    class ConstraintModel;

    class ConstraintProxy : public QObject {
        Q_OBJECT
    public:
        explicit ConstraintProxy( QObject* parent = 0 );
        virtual ~ConstraintProxy();

        void setSourceModel( ConstraintModel* src );
        void setDestinationModel( ConstraintModel* dest );
        void setProxyModel( QAbstractProxyModel* proxy );

        ConstraintModel* sourceModel() const;
        ConstraintModel* destinationModel() const;
        QAbstractProxyModel* proxyModel() const;


    private Q_SLOTS:

        void slotSourceConstraintAdded( const Constraint& );
        void slotSourceConstraintRemoved( const Constraint& );

        void slotDestinationConstraintAdded( const Constraint& );
        void slotDestinationConstraintRemoved( const Constraint& );

    private:
        void copyFromSource();

        QPointer<QAbstractProxyModel> m_proxy;
        QPointer<ConstraintModel> m_source;
        QPointer<ConstraintModel> m_destination;
    };
}

#endif /* KDGANTTCONSTRAINTPROXY_H */

