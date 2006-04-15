/* This file is part of the KDE project
   Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _SERIALLETTER_QtSql_SQLCURSOR_H_
#define _SERIALLETTER_QtSql_SQLCURSOR_H_

/* FOR THE DIRTY HACK */
#include <q3sqlcursor.h>
#include <qsqldriver.h>
//Added by qt3to4:
#include <QSqlQuery>
/* END FOR THE DIRTY HACK */

/******************************************************************
 *
 * DIRTY HACK FOR SOME INFLEXIBILITY IN QT3's SQL stuff
 *
 * This class is rom some Trolltech guy on QT-interest
 ******************************************************************/



class KWMySqlCursor: public Q3SqlCursor
{
public:
    KWMySqlCursor( const QString & query = QString::null, bool autopopulate = true, QSqlDatabase* db = 0 ): Q3SqlCursor( QString::null, autopopulate, db )
    {
        exec( query );
        if ( autopopulate )
            *(QSqlRecord*)this = ((QSqlQuery*)this)->driver()->record(
*(QSqlQuery*)this );
        setMode( Q3SqlCursor::ReadOnly );
    }
    KWMySqlCursor( const KWMySqlCursor & other ): Q3SqlCursor( other ) {}
    KWMySqlCursor( const QSqlQuery & query, bool autopopulate = true ): Q3SqlCursor( QString::null, autopopulate )
    {
        *(QSqlQuery*)this = query;
        if ( autopopulate )
            *(QSqlRecord*)this = query.driver()->record( query );
        setMode( Q3SqlCursor::ReadOnly );
    }
    bool select( const QString & /*filter*/, const QSqlIndex & /*sort*/ = QSqlIndex() ) { return exec( lastQuery() ); }
    QSqlIndex primaryIndex( bool /*prime*/ = true ) const { return QSqlIndex(); }
    int insert( bool /*invalidate*/ = true ) { return false; }
    int update( bool /*invalidate*/ = true ) { return false; }
    int del( bool /*invalidate*/ = true ) { return false; }
    void setName( const QString& /*name*/, bool /*autopopulate*/ = true ) {}
};


#endif
