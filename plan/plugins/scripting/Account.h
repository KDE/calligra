/* This file is part of the Calligra project
 * Copyright (c) 2008 Dag Andersen <danders@get2net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTING_ACCOUNT_H
#define SCRIPTING_ACCOUNT_H

#include <QObject>
#include <QVariant>


namespace KPlato {
    class Account;
}

namespace Scripting {
    class Project;
    class Account;

    /**
    * The Account class represents an account in a project.
    */
    class Account : public QObject
    {
            Q_OBJECT
        public:
            /// Create a account
            Account( Project *project, KPlato::Account *account, QObject *parent );
            /// Destructor
            virtual ~Account() {}
        
            KPlato::Account *kplatoAccount() const { return static_cast<KPlato::Account*>( m_account ); }
            
        public Q_SLOTS:
            /// Return the project this account is part of
            QObject* project();
            /// Return the accounts name
            QString name() const;
            
            /// Return the number of child accounts
            int childCount() const;
            /// Return the child account at @p index
            QObject *childAt( int index );

            /// Return a map of planned effort and cost pr day for interval @p start to @p end
            QVariant plannedEffortCostPrDay( const QVariant &start, const QVariant &end, const QVariant &schedule );
            /// Return a map of actual effort and cost pr day for interval @p start to @p end
            QVariant actualEffortCostPrDay( const QVariant &start, const QVariant &end, const QVariant &schedule );

            /// Return a map of planned effort and cost pr day
            QVariant plannedEffortCostPrDay( const QVariant &schedule );
            /// Return a map of actual effort and cost pr day
            QVariant actualEffortCostPrDay( const QVariant &schedule );

        private:
            Project *m_project;
            KPlato::Account *m_account;
    };

}

#endif
