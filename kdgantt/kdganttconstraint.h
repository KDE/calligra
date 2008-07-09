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
#ifndef KDGANTTCONSTRAINT_H
#define KDGANTTCONSTRAINT_H

#include <QModelIndex>
#include <QObject>
#include <QSharedDataPointer>

#include "kdgantt_export.h"
#ifndef QT_NO_DEBUG_STREAM
#include <QDebug>
#endif

namespace KDGantt {
    class KDGANTT_EXPORT Constraint {
        class Private;
    public:
        enum Type
        { 
            TypeSoft = 0, 
            TypeHard = 1
        };	
        enum RelationType
        { 
            FinishStart = 0,
            FinishFinish = 1,
            StartStart = 2,
            StartFinish = 3
        };

        enum ConstraintDataRole
        {
            ValidConstraintPen = Qt::UserRole,
            InvalidConstraintPen
        };

        Constraint( const QModelIndex& idx1,  const QModelIndex& idx2, Type type=TypeSoft, RelationType=FinishStart );
        Constraint( const Constraint& other);
        ~Constraint();

        Type type() const;
        RelationType relationType() const;
        QModelIndex startIndex() const;
        QModelIndex endIndex() const;

        void setData( int role, const QVariant& value );
        QVariant data( int role ) const;

        Constraint& operator=( const Constraint& other );

        bool operator==( const Constraint& other ) const;

        inline bool operator!=( const Constraint& other ) const {
            return !operator==( other );
        }

        uint hash() const;
#ifndef QT_NO_DEBUG_STREAM
        QDebug debug( QDebug dbg) const;
#endif

    private:
        QSharedDataPointer<Private> d;
    };

    inline uint qHash( const Constraint& c ) {return c.hash();}
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<( QDebug dbg, const KDGantt::Constraint& c );
#endif /* QT_NO_DEBUG_STREAM */

#endif /* KDGANTTCONSTRAINT_H */

