/**************************************************************************
						 sqlcursor.cpp  -  description
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
#include <qmessagebox.h> 
#include <qsqldatabase.h>
#include <qsqldriver.h>
#include <qsqlrecord.h>
#include <qstring.h>

#include "sqlcursor.h"

CSqlCursor::CSqlCursor( const QString & strSql, bool autopopulate, QSqlDatabase* db )
	:QSqlCursor( QString::null, autopopulate, db )
{
	Q_ASSERT( !strSql.isEmpty() || !strSql.isNull() );

	if ( !QSqlDatabase::contains() )
	{
		//QMessageBox::warning( 0,
		//	tr( "Database connection Error" ),
		//	tr( "Unable to find any database connection." ) );
		qWarning( "Unable to find any database connection." );
		return;
	}

	exec( strSql );
	if ( isSelect() ) {
		// get the meta data about query object
		QSqlRecordInfo inf = (driver()->recordInfo( *this ) );
		for ( QSqlRecordInfo::iterator it = inf.begin(); it != inf.end(); ++it ) {
			append( *it );
		}	    
	}else {
		// an error occurred if the cursor is not active
		if ( !isActive() ) {
			QSqlError err = lastError();
			QString errStr ( "The database reported an error\n" );
			if ( !err.databaseText().isEmpty() )
				errStr += err.databaseText();
			if ( !err.driverText().isEmpty() )
				errStr += err.driverText();
			//QMessageBox::critical( 0, tr("report engine error"), errStr );
			qWarning( "%s", errStr.local8Bit().data() );
		} else {
			// we have not a select query statement
			//QMessageBox::critical( 0, tr("report engine error"), tr("The string query is not a select sql statement.") );
			qWarning( "The string query is not a select sql statement." );
		}
    }

	setMode( QSqlCursor::ReadOnly );
}

QString CSqlCursor::getXMLValue( int i )
{
	QString val = specialXMLData( value( i ).toString() );
	return fieldName( i ) + "=\"" + val + "\" ";
}

QString CSqlCursor::getXMLValue( const QString& name )
{
#if defined(QT_DEBUG)
	qDebug( "getXMLValue : name: %s, value: %s", name.latin1(), value( name ).toString().latin1() );
#endif
	QString val = specialXMLData( value( name ).toString() );
	return name + "=\"" +  val + "\" ";
}

QString CSqlCursor::specialXMLData(const QString& str)
{
	QString s = str;
    s.replace( "&", "&amp;" );
    s.replace( ">", "&gt;" );
    s.replace( "<", "&lt;" );
    s.replace( "\"", "&quot;" );
    s.replace( "\'", "&apos;" );
    return s;
}
