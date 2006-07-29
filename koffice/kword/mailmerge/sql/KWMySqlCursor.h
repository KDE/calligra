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
#include <qsqlcursor.h>
#include <qsqldriver.h>
/* END FOR THE DIRTY HACK */

/******************************************************************
 *
 * DIRTY HACK FOR SOME INFLEXIBILITY IN QT3's SQL stuff
 *
 * This class is rom some Trolltech guy on QT-interest
 ******************************************************************/



class KWMySqlCursor: public QSqlCursor
{
public:
    KWMySqlCursor( const QString & query = QString::null, bool autopopulate = 
TRUE, QSqlDatabase* db = 0 ): QSqlCursor( QString::null, autopopulate, db )
    {
        exec( query );
        if ( autopopulate )
            *(QSqlRecord*)this = ((QSqlQuery*)this)->driver()->record(
*(QSqlQuery*)this );
        setMode( QSqlCursor::ReadOnly );
    }
    KWMySqlCursor( const KWMySqlCursor & other ): QSqlCursor( other ) {}
    KWMySqlCursor( const QSqlQuery & query, bool autopopulate = TRUE ): 
QSqlCursor( QString::null, autopopulate )
    {
        *(QSqlQuery*)this = query;
        if ( autopopulate )
            *(QSqlRecord*)this = query.driver()->record( query );
        setMode( QSqlCursor::ReadOnly );
    }
    bool select( const QString & /*filter*/, const QSqlIndex & /*sort*/ = 
QSqlIndex() ) { return exec( lastQuery() ); }
    QSqlIndex primaryIndex( bool /*prime*/ = TRUE ) const { return
QSqlIndex(); }
    int insert( bool /*invalidate*/ = TRUE ) { return FALSE; }
    int update( bool /*invalidate*/ = TRUE ) { return FALSE; }
    int del( bool /*invalidate*/ = TRUE ) { return FALSE; }
    void setName( const QString& /*name*/, bool /*autopopulate*/ = TRUE ) {}
};


#endif
