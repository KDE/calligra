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
#ifndef KDGANTTCONSTRAINTMODEL_H
#define KDGANTTCONSTRAINTMODEL_H

#include <QModelIndex>
#include <QDebug>

#include "kdganttglobal.h"
#include "kdganttconstraint.h"

namespace KDGantt {
    class KDGANTT_EXPORT ConstraintModel : public QObject {
        Q_OBJECT
        KDGANTT_DECLARE_PRIVATE_DERIVED_PARENT( ConstraintModel, QObject* )
     public:
        explicit ConstraintModel( QObject* parent=0 );
        virtual ~ConstraintModel();

        void addConstraint( const Constraint& c );
        bool removeConstraint( const Constraint& c );

        void clear();
        void cleanup();

        QList<Constraint> constraints() const;

        bool hasConstraint( const Constraint& c ) const;
        inline bool hasConstraint( const QModelIndex& s, 
                                   const QModelIndex& e ) const;

        QList<Constraint> constraintsForIndex( const QModelIndex& ) const;

    Q_SIGNALS:
        void constraintAdded(const Constraint&);
        void constraintRemoved(const Constraint&);

    private:
        Private* _d;
    };

    inline const ConstraintModel::Private* ConstraintModel::d_func() const { return _d; }
    inline ConstraintModel::Private* ConstraintModel::d_func() { return _d; }
    inline bool ConstraintModel::hasConstraint( const QModelIndex& s, const QModelIndex& e ) const {
        return hasConstraint( Constraint( s, e ) );
    }
}

#ifndef QT_NO_DEBUG_STREAM
#include <QDebug>

QDebug operator<<( QDebug dbg, const KDGantt::ConstraintModel& model );
inline QDebug operator<<( QDebug dbg, KDGantt::ConstraintModel* model ) 
{
    return operator<<(dbg,*model);
}

#endif /* QT_NO_DEBUG_STREAM */

#endif /* KDGANTTCONSTRAINTMODEL_H */

