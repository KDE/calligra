/**************************************************************************
						 sqlcursor.h  -  description
							 -------------------
	begin				 : 2003-03-22 23:54:51
	copyright			 : (C) 2003 by Joris Marcillac
	email				 : joris@marcillac.org
**************************************************************************/
/**************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*	This program is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of		  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	  *
*	Library General Public License for more details.					  *
*                                                                         *
*	You should have received a copy of the GNU Library General Public	  *
*	License along with this library; if not, write to the Free			  *
*	Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,	      *
*	MA 02111-1307, USA													  *
*                                                                         *
**************************************************************************/
#if !defined(CSQLCURSOR_H)
#define CSQLCURSOR_H

#include <qsqlcursor.h>

class QSqlDatabase;
class QString;

class CSqlCursor :
	public QSqlCursor
{
public:
	CSqlCursor( const QString & query = QString::null, bool autopopulate = TRUE, QSqlDatabase* db = 0 );
    CSqlCursor( const CSqlCursor & other ): QSqlCursor( other ) {}
	virtual ~CSqlCursor() {}

	bool select( const QString & /*filter*/, const QSqlIndex & /*sort*/ = QSqlIndex() )
	{ return exec( lastQuery() ); }
    QSqlIndex primaryIndex( bool /*prime*/ = TRUE ) const
	{ return QSqlIndex(); }
    int insert( bool /*invalidate*/ = TRUE )
	{ return FALSE; }
	int update( bool /*invalidate*/ = TRUE )
	{ return FALSE; }
    int del( bool /*invalidate*/ = TRUE )
	{ return FALSE; }
    void setName( const QString& /*name*/, bool /*autopopulate*/ = TRUE ) {}

	QString getXMLValue( int i );
	QString getXMLValue( const QString& name );

private:
	QString specialXMLData(const QString& str);
};
#endif // !defined(CSQLCURSOR_H)
